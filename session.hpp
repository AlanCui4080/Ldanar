#ifndef _SESSION_H_
#define _SESSION_H_
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
namespace ldanar
{
    void start_session(int fd, struct sockaddr_in &conn);
    class basic_session
    {
    public:
        flag_t conn_flag; //SESSION attr
        flag_t req_flag;  //data tranfor attr
        basic_session()
        {
            conn_flag = 0;
            req_flag = 0;
        }
    
    };
    class thread_switcher
    {
        public:
        int fd;
        ldanar::basic_session *ses;
    };
    namespace http
    {
        void *handle_recv(void *fdn);
        class session : ldanar::basic_session
        {
            ldanar::http::request lheader; // Last header  used by HTTP/2
        };
    }
    namespace session
    {
        using http = http::session;
        enum
        {
            NEED_CHG_PROTO = (1UL << 0),
            NEED_CLOSE = (1UL << 0),
        };
    };

}
#endif