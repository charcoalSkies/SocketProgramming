#include "define.h"

void *rcv(void *arg);

int main(int argc, char *argv[])
{
    /* will hold the destination addr */ 
    int sockfd;
    struct sockaddr_in dest_addr;
    int rcv_byte;
    char buf[BUFFSIZE];
    char id[BUFFID];
    char pw[BUFFPW];
    char msg[BUFFSIZE];
    char sendMSG[BUFFSIZE];

    pthread_t rcv_thread;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd == -1)
    {
        perror("Client-socket() error!");
        exit(EXIT_FAILURE);
    }
    else printf("Client-socket() sockfd is OK...\n");
    
    /* host byte order */
    dest_addr.sin_family = AF_INET;

    /* short, network byte order */
    dest_addr.sin_port = htons(SERV_PORT);

    /* my_addr.sin_addr.s_addr = inet_addr(SERV_IP); */
    dest_addr.sin_addr.s_addr = inet_addr(SERV_IP);

    /*z zero the rest of the struct */
    memset(&(dest_addr.sin_zero), 0, 8);

    /* connect */
    if(connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Client-connect() error");
        exit(EXIT_FAILURE);
    }
    else printf("Client-connect() is OK...\n\n");

    /* receive INIT_MSG */
    // rcv_byte = recv(sockfd, buf, sizeof(buf), 0);
    // printf("%s\n", buf);
    
    pthread_create(&rcv_thread, NULL, rcv, (void*)sockfd);
    while(1)
    {
        gets(msg);
        sprintf(sendMSG,"%s", msg);
        send(sockfd, sendMSG, strlen(sendMSG) + 1, 0);
        sendMSG[0] = '\0';
    }
    close(sockfd);
    exit(1);
}

void *rcv(void *arg)
{
    int sockfd = (int)arg;
    char buff[BUFFSIZE];
    while(1)
    {
        read(sockfd, buff, sizeof(buff));
        if(strlen(buff) == 0)
        {
            printf("상대방과의 연결이 끊어졌습니다.\n");
            break;
        }
        printf("\r%s\n", buff);
        buff[0] = '\0';
    }
    pthread_exit(0);
    return(0);
}
