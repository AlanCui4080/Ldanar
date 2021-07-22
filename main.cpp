#include<iostream>
#include<server.hpp>
#include<signal.h>
int main()
{
    signal(SIGINT,destory_sfd);
    init_server(8010);
}