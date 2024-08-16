#include<iostream>
#include<ctemplate/template.h>
#include"httplib.h"
//#include"model.hpp"
#include"control.hpp"
#include"log.hpp"
int main()
{
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
    sr.Get(R"(/question/(\d+))",[](const httplib::Request& re,httplib::Response& rp)
    {
        std::string id=re.matches[1];
        rp.set_content("这是一道题目"+id,"text/plain; charset=utf-8");
    });
    //判题
    sr.Get(R"(/judge/(\d+))",[](const httplib::Request& re,httplib::Response& rp)
    {
        std::string id=re.matches[1];
        rp.set_content("这是一道题目的判题"+id,"text/plain; charset=utf-8");
    });
    sr.set_base_dir("../wwwroot");
    sr.listen("0.0.0.0",8848);
    return 0;
}