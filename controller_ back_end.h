#ifndef CONTROLLER_BACK_END_H
#define CONTROLLER_BACK_END_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

#include "utils.h"

#define START_CHAR '<'
#define END_CHAR '>'

#define MEAN -160
#define STDDEV 180

typedef struct{
    int socket_fd;
    FILE* file_fd;
}thread_arg_struct;

typedef struct{
    float u1;
    float u2;
} control_signal;

typedef struct{
    float x1;
    float x2;
} states;


void* process_input(void* args);
int parse_message(const char* message, states* x_state, states* y_state, float* time_since_last_output);
int ms_sleep(long msec);
double randn(double mu, double sigma);
float control_law(states state_input);


#endif