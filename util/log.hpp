#pragma once
#include <iostream>
#include <thread>
#include <iomanip>
#include <fstream>
#include <string>
#include <chrono>
#include <sstream>
#include <memory>
#include <set>
#include <cstdarg>
namespace Log_util
{
    const size_t BUFFSIZE = 1024;
    // 日志等级
    enum class log_level
    {
        DEBUG = 1,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };
    // 日志事件
    struct log_event
    {
        typedef std::shared_ptr<log_event> ptr;

        log_event(log_level level, const char *file, int32_t m_line, std::thread::id thread_id, uint32_t fiber_id, std::chrono::system_clock::time_point time, const char *str, ...);

        log_level _level;                            // 日志等级
        const char *_fname = nullptr;                // 文件名
        int _line;                                   // 行号
        std::thread::id _tid;                        // 线程id
        int _fid;                                    // 协程id
        std::chrono::system_clock::time_point _time; // 时间
        std::string _message;                        // 消息
    };
    // 日志格式器
    class log_format
    {
    public:
        typedef std::shared_ptr<log_format> ptr;
        std::string format(log_event::ptr event);
    };
    // 日志输出地
    class log_appender
    {
    public:
        log_appender()
        {}
        typedef std::shared_ptr<log_appender> ptr;
        virtual ~log_appender()
        {}
        virtual void log(log_event::ptr event)
        {}
        log_appender(std::string name, log_level mlevel = log_level::DEBUG) : _mlevel(mlevel), _name(name)
        {}
        std::string get_name()
        {
            return _name;
        }

    protected:
        log_format::ptr _format;
        log_level _mlevel;
        std::string _name;
    };
    struct _FUNC_
    {
        bool operator()(const log_appender::ptr &p1, const log_appender::ptr &p2) const
        {
            return p1->get_name() < p2->get_name();
        }
    };
    // 日志器
    class logger
    {
    public:
        typedef std::shared_ptr<logger> ptr;
        static logger *get_logger()
        {
            static logger log;
            return &log;
        }
        void add_appender(log_appender::ptr appender)
        {
            _outs.insert(appender);
        }
        void del_appender(std::string name)
        {
            log_appender::ptr p = std::make_shared<log_appender>(name);
            if (_outs.count(p))
                _outs.erase(p);
        }
        void log(log_event::ptr event,const std::string& fname="ANY")
        {
            for (auto &i : _outs)
            {
                if(fname=="ANY"||fname==i->get_name())
                {
                    i->log(event);
                }
            }
        }

    private:
        logger()
        {}
        std::set<log_appender::ptr, _FUNC_> _outs; // 日志输出地集合
    };
//在所有添加的日志输出地输出
#define LOG(level,str, ...)                                                                                                                                                                          \
    do                                                                                                                                                                                           \
    {                                                                                                                                                                                            \
        Log_util::logger::get_logger()->log(std::make_shared<Log_util::log_event>(level, __FILE__, __LINE__, std::this_thread::get_id(), -1, std::chrono::system_clock::now(), str, ##__VA_ARGS__)); \
    } while (0)
#define FLOG(file,level,str, ...)                                                                                                                                                                          \
    do                                                                                                                                                                                           \
    {                                                                                                                                                                                            \
        Log_util::logger::get_logger()->log(std::make_shared<Log_util::log_event>(level, __FILE__, __LINE__, std::this_thread::get_id(), -1, std::chrono::system_clock::now(), str, ##__VA_ARGS__),file); \
    } while (0)
#define ADD_APPENDER_STDOUT(LEVEL)                                                                   \
    do                                                                                               \
    {                                                                                                \
        Log_util::logger::get_logger()->add_appender(std::make_shared<Log_util::log_stdout>("stdout", LEVEL)); \
    } while (0)
#define ADD_APPENDER_FILEOUT(LEVEL, FILENAME)                                                         \
    do                                                                                                \
    {                                                                                                 \
        Log_util::logger::get_logger()->add_appender(std::make_shared<Log_util::log_fileout>(FILENAME, LEVEL)); \
    } while (0)
#define DEL_APPENDER_FILEOUT(FILENAME)                     \
    do                                                     \
    {                                                      \
        Log_util::logger::get_logger()->del_appender(FILENAME); \
    } while (0)
#define DEL_APPENDER_STDOUT()                              \
    do                                                     \
    {                                                      \
        Log_util::logger::get_logger()->del_appender("stdout"); \
    } while (0)

    // 向标准输出输出
    class log_stdout : public log_appender
    {
    public:
        typedef std::shared_ptr<log_stdout> ptr;
        virtual void log(log_event::ptr event) override;
        log_stdout(std::string name, log_level mlevel = log_level::DEBUG) : log_appender(name, mlevel)
        {}

    private:
    };
    // 向文件输出
    class log_fileout : public log_appender
    {
    public:
        typedef std::shared_ptr<log_fileout> ptr;
        log_fileout(const std::string &fname, log_level mlevel = log_level::DEBUG) : log_appender(fname, mlevel)
        {
            _file.open(fname, std::ios::app);
        }
        virtual void log(log_event::ptr event) override;
        ~log_fileout()
        {
            if (_file.is_open())
            {
                _file.close();
            }
        }

    private:
        std::ofstream _file;
    };

    log_event::log_event(log_level level, const char *file, int line, std::thread::id thread_id, uint32_t fiber_id, std::chrono::system_clock::time_point time, const char *str, ...)
        : _level(level), _fname(file), _line(line), _tid(thread_id), _fid(fiber_id), _time(time)
    {
        va_list va;
        va_start(va, str);
        char buff[BUFFSIZE] = {0};
        vsnprintf(buff, BUFFSIZE, str, va);
        _message = buff;
        va_end(va);
    }
    void log_stdout::log(log_event::ptr event)
    {
        if (event->_level >= _mlevel)
        {
            std::cout << _format->format(event) << std::endl;
        }
    }
    void log_fileout::log(log_event::ptr event)
    {
        if (!_file.is_open())
        {
            std::cout << "文件打开失败" << std::endl;
            return ;
        }
        if (event->_level >= _mlevel)
        {
            _file << _format->format(event) << std::endl;
        }
    }
#define ADD_LEVEL(str, level) \
    case log_level::level:    \
        str += #level;        \
        break
    std::string log_format::format(log_event::ptr event)
    {
        std::string str = "\n[";
        switch (event->_level)
        {
            ADD_LEVEL(str, DEBUG);
            ADD_LEVEL(str, INFO);
            ADD_LEVEL(str, WARNING);
            ADD_LEVEL(str, ERROR);
            ADD_LEVEL(str, FATAL);
        }
        str += "]";
        std::time_t time_t = std::chrono::system_clock::to_time_t(event->_time);

        // 使用 std::localtime 将 std::time_t 转换为 struct tm
        std::tm *tm = std::localtime(&time_t);
        // 使用 std::ostringstream 和 std::put_time 将 struct tm 转换为时间字符串
        std::ostringstream oss;
        oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
        str += "[" + oss.str() + "]";
        str += "[" + std::string(event->_fname) + ":" + std::to_string(event->_line) + "]";
        oss.str("");
        oss << event->_tid;
        str += "[tid:" + oss.str() + "]";
        str += "[fid:" + std::to_string(event->_fid) + "]:";
        str += event->_message + "\n";
        return str;
    }

}
using enum Log_util::log_level;