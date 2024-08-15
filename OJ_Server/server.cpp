#include<iostream>
//#include"httplib.h"
#include"model.hpp"
#include"log.hpp"
int main()
{

    Model data;
    data.test();
    //httplib::Server sr;
    ////获取题目列表
    //sr.Get("/all-question",[](const httplib::Request& re,httplib::Response& rp)
    //{
    //    rp.set_content("这是题目列表","text/plain; charset=utf-8");
    //});
    ////查看指定题目
    //sr.Get(R"(/question/(\d+))",[](const httplib::Request& re,httplib::Response& rp)
    //{
    //    std::string id=re.matches[1];
    //    rp.set_content("这是一道题目"+id,"text/plain; charset=utf-8");
    //});
    ////判题
    //sr.Get(R"(/judge/(\d+))",[](const httplib::Request& re,httplib::Response& rp)
    //{
    //    std::string id=re.matches[1];
    //    rp.set_content("这是一道题目的判题"+id,"text/plain; charset=utf-8");
    //});
    //sr.set_base_dir("../wwwroot");
    //sr.listen("0.0.0.0",8848);
    return 0;
}