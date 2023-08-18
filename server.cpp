/* run using ./server <port> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include<queue>
#include "http_server.cpp"



#define THREAD_POOL_SIZE 10
#define REQUEST_LENGTH 256
#define RESPONSE_BUFFER_LENGTH 10000
using namespace std;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_t threads[THREAD_POOL_SIZE];
queue<int> q;

void * handler(void *){
	  char buffer[2000];
	while(true) {
		int SockFd = 0;
		pthread_mutex_lock(&mutex);		
		while(q.empty()) {
      SockFd = 0;
			pthread_cond_wait(&cond, &mutex);	
		}
    SockFd = q.front();
    q.pop();
		pthread_mutex_unlock(&mutex);
		if(SockFd != 0) {
			// we have a connection 
	  // while(1){
	    bzero(buffer, 2000);
      read(SockFd, buffer, 1999);
      string req(buffer,strlen(buffer));
  	  HTTP_Response *res = handle_request(req);
      string s = res->get_string();
      write(SockFd,res->get_string().c_str(), s.length());
      close(SockFd);
      delete res;
	  // write(SockFd, buffer, 18);
	  // }
		}
	}
}

void error(char *msg) {
  perror(msg);
  exit(1);
}


int main(int argc, char *argv[]) {
  int sockfd, SockFd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  int opt =1;
  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  /* create socket */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  if (setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
  }
  /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
   */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind socket to this port number on this machine */

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
   error("ERROR on binding");

  /* listen for incoming connection requests */

  listen(sockfd, 1000);
  clilen = sizeof(cli_addr);

for(int i=0; i< THREAD_POOL_SIZE; i++) {
		pthread_create(&threads[i], NULL, handler, NULL);
	}
  while(1){
    SockFd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    pthread_mutex_lock(&mutex);
	  q.push(SockFd);
	  pthread_cond_signal(&cond);
	  pthread_mutex_unlock(&mutex);
  }
  return 0;
}

