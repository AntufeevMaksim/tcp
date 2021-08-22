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

std::vector<char> Read(int s, fd_set readfs, timeval tv){
  std::vector<char> buf(10);

  //printf("a");

  int res = select(s+1, &readfs, NULL, NULL, &tv);
  if (res>0){
    int rc = recv(s, buf.data(), 1, 0);
    return (rc == 0 ? std::vector<char>{} : buf);
  }
  else{
    return {};
    //return -1;
  }
}

void PrintAnswer(/*char buf*/ const std::vector<char>& buf){
//  printf("%c\n", buf);
  printw("%s\n", buf.data());
//  printf("%s\n", buf.data());
}


int main(){
  using namespace std::chrono_literals;
	WINDOW* w = initscr();			/* Start curses mode 		*/
  cbreak();
  nodelay(w, TRUE);
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	// noecho();	
  // curs_set(0);

  // int dir = getch();
  // if (dir == ERR){
  //   dir = 258;
  // }

  struct sockaddr_in local;
  int s;
  int rc;
//  char buf;

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



  while (true){

    auto buf = Read(s, readfs, tv);
    if (/*buf!=-1*/ !buf.empty()){
      PrintAnswer(buf);
    }


    std::this_thread::sleep_for(10ms);
    char dir = getch();
    if (dir == 's'){
      break;
    }
    if (dir != ERR){
      rc = send(s, &dir, 1, 0);
      if (rc <= 0){
        perror("error send");
        return 1;
      }   
    }
  }
  //shutdown(listner, SHUT_RDWR);
  return 0;
}
