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
#include <stdarg.h>
#include "log.hpp"
const ldanar::exception socket_init_failed;
const ldanar::exception socket_close_failed;
const ldanar::exception session_create_failed;
const ldanar::exception session_close_failed;
const ldanar::exception connect_create_failed;
const ldanar::exception connect_close_failed;
void ldanar::log(std::string str,...)
{
    va_list va;
    va_start(va,str);
    str = "[" + std::to_string(time(NULL)) +"]" + std::to_string(getpid()) + ":"+str+"\n";
    vprintf(str.c_str(),va);
    va_end(va);
}
[[noreturn]] void _ecatch(ldanar::exception e)
{
    if(e == socket_init_failed)
        ldanar::log("faild when trying to init sockets,errno:%s",strerror(errno));
    _exit(EXIT_FAILURE);
}