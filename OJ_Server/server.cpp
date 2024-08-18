#include<iostream>
#include<ctemplate/template.h>
#include"httplib.h"
//#include"model.hpp"
#include"control.hpp"
#include"log.hpp"
int main()
{
    ADD_APPENDER_STDOUT(Log_util::log_level::DEBUG);
    Control ctrl;
    //std::string html;
    //ctrl.get_all_question(html);
    //std::cout<<html<<std::endl;

    httplib::Server sr;
    //获取题目列表
    sr.Get("/all-question",[&ctrl](const httplib::Request& re,httplib::Response& rp)
    {
        std::string question_html;
        ctrl.get_all_question(question_html);
        rp.set_content(question_html,"text/html; charset=utf-8");
    });
    //查看指定题目
    sr.Get(R"(/question/(\d+))",[&ctrl](const httplib::Request& re,httplib::Response& rp)
    {
        int id=std::stoi(re.matches[1]);
        std::string one_html;
        ctrl.get_number_question(id,one_html);
        rp.set_content(one_html,"text/html; charset=utf-8");
    });
    //判题
    sr.Post(R"(/judge/(\d+))",[&ctrl](const httplib::Request& re,httplib::Response& rp)
    {
        int id=std::stoi(re.matches[1]);
        std::string injson=re.body;
        std::string outjson;
        ctrl.judge(id,injson,outjson);
        rp.set_content(outjson,"application/json;charset=utf-8");
    });
    sr.set_base_dir("../wwwroot");
    sr.listen("0.0.0.0",8848);
    return 0;
}