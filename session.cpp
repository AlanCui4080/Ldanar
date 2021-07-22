#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include "http.hpp"
std::vector<int> active_cfd;
void* handle_session(void* fdn)
{
    int* fd = (int*)fdn;
    char rdata[3260];
    read(*fd,rdata,sizeof(rdata));
    printf("%d@got: \n%s@\n",getpid(),rdata);
    if(((std::string)rdata).find("HTTP") == std::string::npos)
    {
        shutdown(*fd,SHUT_RDWR);
        printf("%d@shutdown cfd: %s\n",getpid(),strerror(errno));
        close(*fd);
        printf("%d@close cfd: %s\n",getpid(),strerror(errno));
        pthread_exit(nullptr);
    }
    auto rq =analyse_request(rdata);
    auto rs = respond(rq);
    auto bat = build_response(&rs);
    char* buf = (char *)malloc(bat.size()+rs.datalen+512);
    strcpy(buf,bat.c_str());
    if(rs.datalen)
        strcpy(buf+bat.size(),(char *)rs.data);
    send(*fd,buf,bat.size()+rs.datalen,0);
    printf("%d@send:@\n %s\n",getpid(),buf);
    free(buf);
    free(rs.data);



    shutdown(*fd,SHUT_RDWR);
    printf("%d@shutdown cfd: %s\n",getpid(),strerror(errno));
    close(*fd);
    printf("%d@close cfd: %s\n",getpid(),strerror(errno));
    for (size_t i = 0; i < active_cfd.size(); i++)
    {
        if(active_cfd[i] == *fd)
        {
            active_cfd.erase(active_cfd.begin()+i);
            pthread_exit(nullptr);
        }
    }
    
}
void listen_connect(int sfd)
{
    struct sockaddr_in conn_addr;
    socklen_t len;
    int cfd,lcfd;
    while(1)
    {
        cfd = accept(sfd,(struct sockaddr *)&conn_addr,&len);
        printf("main@accepted cfd\n");
        if(cfd != lcfd)
        {
            pthread_t a;
            active_cfd.push_back(cfd);
            pthread_create(&a,NULL,handle_session,&cfd);
        }
        lcfd = cfd;
    }
}