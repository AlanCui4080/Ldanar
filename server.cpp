#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <vector>
#include <stdio.h>
#include "session.hpp"
#include "log.hpp"
#include "session.hpp"
//extern void listen_connect(int sfd);
namespace ldanar
{
static int sfd;
void listen_connect(int sfd)
{
    static struct sockaddr_in conn_addr;
    socklen_t len = sizeof(conn_addr);
    int rvfd, sdfd;

    while (1)
    {
        //int pfd[2] = {0};
        //pipe(pfd);
        //for (size_t i = 0; i < 2; i++);//i don't know why it needs 2 times! but it works!
        //{
        // static handelst* hs = (handelst*)malloc(sizeof(handelst));
        rvfd = accept(sfd, (struct sockaddr *)&conn_addr, &len);
        // hs->fd = rvfd;
        //hs.pipe = pfd[1];
        ldanar::log("accept connect.");
        ldanar::start_session(rvfd,conn_addr);
        /*static pthread_t a; //just a junk
        active_cfd.push_back(rvfd);*
        pthread_create(&a, NULL, http_handle_recv, (void *)(unsigned long long)rvfd);*/

        // yes it's funny,turn a int into a void* and turn it back is not
        //a good idea,but it can solve problems on memory opertions.

        /**************************************************/
        //   hs = (handelst*)malloc(sizeof(handelst));
        /*rvfd = accept(sfd, (struct sockaddr *)&conn_addr, &len);
        //  hs->fd = rvfd;
        //hs.pipe = pfd[1];
        printf("main@accepted recv-cfd:%d:%s\n", rvfd, strerror(errno));

        active_cfd.push_back(rvfd);
        pthread_create(&a, NULL, http_handle_recv, (void *)(unsigned long long)rvfd);*/
        //}

        //Yeah SIR,accept twice can avoid the problems,but while(1) cannot
        //i don't know why,but the fucking CPP is just like this

        /*sdfd = accept(sfd,(struct sockaddr *)&conn_addr,&len);
            hs.fd = sdfd;
            hs.pipe = pfd[0];
            printf("main@accepted send-cfd:%d\n",sdfd);
            //close(pfd[0]);
            //usleep(20);
            pthread_t b;
            active_cfd.push_back(sdfd);
            pthread_create(&a,NULL,handle_recv,hs);*/
    }
}
void init_server(int port)//init handshake socket
{
    sfd = socket(AF_INET,SOCK_STREAM,0);//INET4
    ldanar::log("create socket:%s",strerror(errno));
    
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(server_addr))) printf("ERR!");
    ldanar::log("bind:%s",strerror(errno));

    if(listen(sfd,1)) printf("ERR!");
    ldanar::log("listen:%s",strerror(errno));
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
}