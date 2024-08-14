#pragma once
#include <mysql/mysql.h>
#include<string>
#include<sstream>
#include<functional>
#include<cstdlib>
#include"log.hpp"
namespace mysql_util
{
    enum
    {
        INIT_ERROR=1,
        CONNECT_ERROR,

    };
    class mysql
    {
    public:
        mysql()
        {
            using enum Log_util::log_level;
            _root=mysql_init(nullptr);
            if(_root==nullptr)
            {
                LOG(FATAL,"mysql connect error");
                exit(INIT_ERROR);
            }
            LOG(INFO,"mysql init success");
            MYSQL* p=mysql_real_connect(_root,"47.121.132.64","MindbniM","pin0014,.tx3","web",3306,nullptr,0);
            if(p==nullptr)
            {
                LOG(FATAL,"mysql init error");
                mysql_close(_root);
                exit(CONNECT_ERROR);
            }
            LOG(INFO,"mysql connect success");
            int n=mysql_set_character_set(_root,"utf8");
            if(n!=0)
            {
                LOG(ERROR,"mysql set char error");
            }

        }
        std::string exec(const std::string& sql)
        {  
            _mutex.lock();
            using enum Log_util::log_level;
            int n=mysql_query(_root,sql.c_str());
            if(n!=0)
            {
                LOG(ERROR,"sql exe error");
                _mutex.unlock();
                return "-1";
            }
            LOG(INFO,"sql exe success");
            if(mysql_field_count(_root))
            {
                std::string str=get_res();
                _mutex.unlock();
                return str;
            }
            _mutex.unlock();
            return "";
        }
        ~mysql()
        {
            mysql_close(_root);
        }
    private:
        std::string get_res()
        {
            using enum Log_util::log_level;
            MYSQL_RES*res=mysql_store_result(_root);
            if(res==nullptr)
            {
                LOG(ERROR,"get res error");
                return "";
            }
            unsigned int n=mysql_num_fields(res);
            std::ostringstream str;
            MYSQL_ROW w;
            w=mysql_fetch_row(res);
            while(w!=nullptr)
            {
                for(int i=0;i<n;i++)
                {
                    if(w[i]==nullptr)
                    {
                        str<<"NULL";
                    }
                    else str<<w[i];
                    str<<'\t';
                }
                str<<"\n";
                w=mysql_fetch_row(res);
            }
            mysql_free_result(res);
            return str.str();
        }
        MYSQL* _root;
        Thread_util::mutex _mutex;
    };
}