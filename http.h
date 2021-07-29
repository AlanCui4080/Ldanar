#ifndef _HTTP_
#define _HTTP_
#include <string>
#include <vector>
#define MX_TRANS_SIZE 1048576 * 16 - 1
namespace ldanar
{
    extern std::vector<int> active_cfd;
    //basic type abstract
    template <typename T>
    class basic_abstract
    {
        T val;

    public:
        bool operator==(const basic_abstract &v);
        bool operator==(const T &v);

        basic_abstract<T> operator&(const T &v);
        basic_abstract<T> operator|(const T &v);
        basic_abstract<T> operator+(const T &v);
        basic_abstract<T> operator-(const T &v);
        basic_abstract<T> operator*(const T &v);
        basic_abstract<T> operator/(const T &v);
        basic_abstract<T> operator%(const T &v);
        basic_abstract<T> operator^(const T &v);
        basic_abstract<T> operator~();
        T getbit(const T &v);

        basic_abstract<T> operator=(const T v);

        operator T(){return (T)this->val;};

        basic_abstract(const T &v);
        basic_abstract();
    };
    typedef /*basic_abstract<*/unsigned int/*>*/ method_t;
    //basic status type
    typedef /*basic_abstract<*/unsigned int/*>*/ status_t;
    typedef /*basic_abstract<*/std::string/*>*/ version_t;
    //basic version type
    typedef std::string basic_version;
    typedef unsigned long long flag_t;
    enum flag
    {
        BIG_FILE = (1UL << 0),
        DEATCH_SEG = (1UL << 1)
    };
    //transsion protocol
    enum protocol
    {
        HTTP,
        HTTPS,
        WS,
        WSS
    };
    //basic response type
    class basic_data
    {
    public:
        std::string subj;
        std::vector<std::string> info;
    };
    class basic_packet
    {
        public:
        unsigned int protocol;
        const std::string server = "Ldanar"; //server name
        version_t ver;
    };
    namespace http
    {
        enum method
        {
            UNKNOWN,
            GET,
            POST
        };
        class dseg_info
        {
        public:
            size_t start, end;
        };
        class response : public ldanar::basic_packet
        {
        public:
            std::vector<ldanar::basic_data> obj;
            status_t status;
            size_t datalen;
            dseg_info dseg;
            flag_t flags;
            char *data;
            response(){ this->protocol = protocol::HTTP;};
        
        };
        class request : public ldanar::basic_data, public ldanar::basic_packet
        {
        public:
            flag_t flags;
            std::string url;
            std::vector<basic_data> obj;
            std::vector<std::string> find_option(std::string subj);
            method_t method;
            request(){ this->protocol = protocol::HTTP;};
        };
        extern std::vector<std::string> npos;
        std::string build_response(http::response res);
        http::request analyse_request(std::string str);
        http::response respond(http::request rq);
    }
}
#endif