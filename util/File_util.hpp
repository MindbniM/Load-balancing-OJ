#pragma once
#include<iostream>
#include<string>
#include<atomic>
#include<sys/types.h>
#include<wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/time.h>
namespace util
{
    namespace Path
    {
        const std::string PATH="../temp/";
        const std::string PATH_EXE=".exe";
        const std::string PATH_ERR="_log.txt";
        const std::string PATH_STDIN="_in.txt";
        const std::string PATH_STDOUT="_out.txt";
        const std::string PATH_STDERR="_err.txt";
    }
    namespace Language
    {
        const std::string CPP(".cpp");
    }
   class Path_util
    {
        // 添加文件后缀
        std::string static Add_suffix(const std::string& filename,const std::string& suffix)
        {
            return Path::PATH+filename+suffix;
        }
    public:
        //可执行文件
        std::string static Exefile(const std::string& filename)
        {
            return Add_suffix(filename,Path::PATH_EXE);
        }
        //编译错误输出文件
        std::string static Errorfile(const std::string& filename)
        {
            return Add_suffix(filename,Path::PATH_ERR);
        }
        //源文件
        std::string static Srcfile(const std::string& filename)
        {
            return Add_suffix(filename,Language::CPP);
        }
        //标准输入
        std::string static Stdin(const std::string& filename)
        {
            return Add_suffix(filename,Path::PATH_STDIN);
        }
        //标准输出
        std::string static Stdout(const std::string& filename)
        {
            return Add_suffix(filename,Path::PATH_STDOUT);
        }
        //标准错误
        std::string static Stderr(const std::string& filename)
        {
            return Add_suffix(filename,Path::PATH_STDERR);
        }
        
        //获得一个唯一文件名
        std::string static get_uinque_name()
        {
            struct timeval v;
            gettimeofday(&v,nullptr);
            std::string str=std::to_string(v.tv_sec*1000+v.tv_usec/1000);
            static std::atomic_uint i(0);
            i++;
            str+="_"+std::to_string(i);
            return str;
        }
    };
    class File_util
    {
    public:
        //判断一个文件是否存在
        bool static IsExist(const std::string& filename)
        {
            struct stat s;
            int n=stat(filename.c_str(),&s);
            return n==0;
        }
        bool static Read(const std::string& filename,std::string& out,bool flag=false)
        {
            out.clear();
            std::ifstream r(filename);
            if(!r.is_open()) return false;
            std::string str;
            while(std::getline(r,str))
            {
                out+=str;
                if(flag) out+='\n';
            }
            return true;
        }
        bool static Write(const std::string& filename,const std::string& code)
        {
            std::ofstream out(filename);
            if(!out.is_open()) return false;
            out.write(code.c_str(),code.size());
            return true;
        }
        bool static Rm(const std::string& filename)
        {
            if(IsExist(filename))
            {
                unlink(filename.c_str());
                return true;
            }
            return false;
        }
    };

}