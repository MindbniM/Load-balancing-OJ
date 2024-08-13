#include<jsoncpp/json/json.h>
#include"Compile_run.hpp"
#include "log.hpp"
#include<httplib.h>
int main()
{
    ADD_APPENDER_STDOUT(Log_util::log_level::DEBUG);
    httplib::Server sr;
    sr.Get("/Compile_Run",[](const httplib::Request& re,httplib::Response& rp)
    {
        std::string jsonin=re.body;
        std::string jsonout;
        if(!jsonin.empty())
        {
            Compile_run::start(jsonin,jsonout);
            rp.set_content(jsonout,"application/json;chatset=utf-8");
        }
    });
    sr.listen("0.0.0.0",8848);
    return 0;
}