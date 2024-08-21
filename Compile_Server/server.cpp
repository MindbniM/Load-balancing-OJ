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
    ADD_APPENDER_STDOUT(Log_util::log_level::DEBUG);
    //Compile_Java comp;
    //comp.Compile_file("123455","java");
    //Runner_Java R;
    //R.Run("123455",5,1024000,"java");
    if(argc!=2)
    {
        Usage();
        return 0;
    }
    httplib::Server sr;
    sr.Post("/Compile_Run",[](const httplib::Request& re,httplib::Response& rp)
    {
        std::string jsonin=re.body;
        std::string jsonout;
        LOG(Log_util::log_level::INFO,"收到编译运行请求");
        std::cout<<jsonin<<std::endl;
        if(!jsonin.empty())
        {
            Compile_run::start(jsonin,jsonout);
            rp.set_content(jsonout,"application/json;chatset=utf-8");
        }
    });
    sr.listen("0.0.0.0",atoi(argv[1]));
    return 0;
}