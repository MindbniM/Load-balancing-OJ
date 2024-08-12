#include<jsoncpp/json/json.h>
#include"Compile_run.hpp"
int main()
{
    ADD_APPENDER_STDOUT(Log_util::log_level::DEBUG);
    Json::Value root;
    root["code"]=R"(
    #include<iostream>
    #include<cstdlib>
    int main()
    {
        std::cout<<"what can i sey"<<std::endl;
        return 0;
    }
    )";
    root["input"]="";
    root["cpu_limit"]=3;
    root["mem_limit"]=10240*3;
    Json::FastWriter w;
    std::string in=w.write(root);
    std::string out;
    Compile_run::start(in,out);
    std::cout<<out<<std::endl;
    return 0;
}