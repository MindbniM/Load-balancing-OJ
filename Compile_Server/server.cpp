#include<jsoncpp/json/json.h>
#include"Compile_run.hpp"
#include "log.hpp"
#include<httplib.h>
void Usage()
{
    std::cerr<<"\t ./C_server [port]"<<std::endl;
}
int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        Usage();
        return 0;
    }
    ADD_APPENDER_STDOUT(Log_util::log_level::DEBUG);
    httplib::Server sr;
    sr.Post("/Compile_Run",[](const httplib::Request& re,httplib::Response& rp)
    {
        std::string jsonin=re.body;
        std::string jsonout;
        if(!jsonin.empty())
        {
            Compile_run::start(jsonin,jsonout);
            rp.set_content(jsonout,"application/json;chatset=utf-8");
        }
    });
    sr.listen("0.0.0.0",atoi(argv[1]));
    return 0;
}