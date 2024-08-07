#pragma once
#include<string>
namespace util
{
    class Path_util
    {
        const static std::string path;
        const static std::string path_exe;
        const static std::string path_err;
        // 添加文件后缀
        std::string static Add_suffix(const std::string& filename,const std::string& suffix)
        {
            return path+filename+suffix;
        }
    public:
        std::string static Exefile(const std::string& filename)
        {
            return Add_suffix(filename,path_exe);
        }
        std::string static Errorfile(const std::string& filename)
        {
            return Add_suffix(filename,path_err);
        }
        std::string static Srcfile(const std::string& filename,const std::string& type)
        {
            std::string suffix=".";
            suffix+=type;
            return Add_suffix(filename,suffix);
        }
        bool static IsExist(const std::string& filename)
        {

        }
    };
    const std::string Path_util::path("/home/mindbnim/code/OJ/Compile_Server/temp/");
    const std::string Path_util::path_exe=".exe";
    const std::string Path_util::path_err=".log";

}