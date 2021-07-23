#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <vector>
#include <stdio.h>
extern void listen_connect(int sfd);
static int sfd;
void init_server(int port)//init handshake socket
{
    sfd = socket(AF_INET,SOCK_STREAM,0);//INET4
    printf("main@create socket: %s\n",strerror(errno));
    
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr))) printf("ERR!");
    printf("main@bind: %s\n",strerror(errno));

    if(listen(sfd,1)) printf("ERR!");
    printf("main@listen: %s\n",strerror(errno));
    listen_connect(sfd);
}
extern std::vector<int> active_cfd;
void destory_sfd(int i )
{
    shutdown(sfd,SHUT_RDWR);
    close(sfd);
    printf("main@close sfd: %d$%s\n",sfd,strerror(errno));
    for (size_t i = 0; i < active_cfd.size(); i++)
    {
        shutdown(active_cfd[i],SHUT_RDWR);
        close(active_cfd[i]);
        printf("main@close cfd: %d$%s\n",active_cfd[i],strerror(errno));
    }
    
    _exit(-1);
}
