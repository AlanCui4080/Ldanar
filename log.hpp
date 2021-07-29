#ifndef _LOG_H_
#define _LOG_H_
namespace ldanar
{
    class exception : public std::exception
    {
        public:
        bool operator==(const exception &e)
        {
        return &e==this;
        };
    };
    const extern std::exception socket_init_failed;
    const extern std::exception socket_close_failed;
    const extern std::exception session_create_failed;
    const extern std::exception session_close_failed;
    const extern std::exception connect_create_failed;
    const extern std::exception connect_close_failed;
    void log(std::string str,...);
}
[[noreturn]] void _ecatch(std::exception e);
#endif