#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30

void error_handling(char* message);

int main(int argc, char* argv[])
{
    int serv_sd, clnt_sd;
    FILE * fp;      
    char buf[BUF_SIZE];
    int read_cnt;

    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;

    if(argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);        // 아무 정수나 사용가능 
    }

    fp = fopen("file_server.c","rb");       // t형식 원본 변형이 될 수 있음 // b형식 저장한 그대로 가져올 수 있음
    serv_sd=socket(PF_INET, SOCK_STREAM, 0);        //소켓 생성 운영체제가 관리

    memset(&serv_adr,0,sizeof(serv_adr));       //구조체 초기화
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));

    bind(serv_sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));       //주소할당
    listen(serv_sd, 5);     

    clnt_adr_sz = sizeof(clnt_adr);
    clnt_sd=accept(serv_sd, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);     //데이터 소켓 형성

    while(1)
    {
        read_cnt=fread((void*)buf,1,BUF_SIZE, fp);      //파일 읽기 (void는 역참조 안됨)
        if(read_cnt < BUF_SIZE)
        {
            write(clnt_sd, buf, read_cnt);
            break;
        }
        write(clnt_sd, buf, BUF_SIZE);
    }
    shutdown(clnt_sd, SHUT_WR);     //half-close 사용
    read(clnt_sd, buf, BUF_SIZE);
    printf("Message from client: %s \n",buf);

    fclose(fp);
    close(clnt_sd);
    close(serv_sd);
    return 0;
}

void error_handling(char* message)
{
    fputs(message,stderr);  //표준 에러 2 입력 0 출력 1
    fputc('\n',stderr);
    exit(1);
}