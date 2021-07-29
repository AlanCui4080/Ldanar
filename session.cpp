#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include "http.h"
#include "ctlchr.hpp"
#include "session.hpp"
#include "log.hpp"
namespace ldanar
{
    void start_session(int fd, struct sockaddr_in &conn)
    {
        ldanar::basic_session ses;
        auto pid = fork();
        if (pid)
        {
            ldanar::log("start session,pid%d", pid);
            return;
        }
        ldanar::log("session subproc started");
        pthread_t a;
        while(1) // JUST an engerning sample
        {
            thread_switcher sw;
            sw.ses = &ses;
            sw.fd  = fd;
            pthread_create(&a,NULL,ldanar::http::handle_recv,(void*)&sw); // it's not fast at all!!!!!!!!
            pthread_join(a,NULL);
            if(ses.conn_flag & ldanar::session::NEED_CLOSE)
            {
                shutdown(fd,SHUT_RDWR);
                close(fd);
                //if(errno) throw session_close_failed;
                _exit(EXIT_SUCCESS);
            }
        }
    };
};