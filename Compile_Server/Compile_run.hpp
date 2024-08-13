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
    static void remove(const std::string& filename)
    {
        std::string Src=util::Path_util::Srcfile(filename);
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
        std::string UinqFileName;
        int reson = 0;
        int run_code;
        Json::Value out;
        if (code.size() == 0)
        {
            reson = CODE_EMPTY;
            goto END;
        }
        UinqFileName = util::Path_util::get_uinque_name();
        util::File_util::Write(util::Path_util::Srcfile(UinqFileName),code);
        run_code = Compile::Compile_file(UinqFileName);
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
        reson = Runner::Run(UinqFileName,cpulimit,memlimit);
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
            i=util::File_util::Read(util::Path_util::Stderr(UinqFileName),i,true);
            out["stderr"] = i;
        }
        Json::FastWriter wr;
        outjson=wr.write(out);
        remove(UinqFileName);
        LOG(Log_util::log_level::INFO,"%s 删除临时文件",UinqFileName.c_str());
        return true;
    }
};