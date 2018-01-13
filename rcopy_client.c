#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <netdb.h>
#include "ftree.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef PORT
  #define PORT 30000
#endif

#define TIMES 5 // number of times to send the dest
#define MINCHARS 3
#define MAXCHARS 7

struct fileinfo structify(char* dest)
{
    printf("aefgr\n");
    //get real path
    char dest_abs[MAXPATH];
    realpath(dest, dest_abs);

     //declare struct
    struct fileinfo filestruct;

    //use stat to extract information
    struct stat st;
    stat(dest_abs, &st);
    strncpy(filestruct.path, dest, MAXPATH); //assigning path

    filestruct.mode = st.st_mode; //assigning permissions
    filestruct.size = st.st_size; //assigning size

    if(!(S_ISDIR(filestruct.mode)))
    {
        //open file
        printf("file\n");
        FILE* dest_f;
        dest_f = fopen(dest_abs, "r");
        strncpy(filestruct.hash, hash(dest_f), HASH_SIZE - 1); //assigning hash
        printf("path: %s\n", filestruct.path);
        printf("mode: %ld\n", filestruct.mode);
        printf("size: %ld\n", filestruct.size);
        fclose(dest_f);
        printf("in file area\n");
        return filestruct;
    }
    else{
        //there is no hash
        strcpy(filestruct.hash, "\0"); 
        /*DEBUG*/
        printf("path: %s\n", filestruct.path);
        printf("mode: %ld\n", filestruct.mode);
        printf("size: %ld\n", filestruct.size);
        return filestruct;
    }
}

int setup(char* host, char* dest)
{

    // Setting up the file descriptor
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct fileinfo filestruct;
    filestruct = structify(dest);


    // Setting up the sockaddr_in to connect to
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // We assume the server runs on the same machine as the client 
    // (the IP address 127.0.0.1 always refers to the local machine) 
    if (inet_pton(AF_INET, host, &server.sin_addr)< 0) {
        perror("client: inet_pton");
        close(sock_fd);
        return 1;
    }

    // Connect to the server
    if (connect(sock_fd, (struct sockaddr *) &server, sizeof(server)) == -1) {
        perror("client: connect");
        close(sock_fd);
        return 1;
    }

    printf("I'm connected\n");
    printf("path: %s\n", filestruct.path);
    printf("mode: %ld\n", filestruct.mode);
    printf("size: %ld\n", filestruct.size);
    printf("hash: %s\n", filestruct.hash);

    if(S_ISDIR(filestruct.mode)){
        // Write message to the server
        write(sock_fd, filestruct.path, MAXPATH);
        write(sock_fd, &(filestruct.mode), sizeof(mode_t));
        write(sock_fd, &(filestruct.size), sizeof(size_t));
        write(sock_fd, filestruct.hash, HASH_SIZE);
        int response;
        read(sock_fd, &response, sizeof(int));
        printf("response: %d\n", response);


    } else if (S_ISREG(filestruct.mode)){
        printf("Goes into files\n");
        write(sock_fd, filestruct.path, MAXPATH);
        write(sock_fd, &(filestruct.mode), sizeof(mode_t));
        write(sock_fd, &(filestruct.size), sizeof(size_t));
        write(sock_fd, filestruct.hash, HASH_SIZE);

        // read message from the server into a buffer
        //char buf[128];
        int k;
        int numread = read(sock_fd, k, sizeof(int));
        //buf[numread] = '\0';
        printf("Match  or no Match? %d\n", k);
        if(k == 1){
            // Mismatch case
            


        }
    }

    close(sock_fd);
    return 0;
}

int main(int argc, char** argv) {
  if (argc < 3 || argc > 3)
  {
    printf("Usage: rcopy_client [HOST IP ADDRESS] [SRC]\n");
  } else {
    return setup(argv[1], argv[2]);
  }

  return 0;
}