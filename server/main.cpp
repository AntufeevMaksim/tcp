#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>




int main(){
  struct sockaddr_in local;
  int s;
  int s1;
  int rc;
  char buf [1];

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

  rc = recv(s1, buf, 1, 0);
  if (rc <= 0){
    perror("error recv");
    return 1;
  } 

  printf("%c\n", buf[0]);
  rc = send(s1, "21", 2, 0);
  if (rc <= 0){
    perror("error send");
    return 1;
  }   
  shutdown(s, SHUT_RDWR);
  return 0;

}
