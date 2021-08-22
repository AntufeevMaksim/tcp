#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <poll.h>

char Read(int s, fd_set readfs, timeval tv){
  char buf;

  int res = select(s+1, &readfs, NULL, NULL, &tv);
  if (res>0){
    int rc = recv(s, &buf, 1, 0);
    return buf;
  }
  else{
    return -1;
  }
}




int main(){
  using namespace std::chrono_literals;
  struct sockaddr_in local;
  int s;
  int s1;
  int s2;
  int rc;
  char buf;

  local.sin_family = AF_INET;
  local.sin_port = htons(8888);
  local.sin_addr.s_addr = htonl(INADDR_ANY);

  s = socket( AF_INET, SOCK_STREAM, 0);
  if (s < 0){
    perror("socket error");
    return 1;
  }
  int opt = 1;

  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)) == -1)
    perror("setsockopt");


  rc = bind(s, (struct sockaddr*)&local, sizeof(local));
  if (rc<0){
    perror("error bind");
    return 1;
  }
  
  rc = listen(s, 5);
  if (rc){
    perror("listen error");
    return 1;
  }

  s1 = accept(s, NULL, NULL);
  if (s1 < 0){
    perror("accept error");
    return 1;
  }
  
  s2 = accept(s, NULL, NULL);
  if (s1 < 0){
    perror("accept error");
    return 1;
  }  

  // send(s2, "client 2", 10, 0);
  // char* a = "3";
  // printf(a);
  // rc = send(s1, "Y", 1, 0);
  struct timeval tv;
  fd_set readfs1;
  tv.tv_sec = 1;
  tv.tv_usec = 5000;
  FD_ZERO(&readfs1);
  FD_SET(s1, &readfs1);

  fd_set readfs2;
  FD_ZERO(&readfs2);
  FD_SET(s2, &readfs2);


  while (true){
    // rc = recv(s1, buf, 1, 0);
    // if (rc <= 0){
    //   perror("error recv");
    //   return 1;
    // } 

    std::this_thread::sleep_for(10ms);

    buf = Read(s1, readfs1, tv);

    if (buf!=-1){
      rc = send(s2, &buf, 1, 0);
      if (rc <= 0){
        perror("error send");
        return 1;
      }   
    }


    buf = Read(s2, readfs2, tv);
    if (buf!=-1){
      rc = send(s1, &buf, 1, 0);
      if (rc <= 0){
        perror("error send");
        return 1;
      }
    }
  }

  shutdown(s, SHUT_RDWR);
  return 0;

}
