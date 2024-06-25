#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


// Funzione per verificare se una directory esiste
int directory_exists(const char *path) {
    struct stat info;

    if (stat(path, &info) != 0) {
        return 0; // Directory non esiste
    } else if (S_ISDIR(info.st_mode)) {
        return 1; // Directory esiste
    } else {
        return 0; // Il path esiste ma non è una directory
    }
}

// Funzione per creare directory e tutte le directory necessarie nel path
int create_directories(const char *path) {
    char *temp_path = strdup(path);
    if (temp_path == NULL) {
        perror("Error duplicating path string");
        return -1;
    }

    char *pos = temp_path;
    int result = 0;

    // Itera attraverso ogni directory nel path
    while (*pos != '\0') {
        // Trova il prossimo separatore di directory
        if (*pos == '/') {
            // Temporaneamente termina la stringa qui
            *pos = '\0';

            // Se la directory non esiste, creala
            if (!directory_exists(temp_path)) {
                if (mkdir(temp_path, 0755) != 0 && errno != EEXIST) {
                    perror("Error creating directory");
                    result = -1;
                    break;
                }
            }

            // Ripristina il separatore di directory
            *pos = '/';
        }
        pos++;
    }

    // Controlla l'ultima parte del path
    if (result == 0 && !directory_exists(temp_path)) {
        if (mkdir(temp_path, 0755) != 0 && errno != EEXIST) {
            perror("Error creating directory");
            result = -1;
        }
    }

    free(temp_path);
    return result;
}


void print_usage() {
    printf("Usage: [-a] [server address] [-p] [server port] [-d] [root directory]\n");
}

void few_arguments_management() {
    printf("Error: too few arguments.\n");
    print_usage();
    exit(1);
}

bool is_not_an_option(char* string) {
    return string[0] != '-';
}

void add_name_after_option(int *i, int argc, char* argv[], char **string) {
    
    if((*i)+1 < argc && is_not_an_option(argv[(*i)+1]) && *string == NULL) {
        *string = strdup(argv[(*i)+1]);
        if(string == NULL) {
                perror("Error during memory allocation: ");
                exit(1);
        }

        (*i)++;
    }
}

int setup_server_socket(const char *address, const char *port) {
    int sockfd;
    struct sockaddr_in server_addr;

    // Creazione del socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        return -1;
    }

    // Impostazione dell'indirizzo e della porta del server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    if (inet_pton(AF_INET, address, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sockfd);
        return -1;
    }

    // Bind del socket all'indirizzo e alla porta
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        return -1;
    }

    // Messa in ascolto del socket
    if (listen(sockfd, 10) < 0) {
        perror("Listen failed");
        close(sockfd);
        return -1;
    }

    printf("Server listening on %s:%s\n", address, port);
    return sockfd;
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
                    add_name_after_option(&i, argc, argv, &server_address);
                    break;

                case 'p': 
                    add_name_after_option(&i, argc, argv, &server_port);
                    break;

                case 'd':
                    add_name_after_option(&i, argc, argv, &root_directory);
                    break;
                    
                default: 
                    printf("Error: unkown option");
                    print_usage;
                    exit(1);
                
            }

        }
        
        //a name is not preceded by an option
        else {
            printf("Error: argument before option.\n");
            print_usage();
            exit(1);
        }
    }

    if(!directory_exists(root_directory)) {
        printf("Cannot find directory requested.");
        exit(1);
    }

    int server_sock = setup_server_socket(server_address, server_port);
    if (server_sock < 0) {
        printf("Failed to set up server.\n");
        exit(1);
    }

    // Il server è ora in ascolto per le connessioni in arrivo
    while (1) {
        int client_sock;
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        // Accetta una connessione in arrivo
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Accepted connection from client.\n");
        // Gestisci la connessione del client
        // Aggiungi qui il codice per comunicare con il client

        close(client_sock);
    }

    close(server_sock);


    free(server_address);
    free(server_port);
    free(root_directory);
    return 0;
}