#include <http.hpp>
#include <ctlchr.hpp>
#include <unistd.h> 
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <climits>
std::vector<std::string> npos;
std::vector<std::string> request::find_option(std::string subj)//simply,so there is no commit
{
    printf("obj_size%lu\n", this->obj.size());
    for (size_t i = 0; i < this->obj.size(); i++)
    {
        printf("Cmp...:%s and %s\n", this->obj[i].subj.c_str(), subj.c_str());
        if (this->obj[i].subj == subj)
        {
            return this->obj[i].info;
            printf("Found!");
        }
    }
    printf("npos!");
    return npos;
}
/********************************************************************/
std::string build_response(response res)
{
    std::string retval;
    retval += res.ver + SPACE + std::to_string(res.stat) + stat_array[(res.stat) / 100][(res.stat) % 100] + DSL;
    for (size_t i = 0; i < res.obj.size(); i++)
    {
        retval += res.obj[i].subj + ":";
        for (size_t l = 0; l < res.obj[i].info.size(); l++)
        {
            retval += res.obj[i].info[l];
            if (l != 0)
                retval += ",";
        }

        retval += DSL;
    }
    retval += DSL;
    return retval;
};

std::vector<http_req_obj> deatch_request(std::string str)
{
    std::vector<http_req_obj> retval;
    size_t pos, lpos;
    lpos = 0;
    for (pos = 0; str[pos] != '\n' && pos < str.size(); pos++) //skip the first line
        ;
    pos++;
    lpos = pos;
    std::vector<std::string> lines;
    while (1) //deatch each line
    {
        if (str[pos] == '\r')
            break;
        for (; str[pos] != '\n' && pos < str.size(); pos++) 
            ;
        lines.push_back(str.substr(lpos, pos - lpos - 1));
        pos++;
        lpos = pos;
    }
    for (size_t i = 0; i < lines.size(); i++)
    {
        http_req_obj obj;
        obj.subj = lines[i].substr(0, lines[i].find(':'));//got subject
        pos = lines[i].find(':') + 1;
        for (; lines[i][pos] != ' ' && pos < lines[i].size(); pos++) //skip spaces
            ;
        lpos = pos;
        while (1)//got infos
        {
            if (pos >= lines[i].size())
                break;
            for (; lines[i][pos] != ',' && pos < lines[i].size(); pos++) //read it one by one
                ;
            obj.info.push_back(lines[i].substr(lpos, pos - lpos));
            pos++;
            lpos = pos;
        }
        retval.push_back(obj);
    }

    return retval;
}
request analyse_request(std::string str)
{
    request retval;
    if (str.find("GET") == std::string::npos) //we can only analyse GET requests now
    {
        printf("DEBUG:MUNKNOW\n");
        retval.method = UNKNOWN;
        return retval;
    }
    retval.method = GET;
    retval.url = str.substr(str.find("/"), str.find("H") - str.find("/") - 1);
    retval.obj = deatch_request(str);
    return retval;
}
response respond(request rq)
{
    response retval;
    retval.stat = 200;
    retval.flags = rq.flags;
    if (rq.method == UNKNOWN)
    {
        retval.stat = 413;
        printf("DEBUG:413 at file!\n");
        return retval;
    }
    if (rq.url == "/")
        rq.url = "/index.html";
    rq.url = rq.url.substr(1);
    std::ifstream fin(rq.url);
    printf("DEBUG:trying to open @%s@\n", rq.url.c_str());

    fin.seekg(0, std::ios::end ); // 设置指针到文件流尾部
    std::streampos ps = fin.tellg();
    fin.seekg(0);

    auto rg = rq.find_option("Range");
    if (rg != npos)
    {
        printf("Range Found:%s\n", rg[0].c_str());
        retval.flags |= _RCB_DEATCH_SEG;
        if (rg[0].find("bytes") == std::string::npos)
        {
            retval.server = "ladanar_dev";
            retval.ver = "HTTP/1.1";
            retval.stat = 406;
            return retval;
        }
        printf("start:%s@\n", rg[0].substr(rg[0].find('=')+1,rg[0].find('-')-rg[0].find('=')-1).c_str());
        retval.dseg.start = std::stol(rg[0].substr(rg[0].find('=')+1,rg[0].find('-')-rg[0].find('=')-1));
        

        if (rg[0].find('-') >= rg[0].size()-1)
            retval.dseg.end = 0;
        else
        {
            retval.dseg.end = stol(rg[0].substr(rg[0].find('-') + 1));
        }
        retval.stat = 206;
    }
    printf("DEBUG:file len%d\n", (int)ps);
    if (ps <= 0)
    {
        retval.stat = 404;
        printf("DEBUG:404 at file!\n");
        retval.server = "ladanar_dev";
        retval.ver = "HTTP/1.1";
        return retval;
    }
    if ((int)ps > MX_TRANS_SIZE && !(retval.flags & _RCB_DEATCH_SEG))
    {
        /*printf("%d>@touched bigfile\n", gettid());*/
        retval.flags |= _RCB_DEATCH_SEG;
        //retval.flags |= _RCB_BIG_FILE;
        /*retval.dseg.start = 0;*/
        retval.dseg.end = 0;
        /*//http_resp_obj obj;
        //obj.subj = "Transfer-Encoding";
        //obj.info.push_back("chunked");
        //retval.obj.push_back(obj);
        retval.stat = 206;*/
    }
    http_resp_obj tmp;
    tmp.subj = "Content-Type";
    for (size_t i = 0; i < NR_MIME; i++)
    {
        if (mime_type[i][0] == (rq.url.substr(rq.url.find_last_of(".") + 1)))
            tmp.info.push_back(mime_type[i][1]); //mime
    };
    retval.obj.push_back(tmp);
    printf("%d>@started READ\n", gettid());
    if(!(retval.flags & _RCB_BIG_FILE))// now the session will take its place
    {
        printf("%d>@DONOT PREREAD\n", gettid());
        char *bf = (char *)malloc((int)ps + 8);
        fin.read(bf, ps);
        retval.data = bf;
    }
    retval.datalen = ps;
    retval.server = "ladanar_dev";
    retval.ver = "HTTP/1.1";
    fin.close();
    return retval;
}