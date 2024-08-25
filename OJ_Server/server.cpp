#include <iostream>
#include <ctemplate/template.h>
#include "httplib.h"
// #include"model.hpp"
#include "control.hpp"
#include "log.hpp"
int main()
{
    ADD_APPENDER_STDOUT(Log_util::log_level::DEBUG);
    Control ctrl;
    // std::string html;
    // ctrl.get_all_question(html);
    // std::cout<<html<<std::endl;

    httplib::Server sr;
    // 获取题目列表
    sr.Get("/all-question", [&ctrl](const httplib::Request &re, httplib::Response &rp)
           {
        LOG(Log_util::log_level::INFO,"收到查看全部题目请求");
        std::string question_html;
        ctrl.get_all_question(question_html);
        rp.set_content(question_html,"text/html; charset=utf-8"); });
    // 查看指定题目
    sr.Get(R"(/question/(\d+)/)", [&ctrl](const httplib::Request &re, httplib::Response &rp)
           {
        int id=std::stoi(re.matches[1]);
        std::string language=re.get_param_value("lang");
        if(language.empty()) language="c_cpp";
        LOG(Log_util::log_level::INFO,"收到查看指定题目请求%d:语言:%s",id,language.c_str());
        std::string one_html;
        ctrl.get_number_question(id,language,one_html);
        rp.set_content(one_html,"text/html; charset=utf-8"); });
    // 判题
    sr.Post(R"(/judge/(\d+))", [&ctrl](const httplib::Request &re, httplib::Response &rp)
            {
        int id=std::stoi(re.matches[1]);
        LOG(Log_util::log_level::INFO,"收到编译指定题目请求%d",id);
        std::string injson=re.body;
        std::string outjson;
        ctrl.judge(id,injson,outjson);
        rp.set_content(outjson,"application/json;charset=utf-8"); });

    // 登录
    sr.Post("/login", [&ctrl](const httplib::Request &re, httplib::Response &rp)
            {
    std::string injson = re.body;
    std::string outjson;
    uint32_t id = ctrl.login(injson, outjson);

    if (id!=0||id!=1) {
        // 设置会话 ID 的 Cookie
        std::string cookie = "session_id=" + std::to_string(id) + "; Path=/; HttpOnly";
        rp.set_header("Set-Cookie", cookie);

        // 可选：响应中直接返回会话 ID
        rp.set_content(std::to_string(id), "text/plain");
    } else {
        // 如果登录失败，返回错误信息
        rp.set_content(outjson, "application/json;charset=utf-8");
    } });

    sr.set_base_dir("../wwwroot");
    sr.listen("0.0.0.0", 80);
    return 0;
}