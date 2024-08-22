#pragma once
#include <vector>
#include <fstream>
#include <mutex>
#include "model.hpp"
#include "view.hpp"
#include "httplib.h"
#include <jsoncpp/json/json.h>
struct Machine
{
    std::string ip;
    int port;
    int load;
    bool onlie;
    std::mutex *m;
    int Load()
    {
        return load;
    }
    void IncLoad()
    {
        m->lock();
        load++;
        m->unlock();
    }
    void DecLoad()
    {
        m->lock();
        load--;
        m->unlock();
    }
    void Offline()
    {
        m->lock();
        onlie = false;
        m->unlock();
    }
    void Online()
    {
        m->lock();
        onlie = false;
        m->unlock();
    }
};
const std::string service_machine_conf = "../service_machine.conf";
class Load
{
public:
    Load()
    {
        Load_conf();
        LOG(Log_util::log_level::INFO, "已加载配置文件");
        ShowMachines();
    }
    bool Load_conf()
    {
        using enum Log_util::log_level;
        std::ifstream in(service_machine_conf);
        if (!in.is_open())
        {
            LOG(Log_util::log_level::FATAL, "配置文件打开失败");
            return false;
        }
        std::string temp;
        int line_number = 0;
        while (getline(in, temp))
        {
            line_number++;
            std::stringstream i(temp);
            Machine mach;

            if (!(i >> mach.ip))
            {
                std::string log = "配置文件不标准, 无法读取IP, 行: " + std::to_string(line_number);
                LOG(WARNING, log.c_str());
                continue;
            }

            if (!(i >> mach.port)) // 假设是 port
            {
                std::string log = "配置文件不标准，无法读取端口，行: " + std::to_string(line_number);
                LOG(WARNING, log.c_str());
                continue;
            }

            mach.m = new std::mutex;
            mach.onlie=true;
            _v.push_back(mach);
        }
        return true;
    }
    bool SmartChiose(std::string &ip, int &port)
    {
        using enum Log_util::log_level;
        _m.lock();
        int load = INT_MAX;
        for (auto &m : _v)
        {
            if (m.onlie)
            {
                if (m.Load() < load)
                {
                    load = m.Load();
                    ip = m.ip;
                    port = m.port;
                }
            }
        }
        _m.unlock();
        if (load == INT_MAX)
        {
            LOG(FATAL, "后端服务全部寄了");
            return false;
        }
        return true;
    }
    void Offline_Machine(const std::string &ip, int port)
    {
        _m.lock();
        for (auto &i : _v)
        {
            if (i.ip == ip && i.port == port)
            {
                i.Offline();
                break;
            }
        }
        _m.unlock();
    }
    void Online_Machine(const std::string &ip, int port)
    {
        _m.lock();
        for (auto &i : _v)
        {
            if (i.ip == ip && i.port == port)
            {
                i.Online();
                break;
            }
        }
        _m.unlock();
    }
    void IncLoad(const std::string &ip, int port)
    {
        for (auto &i : _v)
        {
            if (i.ip == ip && i.port == port)
            {
                i.IncLoad();
                break;
            }
        }
    }
    void DecLoad(const std::string &ip, int port)
    {
        for (auto &i : _v)
        {
            if (i.ip == ip && i.port == port)
            {
                i.DecLoad();
                break;
            }
        }
    }
    void ShowMachines()
    {
        _m.lock();
        std::cout << "当前在线主机列表: ";
        for (auto &id : _v)
        {
            if (id.onlie)
                std::cout << id.ip << ":" << id.port << std::endl;
            ;
        }
        std::cout << std::endl;
        std::cout << "当前离线主机列表: ";
        for (auto &id : _v)
        {
            if (!id.onlie)
                std::cout << id.ip << ":" << id.port << std::endl;
            ;
        }
        std::cout << std::endl;
        _m.unlock();
    }

private:
    std::vector<Machine> _v;
    std::mutex _m;
};
class Control
{
public:
    Control()
    {
    }
    bool get_all_question(std::string &html)
    {
        if (_v.AllExpandHtml(_m, html))
        {
            return true;
        }
        return false;
    }
    bool get_number_question(int number,const std::string& language, std::string &html)
    {
        question q;
        _m.get_question(number, q);
        if (_v.OneExpandHtml(q, html,language))
        {
            return true;
        }
        return false;
    }
    void judge(int number, const std::string injson, std::string &outjson)
    {
        using enum Log_util::log_level;
        question q;
        _m.get_question(number, q);

        Json::Reader reader;
        Json::Value in_value;
        reader.parse(injson, in_value);
        std::string code = in_value["code"].asString();
        std::string language = in_value["language"].asString();
        Json::Value compile_value;
        compile_value["input"] = in_value["input"].asString();

        if (language == "java")
            compile_value["code"] = q.java.include + code + q.java.tail;
        else if (language == "python")
            compile_value["code"] = q.py.include + code + q.py.tail;
        else
            compile_value["code"] = q.cpp.include + code + q.cpp.tail;

        compile_value["cpu_limit"] = q.cpu_limit;
        compile_value["mem_limit"] = q.mem_limit;
        compile_value["language"] = language;
        Json::FastWriter writer;
        std::string compile_string = writer.write(compile_value);
        while (true)
        {
            std::string m_ip;
            int m_port;
            if (!_load.SmartChiose(m_ip, m_port))
            {
                break;
            }
            LOG(INFO, "选择主机%s : %d", m_ip.c_str(), m_port);

            httplib::Client cli(m_ip, m_port);
            _load.IncLoad(m_ip, m_port);
            if (auto res = cli.Post("/Compile_Run", compile_string, "application/json;charset=utf-8"))
            {
                if (res->status == 200)
                {
                    outjson = res->body;
                    _load.DecLoad(m_ip, m_port);
                    LOG(INFO, "请求编译和运行服务成功...");
                    break;
                }
                _load.DecLoad(m_ip, m_port);
            }
            else
            {
                LOG(ERROR, "当前请求的主机%s : %d可能已经离线", m_ip.c_str(), m_port);
                _load.DecLoad(m_ip, m_port);
                _load.Offline_Machine(m_ip, m_port);
            }
        }
    }

private:
    Model _m;
    View _v;
    Load _load;
};