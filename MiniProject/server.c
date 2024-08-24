#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <netinet/tcp.h>

#include "./functions/server_constants.h"
#include "./functions/admin.h"

#include "./functions/facultyadmin.h"

#include "functions/student_admin.h"


void connection_handler(int connectionFileDescriptor)
{
    printf("Client has connected to the server!\n");

    char readBuffer[1000], writeBuffer[1000];
    ssize_t readBytes, writeBytes;
    int userChoice;

    writeBytes = write(connectionFileDescriptor, INITIAL_PROMPT, strlen(INITIAL_PROMPT));
    if (writeBytes == -1)
        perror("Error while sending first prompt to the user!");
    else
    {
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
            perror("Error while reading from client");
        else if (readBytes == 0)
            printf("No data was sent by the client");
        else
        {
            userChoice = atoi(readBuffer);
            switch (userChoice)
            {
            case 1:
                // Admin
               admin_operation_handler(connectionFileDescriptor);
                break;
            case 2:
                 //Customer
                faculty_operation_handler(connectionFileDescriptor);
                break;
            case 3:
                student_operation_handler(connectionFileDescriptor);
                break;
            default:
                // Exit
                break;
            }
        }
    }
    printf("Terminating connection to client!\n");
}

int main() {
    struct sockaddr_in serv, cli;
    int sd, nsd;
    char buff[80];
    sd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sd == -1) {
        perror("Error while creating socket: ");
        exit(0);
    }
    printf("client side: socket created successfully \n");
    int reuse = 1;
   
    serv.sin_family = AF_UNIX;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(3372);

    int bind_status = bind(sd, (void*)(&serv), sizeof(serv));

    if (bind_status == -1) {
        perror("Error while binding: ");
        exit(0);
    }

    int listen_status = listen(sd, 5);
    if (listen_status == -1) {
        perror("Error while trying to listen for connections: ");
        exit(0);
    }

    while (1) {
        int sz = sizeof(cli);

        nsd = accept(sd, (void*)(&cli), &sz);

        if (nsd == -1) {
            perror("Error while accepting connection: ");
            close(sd);
            exit(0);
        }

      
        if (!fork()) {
            connection_handler(nsd);
            close(nsd);
            exit(0);
        }

    }
    close(sd);
}
