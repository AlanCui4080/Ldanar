#ifndef _HTTP_
#define _HTTP_
#include <string>
#include <vector>
    #define MX_TRANS_SIZE 1048576*16 -1
    class http_resp_obj
    {
        public:
        std::string subj;
        std::vector<std::string> info;
    };
    typedef unsigned int http_stat_t;
    typedef std::string http_ver;//HTTP/1.0 (such as)

    typedef unsigned long long flag_t;
    #include "http_bits.h"
    class http_dseg_info
    {
        public:
        size_t start,end;
    };
    class response
    {
        public:
        flag_t flags;
        http_ver ver;
        http_stat_t stat;//return status
        std::string server;//server name 
        std::vector<http_resp_obj> obj;
        size_t datalen;
        void* data;
        http_dseg_info dseg;
    };
    typedef unsigned int http_method;
    enum{
        UNKNOWN,
        GET,
        POST,
    };
    using http_req_obj = http_resp_obj;
    class request
    {
        public:
        flag_t flags;
        http_method method;
        std::string url;
        std::vector<http_req_obj> obj;
        std::vector<std::string> find_option(std::string subj);
        
    };
     extern std::vector<std::string> npos;
std::string build_response(response res);
request analyse_request(std::string str);
response respond(request rq);
#endif