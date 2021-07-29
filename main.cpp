#include <iostream>
#include <exception>
#include <server.hpp>
#include <signal.h>
#include"http.h"
#include "server.hpp"

int main()
{
        printf("README!!!!!this version of code has a serious problem: \nafter client close socket,it makes main proc fall into a loop and cause mem leak!\n");
        printf("README!!!!!this version of code has a serious problem: \nthe safe of code is unknow!it's just a tool for study, do not use it for any real important task!\n");
        printf("After read every words,please press any key\n");
        scanf("%c");
   /* auto empty = new std::vector<std::string>;
    ldanar::http::npos = *empty;*/
    try
    {
        signal(SIGINT,ldanar::destory_sfd);
        ldanar::init_server(8010);
    }
    catch(std::exception e)
    {
        printf("WHAT THE F**K!?");
    }
}





