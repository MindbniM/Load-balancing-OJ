#include "Compile.hpp"
#include "Runner.hpp"
#include <jsoncpp/json/json.h>
//输入: 程序代码, 用户输入
//输出: 状态码, 请求结果, 运行结果
enum
{
    CODE_EMPTY=-1,
    COMPILE_ERROR=-2,
    RUN_ERROR=-3,
    NONE_ERROR=-5,
};
class Compile_run
{
    static void get_status(Json::Value& root,int code)
    {

    }
public:
    static bool start(const std::string& injson,std::string& outjson)
    {
        Json::Value root;
        Json::Reader re;
        re.parse(injson,root);
        std::string code=root["code"].asString();
        std::string input=root["input"].asString();
        int cpulimit=root["cpu_limit"].asInt();
        int memlimit=root["mem_limit"].asInt();
        std::string UinqFileName;
        int reson=0;
        int run_code;
        Json::Value out;
        if(code.size()==0)
        {
            reson=CODE_EMPTY; 
            goto END;
        }
        UinqFileName=util::Path_util::Srcfile(util::Path_util::get_uinque_name());
        run_code=Compile::Compile_file(UinqFileName);
        if(run_code>0)
        {
            reson=NONE_ERROR;
            goto END;
        }
        else if(run_code<0)
        {
            reson=COMPILE_ERROR;
            goto END;
        }
        run_code=Runner::Run(UinqFileName);
        if(run_code<0)
        {
            reson=RUN_ERROR;
            goto END;
        }
        else if(run_code==0)
        {
            reson=run_code;
            goto END;
        }
        else
        {
            reson=NONE_ERROR;
            goto END;
        }
        END:
        out["reason"]=reson;
        get_status(out,reson);
        if(reson==0)
        {
            out["stdout"]=util::File_util::Read(util::Path_util::Stdout(UinqFileName));
            out["stderr"]=util::File_util::Read(util::Path_util::Stderr(UinqFileName));
        }


    }
};