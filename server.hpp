#ifndef _SERVER_HPP_
#define _SERVER_HPP_
namespace ldanar
{
    void listen_connect(int sfd);
    void init_server(int port);
    void destory_sfd(int i);
}
#endif 