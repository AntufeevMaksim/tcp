#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <iostream>
#include <ncurses.h>
#include <thread>
#include <chrono>
#include <poll.h>
#include <vector>
#include <sys/ioctl.h>

std::vector<char> Read(int s, fd_set readfs, timeval tv){
  std::vector<char> buf(10);
  int n;
  unsigned int m;


  int res = select(s+1, &readfs, NULL, NULL, &tv);
  if (res>0){
    ioctl(s, TIOCINQ, &n);
    int rc = recv(s, buf.data(), n, 0);
    return (rc == 0 ? std::vector<char>{} : buf);
  }
  else{
    return {};
  }
}


// void ChangeColor(int color){
//   init_pair(1, color, COLOR_BLACK);
//   attron(COLOR_PAIR(1));
// }

void PrintAnswer(const std::vector<char>& buf){
//  ChangeColor(COLOR_BLUE);
//  printw("\033[1;31m");
  printw("%s\n", buf.data());
//  refresh();
//  ChangeColor(COLOR_WHITE);
}

void Input(std::vector<char>& input, WINDOW* w){
  char sim = getch();
  if (sim == ERR){
    return;
  }

  input.push_back(sim);
  nodelay(w, FALSE);
  while (true){
    char sim = getch();
    if (sim != '\n'){
      input.push_back(sim);
    }
    else{
      break;
    }
  }
  nodelay(w, TRUE);
  int y = getcury(w);
  wmove(w, y+1, 0);
//  refresh();
  //printw("\n"); 
}





int main(){
  using namespace std::chrono_literals;
	WINDOW* w = initscr();			/* Start curses mode 		*/
  cbreak();
  nodelay(w, TRUE);
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
//  start_color();
  struct sockaddr_in local;
  int s;
  int rc;

  local.sin_family = AF_INET;
  local.sin_port = htons(8888);
  local.sin_addr.s_addr = inet_addr("127.0.0.1");

  s = socket( AF_INET, SOCK_STREAM, 0);
  if (s < 0){
    perror("socket error");
    return 1;
  }

  rc = connect(s, (struct sockaddr*)&local, sizeof(local));
  if (rc){
    perror("error connect");
    return 1;
  }

  struct timeval tv;
  fd_set readfs;
  fd_set writefs;
  tv.tv_sec = 2;
  tv.tv_usec = 5000;
  FD_ZERO(&readfs);
  FD_SET(s, &readfs);



  std::vector<char> input;
  while (true){

    auto buf = Read(s, readfs, tv);
    if (!buf.empty()){
      PrintAnswer(buf);
    }


    std::this_thread::sleep_for(10ms);

    // char dir = getch();
    // if (dir == 's'){
    //   break;
    // }
    Input(input, w);



    if (!input.empty()){
      rc = send(s, input.data(), input.size(), 0);
      input.clear();
      if (rc <= 0){
        perror("error send");
        return 1;
      }   
    }
  }
  return 0;
}
