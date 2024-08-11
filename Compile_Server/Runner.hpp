#pragma once
#include<string>
#include<unistd.h>
#include<sys/time.h>
#include<sys/resource.h>
#include"File_util.hpp"
enum Run_err
{
    OPEN_ERR=1,
    EXEC_ERR,
    FORK_ERR,
};
class Runner
{
public:
    static void Set_procLimit(int cpu_time_limit,int mem_limit)
    {
        struct rlimit cpu;
        cpu.rlim_max=RLIM_INFINITY;
        cpu.rlim_cur=cpu_time_limit;
        setrlimit(RLIMIT_CPU,&cpu);
        struct rlimit mem;
        mem.rlim_max=RLIM_INFINITY;
        mem.rlim_cur=mem_limit*1024;
        setrlimit(RLIMIT_AS,&mem);
    }
    static int Run(const std::string& filename)
    {
        std::string exe=util::Path_util::Exefile(filename);
        std::string Stdin=util::Path_util::Stdin(filename);
        std::string Stdout=util::Path_util::Stdout(filename);
        std::string Stderr=util::Path_util::Stderr(filename);
        int in=open(Stdin.c_str(),O_CREAT|O_WRONLY,0666);
        int out=open(Stdout.c_str(),O_CREAT|O_WRONLY,0666);
        int err=open(Stderr.c_str(),O_CREAT|O_WRONLY,0666);
        if(in<0||out<0||err<0)
        {
            return OPEN_ERR;
        }
        //LOG(Log_util::log_level::INFO,"创建临时文件\nstdin:%s\nstdout:%s\nstderr:%s",Stdin.c_str(),Stdout.c_str(),Stderr.c_str());
        pid_t id=fork();
        if(id<0)
        {
            close(in);
            close(out);
            close(err);
            return FORK_ERR;
        }
        if(id==0)
        {
            dup2(in,0);
            dup2(out,1);
            dup2(err,2);
            execl(exe.c_str(),exe.c_str(),nullptr);
            exit(EXEC_ERR);
        }
        close(in);
        close(out);
        close(err);
        int status;
        waitpid(id,&status,0);
        if((status&0x7f)==0)
        {
            LOG(Log_util::log_level::INFO,"%s 运行成功",filename.c_str());
        }
        else 
        {
            LOG(Log_util::log_level::INFO,"%s 出错 sigin: %d",filename.c_str(),(status&0x7f));
        }
        return status&0x7f;
    }
private:
};