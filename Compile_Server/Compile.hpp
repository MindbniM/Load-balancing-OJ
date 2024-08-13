#pragma once
#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include"log.hpp"
#include"File_util.hpp"
enum
{
    FILE_CREATE_ERROR=1,
    EXEC_ERROR,
};
class Compile
{
public:
    int static Compile_file(const std::string& file)
    {
        std::string src=util::Path_util::Srcfile(file);
        std::string exe=util::Path_util::Exefile(file);
        std::string err=util::Path_util::Errorfile(file);
        pid_t id=fork();
        if(id<0)
        {
            return 1;
        }
        if(id==0)
        {
            int fd=open(err.c_str(),O_CREAT|O_WRONLY,0666);
            if(fd<0)
            {
                exit(FILE_CREATE_ERROR);
            }
            dup2(fd,2);
            execlp("g++","g++",src.c_str(),"-o",exe.c_str(),nullptr);
            exit(EXEC_ERROR);
        }
        waitpid(id,nullptr,0);
        using enum Log_util::log_level;
        if(util::File_util::IsExist(exe)) 
        {
            LOG(INFO,"编译成功生成:%s",exe.c_str());
            return 0;
        }
        LOG(INFO,"编译失败");
        return -1;
    }
};