#include "define.h"

void UserInformation_LoginS(int fd, char *id, char *pw);
void UserInformation_LoginF(int fd, char *id, char *pw);
void UserInformation_LoginX(int fd, char *id, char *pw);
void *clnt_connection(void *arg); 

pthread_mutex_t g_mutex;
char id[BUFFID];
char pw[BUFFPW];
char recvMSG[BUFFSIZE];
int g_clnt_socks[CLNT_MAX];

int g_clnt_count = 0;
int main(int argc, char *argv[])
{
    /* listen on sock_fd, new connection on new_fd */
    int sockfd, new_fd;
    /* my address information, address where I run this program */
    struct sockaddr_in my_addr;
    /* remote address information */
    struct sockaddr_in their_addr;
    unsigned int sin_size;
    /* buffer */
    int rcv_byte;

    char sendMSG[BUFFSIZE];
    char parent_recvMSG[BUFFID];

    pthread_t t_thread;
    pthread_mutex_init(&g_mutex, NULL);
    //pid_t pid;

    int val = 1;

    /* socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        perror("Server-socket() error!");
        exit(EXIT_FAILURE);
    }
    else printf("Server-socket() sockfd is OK...\n");
    /* host byte order */
    my_addr.sin_family = AF_INET;
    /* short, network byte order */
    my_addr.sin_port = htons(SERV_PORT);
    /* my_addr.sin_addr.s_addr = inet_addr(SERV_IP); */
    my_addr.sin_addr.s_addr = INADDR_ANY;
    /*z zero the rest of the struct */
    memset(&(my_addr.sin_zero), 0, 8);
    /* to prevent 'Address already in use...' */
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof val) < 0)
    {
        perror("setsockopt");
        close(sockfd);
        return -1;
    }
    /* bind */
    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof (struct sockaddr)) == -1)
    {
        perror("Server-bind() error!");
        exit(EXIT_FAILURE);
    }
    else printf("Server-bind() is OK...\n");

    /* listen */
    if(listen(sockfd, BACKLOG) == -1)
    {
        perror("listen() error!");
        exit(EXIT_FAILURE);
    }
    else printf("listen() is OK...\n\n");

    while(1)
    {
          /* ...other codes to read the received data... */
        sin_size = sizeof(struct sockaddr_in);
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        printf("accept() is OK...\n");
        printf("\n");
        pthread_mutex_lock(&g_mutex);
        g_clnt_socks[g_clnt_count++] = new_fd;
        pthread_mutex_unlock(&g_mutex);
        /* send INIT_MSG*/
        send(new_fd, INIT_MSG, strlen(INIT_MSG) + 1, 0);
        pthread_create(&t_thread, NULL, clnt_connection, (void*)new_fd);
    }
    // close(new_fd);
    // close(sockfd);
}

void *clnt_connection(void* arg)
{   
    int new_fd = (int)arg;
    while(1)
    {
        recv(new_fd, recvMSG, sizeof(recvMSG), 0);
        if(strcmp(recvMSG, "\0") == 0)
        {
            printf("FD[%d]의 연결이 끊어졌습니다.\n", new_fd);
            close(new_fd);
            pthread_exit(EXIT_SUCCESS);
        }
        printf("%s\n", recvMSG);                
        // send(new_fd, recvMSG, strlen(recvMSG) + 1, 0);
        // recvMSG[0] = '\0'; 
        send_all_clnt(recvMSG, new_fd);
        recvMSG[0] = '\0';
    }

    pthread_mutex_lock(&g_mutex);
    for(int i = 0; i <g_clnt_count; i++)
    {
        if(new_fd == g_clnt_socks[i])
        {
            for(; i < g_clnt_count -1; i++)
                g_clnt_socks[i] = g_clnt_socks[i+1];
            break;
        }
    }
    pthread_mutex_lock(&g_mutex);
    close(new_fd);
    pthread_exit(EXIT_SUCCESS);
    return NULL;
}

void send_all_clnt(char *msg, int my_sock)
{
    pthread_mutex_lock(&g_mutex);
    for(int i = 0; i < g_clnt_count; i++)
    {
        if(g_clnt_socks[i] != my_sock)
            write(g_clnt_socks[i], msg, strlen(msg)+1);
    }
    pthread_mutex_unlock(&g_mutex);
}