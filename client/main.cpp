#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>




int main(){
  struct sockaddr_in local;
  int s;
  int rc;
  char buf [2];

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
  
  rc = send(s, "1", 1, 0);
  if (rc <= 0){
    perror("error send");
    return 1;
  }   

  rc = recv(s, buf, 2, 0);
  if (rc <= 0){
    perror("error recv");
    return 1;
  } 
  else{
    printf("%c", buf[0]);
    printf("%c\n", buf[1]);
  }
  //shutdown(listner, SHUT_RDWR);
  return 0;
}
