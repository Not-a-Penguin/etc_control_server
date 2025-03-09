#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

int main(int argc, char** argv){

    if(argc < 2){
        fprintf(stderr, "[main]\t Missing file name argument.\n[main]\t Example: './main data.csv'.\n");
        exit(0);
    }

    const char* filename = argv[1];
    FILE* file_fd;
    struct stat sb;

    // If the file already exists, open in append mode
    if(stat(filename, &sb) == 0){
        file_fd = fopen(filename, "a");
    }
    // Else create it
    else{
        printf("[main]\t\t Creating %s \n", filename);
        file_fd = fopen(filename,"w");
        if(file_fd != NULL){
            fprintf(file_fd, "x_x1,x_x2,y_x1,y_x2,u1,u2,time_since_last_output,input_payload_size,output_payload_size\n");
        }
    }

    if(file_fd == NULL){
        perror("[main]\t open() failed");
        return 0;
    }

    create_server(file_fd);
    fprintf(stdout, "[main]\t\t Exiting program.\n");

    return 0;
}