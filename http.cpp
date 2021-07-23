#include<http.hpp>
#include<ctlchr.hpp>
#include<string>
#include<fstream>
#include<iostream>
#include<stdio.h>
#include<climits>
std::string build_response(response* res)
{
    std::string retval;
    retval += res->ver+SPACE+std::to_string(res->stat)+stat_array[(res->stat/100)][(res->stat%100)]+DSL;
    retval += "Server:"+res->server;
    retval += DSL;
    retval += "Connection:close";
    retval += DSL;

    for(size_t i = 0;i < res->obj.size();i++)
    {
        retval +=res->obj[i].subj+":";
        for (size_t l = 0; l < res->obj[i].info.size(); l++)
        {
            retval += res->obj[i].info[l];
            if(l != 0)
                retval += ",";
        }

        retval += DSL;
    }   
    return retval;
};
request analyse_request(std::string str)
{
    request retval;
    if(str.substr(0,str.find(" ")) != "GET");
    {
        printf("DEBUG:MUNKNOW\n");
        retval.method=UNKNOW;
    }
    retval.method=GET;
    if(str.find("H")!=std::string::npos&&str.find("/")!=std::string::npos)retval.url =str.substr(str.find("/"),str.find("H")-str.find("/")-1);
    return retval;
}
response respond(request rq)
{
    response retval;
    if(rq.url == "/") rq.url = "/index.html";
    rq.url = rq.url.substr(1);
    std::ifstream fin(rq.url);
    printf("DEBUG:trying to open @%s@\n",rq.url.c_str());
    fin.seekg(0, std::ios::end);                      // 设置指针到文件流尾部
    std::streampos ps = fin.tellg(); 
    fin.seekg(0);   
    if(ps <=0){retval.stat=404;printf("DEBUG:404 at file!\n");return retval;}
    printf("DEBUG:file len%d\n",(int)ps);
    if(ps > INT_MAX){retval.stat=413;return retval;}
    else{
        http_resp_obj tmp;
        tmp.info.push_back("text/html");
        tmp.subj = "Content-Type";
        retval.obj.push_back(tmp);
        char* bf = (char*)malloc((int)ps+8);
        fin.read(bf,ps);
        retval.data = bf;
        retval.datalen = ps;
    }
    retval.stat=200;
    return retval;
}