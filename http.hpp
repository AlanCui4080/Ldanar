#ifndef _HTTP_
#define _HTTP_
#include <string>
#include <vector>
    class http_resp_obj
    {
        public:
        std::string subj;
        std::vector<std::string> info;
    };
    typedef unsigned int http_stat_t;
    typedef std::string http_ver;//HTTP/1.0 (such as)

    class response
    {
        public:
        http_ver ver = "HTTP/1.1";
        http_stat_t stat = 0;//return status
        std::string server = " ldanar_dev_2107";//server name 
        std::vector<http_resp_obj> obj;
        size_t datalen = 0;
        void* data= nullptr;
    };
    typedef unsigned int http_method;
    enum{
        UNKNOW,
        GET,
        POST,
    };
    class request
    {
        public:
        http_method method = UNKNOW;
        std::string url = "  ";
    };
std::string build_response(response* res);
request analyse_request(std::string str);
response respond(request rq);
#endif