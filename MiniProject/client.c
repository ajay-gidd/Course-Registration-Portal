#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<string.h>
#include<errno.h>
#include<stdbool.h>

void my_client_handle(int sfd);

int main()
{
    struct sockaddr_in serv;
    int sd;

    char buff[80];
    sd=socket(AF_UNIX,SOCK_STREAM,0);
    if(sd==-1){
        perror("Error while creating socket: ");
        exit(1);
    }

    printf("client side: socket created successfully \n");
    serv.sin_family=AF_UNIX;

    serv.sin_addr.s_addr=INADDR_ANY;

    serv.sin_port=htons(3372);

    int status=connect(sd,(void*)(&serv),sizeof(serv));

    if(status==-1){ 
            perror("Error while connecting to server: ");
            exit(1);
    }
    printf("client to server connection successfully established\n");
    my_client_handle(sd);

    close(sd);
}
void my_client_handle(int sfd)
{

    
    char readBuffer[1000], writeBuffer[1000]; 
    ssize_t readBytes, writeBytes;            

    char tempBuffer[1000];

    do
    {
        bzero(readBuffer, sizeof(readBuffer)); 
        bzero(tempBuffer, sizeof(tempBuffer));
        readBytes = read(sfd, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
            perror("Error while reading from client socket!");
        else if (readBytes == 0)
            printf("No error received from server! Closing the connection to the server now!\n");
        else if (strchr(readBuffer, '^') != NULL)
        {
            
            strncpy(tempBuffer, readBuffer, strlen(readBuffer) - 1);
            printf("%s\n", tempBuffer);
            writeBytes = write(sfd, "^", strlen("^"));
            if (writeBytes == -1)
            {
                perror("Error while writing to client socket!");
                break;
            }
        }
        else if (strchr(readBuffer, '$') != NULL)
        {
            
            strncpy(tempBuffer, readBuffer, strlen(readBuffer) - 2);
            printf("%s\n", tempBuffer);
            printf("Closing the connection to the server now!\n");
            break;
        }
        else
        {
            bzero(writeBuffer, sizeof(writeBuffer)); 

            if (strchr(readBuffer, '#') != NULL)
                strcpy(writeBuffer, getpass(readBuffer));
            else
            {
                printf("%s\n", readBuffer);
                scanf("%[^\n]%*c", writeBuffer); 
            }

            writeBytes = write(sfd, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing to client socket!");
                printf("Closing the connection to the server now!\n");
                break;
            }
        }
    } while (readBytes > 0);

    close(sfd);
}
    





