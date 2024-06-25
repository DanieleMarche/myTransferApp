#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void print_usage() {
    printf("Usage: [-a] [server address] [-p] [server port] [-d] [root directory]\n");
}

void few_arguments_management() {
    printf("Error: too few arguments.\n");
    print_usage();
    exit(1);
}

bool is_not_an_option(char* string) {
    if(string[0] != '-') {return true;}
    return false;
}

void add_name_after_option(int *i, int argc, char* argv[], char *string) {
    if((*i)+1 < argc && is_not_an_option(argv[(*i)+1]) && string == NULL) {
        *string = strdup(argv[(*i)+1]);
        if(string == NULL) {
                perror("Error during memory allocation: ");
                exit(1);
        }

    (*i)++;
    }
}

int main(int argc, char* argv[]) {

    char* server_address = NULL;
    char* server_port = NULL; 
    char* root_directory = NULL;

    if(argc < 7) {
        few_arguments_management();
    }

    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == '-') {
            switch(argv[i][1]) {
                case 'a': 
                    add_name_after_option(&i, argc, argv, server_address);
                    break;

                case 'p': 
                    add_name_after_option(&i, argc, argv, server_port);
                    break;

                case 'd':
                    add_name_after_option(&i, argc, argv, root_directory);
                    break;
                    
                default: 
                    printf("Error: unkown option");
                    print_usage;
                    exit(1);
                
            }

        } 
        
        //a name is not preceded by an option
        else {
            printf("Error: argument before option");
            print_usage();
            exit(1);
        }
    }

    return 0;
}