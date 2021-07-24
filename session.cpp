#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include "http.hpp"
#include "ctlchr.hpp"
std::vector<int> active_cfd; //active fd list
//size_t bline = 0;//the size of buff
//size_t loop = 0;
class handelst
{ //handle args list
public:
    int fd;
    //int pipe;
};
/*handle_send(void*) : send thread handle
 * @fdn : handle args list
 */
/*void* handle_send(void* fdn)//send thread handle
{
    int fd = ((handelst*)fdn)->fd;//init vals
    int pipe = ((handelst*)fdn)->pipe;
    printf("%d>%d@started session-send\n",gettid(),fd);

    char junk[8]={0};//waiting for recv-thread
    read(fd,junk,1);

    send(fd,buf,bline,0);

    ///printf("%d>%d@send:@\n %s\n",gettid(),fd,buf);
    free(buf);//releadse

    shutdown(fd,SHUT_RDWR);
    printf("%d>%d@shutdown cfd: %s\n",gettid(),fd,strerror(errno));
    close(fd);
    printf("%d>%d@close cfd: %s\n",gettid(),fd,strerror(errno));
    for (size_t i = 0; i < active_cfd.size(); i++)
    {
        if(active_cfd[i] == fd)
        {
            active_cfd.erase(active_cfd.begin()+i);
            pthread_exit(nullptr);
        }
    }
    return nullptr;
}*/
void *handle_recv(void *fdn)
{
    char *buf; //send buffer,be set by recv
    int fd = (unsigned long long)fdn;
    //free(fdn);
    //int pipe = ((handelst*)fdn)->pipe;
    printf("%d>%d@started session-recv\n", gettid(), fd);
    char rdata[3260];
    read(fd, rdata, sizeof(rdata));
    printf("%d>%d@got: \n%s@\n", gettid(), fd, rdata);
    if (((std::string)rdata).find("HTTP") == std::string::npos /*&& loop < 15*/) //false request throw!
    {
        // loop++;
        // handle_session(fdn);
        shutdown(fd, SHUT_RDWR);
        printf("%d>%d@shutdown cfd: %s\n", gettid(), fd, strerror(errno));
        close(fd);
        printf("%d>%d@close cfd: %s\n", gettid(), fd, strerror(errno));
        //send(fd,buf,bline,0);
        pthread_exit(nullptr);
    }
    auto rq = analyse_request(rdata);
    auto rs = respond(rq);
    auto bat = build_response(rs); //response header
        http_resp_obj obj;
        obj.subj = "Content-Length";
        obj.info.push_back(std::to_string(rs.datalen));
        obj.info.pop_back();
        rs.obj.push_back(obj);
    if (!rs.datalen)
    {
        goto normal;
    }
    else if (rs.flags & _RCB_DEATCH_SEG) //if file break
    {
        printf("%d>%d@started DSEG\n", gettid(), fd);

        //size_t rest = rs.datalen;
        if(rs.dseg.end == 0) rs.dseg.end = rs.dseg.start + MX_TRANS_SIZE;
        if(rs.dseg.end >rs.datalen) rs.dseg.end = rs.datalen;
        size_t tran_len = rs.dseg.end - rs.dseg.start;
        size_t pos = rs.dseg.start;

       /* if (rest < MX_TRANS_SIZE)
            tran_len = rest; //tran_len is length of content,not header
        else
            tran_len = MX_TRANS_SIZE;*/

        obj.subj = "Content-Range";
        obj.info.push_back("bytes " + std::to_string(pos) +
                           "-" + std::to_string(rs.dseg.end-1) +
                           "/" + std::to_string(rs.datalen));
        rs.obj.push_back(obj);
        obj.info.pop_back();
        obj.info.pop_back();
        obj.subj = "Accepect-Ranges";
        obj.info.push_back("bytes");
        rs.obj.push_back(obj);
        auto bat = build_response(rs);

        buf = (char *)malloc(tran_len + 32767 + bat.size());
        strcpy(buf, bat.c_str()); //copy response header

        printf("%d>%d@send:@\n %s\n", gettid(), fd, buf);
        memcpy(buf + bat.size(), (char *)rs.data, tran_len);
        send(fd, buf, tran_len + bat.size(), 0);

        free(rs.data);
        free(buf);
        rs.obj.pop_back();
        rs.obj.pop_back();
        goto clean;
    }
    else if (rs.flags & _RCB_BIG_FILE)
    {
        printf("%d>%d@started BF\n", gettid(), fd);
        buf = (char *)malloc(bat.size() + 512);
        strcpy(buf, bat.c_str()); 
        send(fd, buf, bat.size(), 0);
        printf("%d>%d@send:@\n %s\n", gettid(), fd, buf);
        free(buf);
        size_t tran_len = 0;
        size_t rest = rs.datalen;
        std::ifstream fin(rq.url);
        repeat:
            if (rest < MX_TRANS_SIZE)
                tran_len = rest; //tran_len is length of content,not header
            else
                tran_len = MX_TRANS_SIZE;
            buf = (char*)malloc(tran_len+512);
            fin.read(buf,tran_len);
            send(fd,buf,tran_len,0);
            rest -= tran_len;
            free(buf);
            if(rest) goto repeat;
        fin.close();
        goto clean;
    }
    else
    {
    normal:
        printf("%d>%d@started NORMAL\n", gettid(), fd);
        buf = (char *)malloc(bat.size() + rs.datalen + 512); //alloc the buffer
        strcpy(buf, bat.c_str());                            //copy response header
        memcpy(buf + bat.size(), (char *)rs.data, rs.datalen);
        memset(buf + (bat.size() + rs.datalen), 0, 8); //why?????,
        send(fd, buf, bat.size() + rs.datalen, 0);
        printf("%d>%d@send:@\n %s\n", gettid(), fd, buf);
        free(rs.data);
        free(buf);
    }
    //bline = bat.size()+rs.datalen;

    /*char junk = 'X';
    write(pipe,&junk,1);//make send-thread run!
    close(pipe);

    free(rs.data);//release*/
    clean:
    shutdown(fd, SHUT_RDWR); //shutdown and close socket
    printf("%d>%d@shutdown cfd: %s\n", gettid(), fd, strerror(errno));
    close(fd);
    printf("%d>%d@close cfd: %s\n", gettid(), fd, strerror(errno));
    for (size_t i = 0; i < active_cfd.size(); i++) //remove own fd from active_fd
    {
        if (active_cfd[i] == fd)
        {
            active_cfd.erase(active_cfd.begin() + i);
            pthread_exit(nullptr);
        }
    }

    return nullptr;
}
void listen_connect(int sfd)
{
    struct sockaddr_in conn_addr;
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
        printf("main@accepted recv-cfd:%d:%s\n", rvfd, strerror(errno));

        static pthread_t a; //just a junk
        active_cfd.push_back(rvfd);
        pthread_create(&a, NULL, handle_recv, (void *)(unsigned long long)rvfd);
        // yes it's funny,turn a int into a void* and turn it back is not
        //a good idea,but it can solve problems on memory opertions.

        /**************************************************/
        //   hs = (handelst*)malloc(sizeof(handelst));
        rvfd = accept(sfd, (struct sockaddr *)&conn_addr, &len);
        //  hs->fd = rvfd;
        //hs.pipe = pfd[1];
        printf("main@accepted recv-cfd:%d:%s\n", rvfd, strerror(errno));

        active_cfd.push_back(rvfd);
        pthread_create(&a, NULL, handle_recv, (void *)(unsigned long long)rvfd);
        //}
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