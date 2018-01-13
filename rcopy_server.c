#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include "ftree.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "hash.h"
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include <libgen.h>
/* Internet domain header */

//#define PORT_NUM 57622
#define MAX_BACKLOG 5

int copy_ftree(struct fileinfo* filestruct, int fd);

void serve_client(int fd) {
    char *msg = "Server transmission\r\n";
    char buf[MAXPATH];
   
    int num_read = read(fd, buf, MAXPATH);
    //int second_read = read(fd,buf2,255);
    buf[num_read] = '\0';
    //buf2[second_read] = '\0';
    printf("Server received the following order: %s\n", buf);
    //printf("Server received the second order: %s", buf2);
    write(fd, msg, strlen(msg));
}

int serve_struct(int fd)
{
  //create struct to populate
  struct fileinfo* filestruct = (struct fileinfo*) malloc(sizeof(struct fileinfo));
  
  //declare buffers
  char path_buf[MAXPATH];
  char hash_buf[HASH_SIZE];
  mode_t mode_buf;
  size_t size_buf;

  //FILE* fstream;
  int bytes;

  //read values for each attribute from client
  bytes = read(fd, &path_buf, MAXPATH);
  bytes = read(fd, &mode_buf, sizeof(mode_t));
  bytes = read(fd, &size_buf, sizeof(size_t));
  bytes = read(fd, &hash_buf, HASH_SIZE);

  //populate struct
  strncpy(filestruct->path, path_buf, MAXPATH - 1);
  filestruct->mode = mode_buf;
  filestruct->size = size_buf;
  strncpy(filestruct->hash, hash_buf, HASH_SIZE - 1);

  /*DEBUG*/
/*  printf("path: %s\n", filestruct->path);
  printf("mode: %ld\n", filestruct->mode);
  printf("size: %ld\n", filestruct->size);
  printf("hash: %s\n", filestruct->hash);
  printf("bytes: %d\n", bytes);
*/

  copy_ftree(filestruct, fd);
  return 0;
}

int setup(void) 
{
  int on = 1, status;
  struct sockaddr_in self;
  int listenfd;
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  // Make sure we can reuse the port immediately after the
  // server terminates.
  status = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                      (const char *) &on, sizeof(on));
  if(status == -1) {
    perror("setsockopt -- REUSEADDR");
  }

  self.sin_family = AF_INET;
  self.sin_addr.s_addr = INADDR_ANY;
  self.sin_port = htons(PORT);
  memset(&self.sin_zero, 0, sizeof(self.sin_zero));  // Initialize sin_zero to 0

  printf("Listening on %d\n", PORT);

  if (bind(listenfd, (struct sockaddr *)&self, sizeof(self)) == -1) {
    perror("bind"); // probably means port is in use
    exit(1);
  }

  if (listen(listenfd, 5) == -1) {
    perror("listen");
    exit(1);
  }
  return listenfd;
}

void fcopy_server(int port){
  int listenfd;
  int fd;
  struct sockaddr_in peer;
  socklen_t socklen;
  listenfd = setup();
  // printf("setup done\n");
  socklen = sizeof(peer);
  if ((fd = accept(listenfd, (struct sockaddr *)&peer, &socklen)) < 0) {
    perror("accept");
  } else {

    // printf("New connection on port %d\n", ntohs(peer.sin_port));
    int i = 0;
    int transmit = 0;     
    while(i == 0){
      i = serve_struct(fd);
      transmit = TRANSMIT_OK;
      write(fd, &transmit, sizeof(int));
    }
  }
}

int main(int argc, char const *argv[])
{
    if (argc < 2 || argc > 2)
    {
      printf("Usage: rcopy_server [PORT]\n");
      return 1;
    } else {
      fcopy_server(strtol(argv[1], NULL, 10));
    }

    return 0;
  return 0;
}