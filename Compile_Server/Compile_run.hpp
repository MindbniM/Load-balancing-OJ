#include "Compile.hpp"
#include "Runner.hpp"
#include <jsoncpp/json/json.h>
#include <signal.h>
// 输入: 程序代码, 用户输入
// 输出: 状态码, 请求结果, 运行结果
enum
{
    CODE_EMPTY = -1,
    COMPILE_ERROR = -2,
    NONE_ERROR = -5,
};
class Compile_run
{
    static std::string get_status(int code, const std::string &filename)
    {
        std::string str;
        switch (code)
        {
        case 0:
            return "编译运行成功";
        case CODE_EMPTY:
            return "代码为空";
        case COMPILE_ERROR:
            util::File_util::Read(util::Path_util::Errorfile(filename),str,true);
            return str;
        case NONE_ERROR:
            return "未知错误";
        case SIGABRT:
            return "内存使用超过限制,请检查你的代码的空间复杂度";
        case SIGXCPU:
            return "程序运行超时,请检查你的代码的时间复杂度";
        case SIGFPE:
            return "程序浮点数错误,可能是除0等错误";
        case SIGSEGV:
            return "程序发生段错误,可能是数组越界,野指针等错误";
        default:
            return "debug";
        }
    }
    static void remove(const std::string& filename,const std::string& language)
    {
        std::string Src=util::Path_util::Srcfile(filename,language);
        util::File_util::Rm(Src);

        std::string Exe=util::Path_util::Exefile(filename);
        util::File_util::Rm(Exe);

        std::string Error=util::Path_util::Errorfile(filename);
        util::File_util::Rm(Error);

        std::string Stdin=util::Path_util::Stdin(filename);
        util::File_util::Rm(Stdin);

        std::string Stdout=util::Path_util::Stdout(filename);
        util::File_util::Rm(Stdout);

        std::string Stderr=util::Path_util::Stderr(filename);
        util::File_util::Rm(Stderr);
        if(language=="java")
        {
            std::string Tclass=util::Path_util::Tclass(filename);
            util::File_util::Rm(Tclass);
            std::string Sclass=util::Path_util::Sclass(filename);
            util::File_util::Rm(Sclass);
        }
        
    }
    static Compile::ptr Make_Comp(const std::string& language)
    {
        if(language=="c_cpp") return std::make_shared<Compile_Cpp>();
        else if(language=="python") return std::make_shared<Compile_Py>();
        else if(language=="java") return std::make_shared<Compile_Java>();
        else return nullptr;
    }
    static Runner::ptr Make_Run(const std::string& language)
    {
        if(language=="c_cpp") return std::make_shared<Runner_Cpp>();
        else if(language=="python") return std::make_shared<Runner_Py>();
        else if(language=="java") return std::make_shared<Runner_Java>();
        else return nullptr;
    }
public:
    static bool start(const std::string &injson, std::string &outjson)
    {
        Json::Value root;
        Json::Reader re;
        re.parse(injson, root);
        std::string code = root["code"].asString();
        std::string input = root["input"].asString();
        int cpulimit = root["cpu_limit"].asInt();
        int memlimit = root["mem_limit"].asInt();
        std::string language=root["language"].asString();
        std::string UinqFileName;
        int reson = 0;
        int run_code;
        Json::Value out;
        Compile::ptr Comp=Make_Comp(language);
        Runner::ptr Run=Make_Run(language);
        if (code.size() == 0)
        {
            reson = CODE_EMPTY;
            goto END;
        }
        UinqFileName = util::Path_util::get_uinque_name();
        util::File_util::Write(util::Path_util::Srcfile(UinqFileName,language),code);
        run_code = Comp->Compile_file(UinqFileName,language);
        if (run_code > 0)
        {
            reson = NONE_ERROR;
            goto END;
        }
        else if (run_code < 0)
        {
            reson = COMPILE_ERROR;
            goto END;
        }
        reson = Run->Run(UinqFileName,cpulimit,memlimit,language);
        if (reson < 0)
        {
            reson = NONE_ERROR;
            goto END;
        }

    END:
        out["status"] = reson;
        out["reason"] = get_status(reson, UinqFileName);
        if (reson == 0)
        {
            std::string o;
            util::File_util::Read(util::Path_util::Stdout(UinqFileName),o,true);
            out["stdout"] =o;
            std::string i;
            util::File_util::Read(util::Path_util::Stderr(UinqFileName),i,true);
            out["stderr"] = i;
        }
        Json::FastWriter wr;
        outjson=wr.write(out);
        remove(UinqFileName,language);
        LOG(Log_util::log_level::INFO,"%s 删除临时文件",UinqFileName.c_str());
        return true;
    }
};