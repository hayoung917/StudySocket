#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <fcntl.h>

#define BUF_SIZE 10000

void error_handling(char* message);
void* request_handler(void* arg);

char webpage[] = "HTTP/1.1 200 OK\r\n"
        "Server:Linux Web Server\r\n"
        "Content-Type:text/html; charset=UTF-8\r\n\r\n"
        "<!DOCTYPE html>\r\n"
        "<html><head><title> LHY Web Page </title>\r\n"
        "<style>body{background-color:#FFFF00}</style></head>\r\n"
        "<body><center><h1>TCP/IP Test!</h1><br>\r\n"
        "<img src=\"cat.jpg\"></center></body></html>\r\n";

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_size;
    pthread_t t_id;
    char buf[2048];
    
    if(argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    if(listen(serv_sock, 20)==-1)
        error_handling("listen() error");

    while(1)
    {
	    clnt_adr_size=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr, &clnt_adr_size);
        read(clnt_sock,buf,sizeof(buf)-1);
        
        if(!strncmp(buf,"GET /cat.jpg",12))
        {
            pthread_create(&t_id, NULL, request_handler, (void*)&clnt_sock);
            pthread_detach(t_id);
        }
        else
        {
            write(clnt_sock,webpage,sizeof(webpage));
            close(clnt_sock);
        }
	

    }
    close(serv_sock);
    return 0;
}

void* request_handler(void*arg)
{
    int clnt_sock=*((int*)arg);
    int fp;
    int buf[BUF_SIZE];
    fp=open("cat.jpg",O_RDONLY);

    read(fp,buf,sizeof(buf));
    write(clnt_sock,buf,sizeof(buf));
   
    close(clnt_sock);
    close(fp);
    
    return NULL;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}
