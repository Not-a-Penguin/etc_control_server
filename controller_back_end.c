#include "controller_ back_end.h"

void* process_input(void* args){

    thread_arg_struct* file_descriptors = args;

    int socket_fd = file_descriptors->socket_fd;
    FILE* file_fd = file_descriptors->file_fd;

    int buffer_size = 30;
    char message_buffer[buffer_size];
    int bytes_read, in_payload_size, out_payload_size;
    int parse_ret;

    control_signal out;
    states x_state, y_state;

    float time_since_last_output;
    float* ptr_last_output_time;

    double delayValue;

    while(1){

        bzero(message_buffer, sizeof(message_buffer));
        memset(&x_state, 0, sizeof(x_state));
        memset(&y_state, 0, sizeof(y_state));

        bytes_read = 0;
        bytes_read = read(socket_fd, message_buffer, sizeof(message_buffer));
        
        if(bytes_read < 1){
            perror("[process_input]\t Error reading from socket");
        };

        if(bytes_read == 0){
            printf("[process_input]\t Client disconnected.\n");
            return 0;
        }
        
        // printf("[proocess_input] Got %d bytes from client: %s.\n", bytes_read, message_buffer);

        if(strcmp(message_buffer, "<close>") == 0){
            if(fclose(file_fd) != 0){
                perror("[process_input]\t File failed to close");
            };
            return 0;
        }

        parse_ret = parse_message(message_buffer, &x_state, &y_state, &time_since_last_output);
        bzero(message_buffer, sizeof(message_buffer));

        if(parse_ret !=0){
            fprintf(stderr, "[process_input]\t Message parsing failed: %d.\n", parse_ret);
        }
        else{
            
            in_payload_size = bytes_read;
            out_payload_size = sizeof(message_buffer);

            // fprintf(stdout, "[process_input] Parsed values: x_x1:%f, x_x2:%f, y_x1:%f, y_x2:%f, delta:%f, in payloadsize: %d, out payload size: %d\n", x_state.x1, x_state.x2, y_state.x1, y_state.x2, time_since_last_output, in_payload_size, out_payload_size);
            
            // Message back to microcontroller
            out.u1 = control_law(x_state);
            out.u2 = control_law(y_state);
            sprintf(message_buffer, "<%f,%f>\n", out.u1, out.u2);

            // Simulate lost package
            if(randn(MEAN, STDDEV) > 1.5*STDDEV){
                fprintf(stderr, "[process_input]\t Payload lost.\n");
            }
            else{

                // Simulate delay 
                delayValue = randn(MEAN, STDDEV);
    
                if(delayValue > 0){
                    // fprintf(stdout, "Delaying %.2fms.\n", delayValue);
                    ms_sleep(delayValue);
                }
    
                write(socket_fd, message_buffer, buffer_size);
                
                // Saving to file
                fprintf(file_fd, 
                    "%f,%f,%f,%f,%f,%f,%f,%d,%d\n", 
                    x_state.x1, 
                    x_state.x2, 
                    y_state.x1, 
                    y_state.x2, 
                    out.u1, 
                    out.u2, 
                    time_since_last_output, 
                    in_payload_size, 
                    out_payload_size
                );
            }
        }      
    }
}

int parse_message(const char* message, states* x_state, states* y_state, float* time_since_last_output){

    const char* start = strchr(message, START_CHAR);
    const char* end = strchr(message, END_CHAR);

    if(!start || !end || start > end){
        fprintf(stderr, "[parse_message]\t Invalid message format.\n");
        return -1;
    }

    size_t message_len = end - start - 1;
    char* payload = (char*)malloc(message_len+1);

    if(!payload){
        fprintf(stderr, "[parse_message]\t Memory allocation failed.\n");
        return -2;
    }

    strncpy(payload, start+1, message_len);
    payload[message_len] = '\0';

    int parsed_message = sscanf(payload, "%f,%f,%f,%f,%f", &x_state->x1, &x_state->x2, &y_state->x1, &y_state->x2, time_since_last_output);

    if(parsed_message != 5){
        fprintf(stderr, "[parse_message]\t Parser found less than 5 values.\n");
        return -3;
    }

    free(payload);

    return 0;
}

float control_law(states state_input){

    float output;

    float gains[2] = {2, 3};

    output = gains[0] * state_input.x1 + gains[1] * state_input.x2;

    return output;
}