#pragma once
#include"service.h"
const std::string TEMP_PATH="../temp/";
namespace MindbniM
{
    class GatewayService
    {
    public:
        using ptr=std::shared_ptr<GatewayService>;
        GatewayService(ServiceManager::ptr sm):_sm(sm)
        {
            _server.Get(GET_ALL_QUESTION,std::bind(&GatewayService::get_all_question,this,std::placeholders::_1,std::placeholders::_2));
            _server.Get(GET_ONE_QUESTION,std::bind(&GatewayService::get_one_question,this,std::placeholders::_1,std::placeholders::_2));
            _server.Post(JUDGE,[this](const httplib::Request& req,httplib::Response& rsp){this->judge(req,rsp);});
            _server.Post(LOGIN,[this](const httplib::Request& req,httplib::Response& rsp){this->login(req,rsp);});
            _server.Get(GET_ALL_FORUM,std::bind(&GatewayService::get_all_forum,this,std::placeholders::_1,std::placeholders::_2));
        }
        void listen(int port)
        {
            _server.set_base_dir("../wwwroot");
            _server.listen("0.0.0.0",port);
        }
        void get_all_forum(const httplib::Request &re, httplib::Response &rp)
        {
            LOG_ROOT_DEBUG<<"收到查看全部帖子请求";
            GetForumListRequest req;
            GetForumListResponse rsp;
            std::shared_ptr<brpc::Channel> channel=_sm->choose(FORUM_SERVER);
            if(channel==nullptr)
            {
                LOG_ROOT_ERROR<<"论坛子服务未找到";
                return;
            }
            ForumService_Stub stub(channel.get());
            brpc::Controller cntl;
            stub.GetForumList(&cntl,&req,&rsp,nullptr);
            if(cntl.Failed())
            {
                LOG_ROOT_ERROR<<"调用论坛子服务失败";
                return;
            }
            std::string path=TEMP_PATH+"all_forum.html";
            ctemplate::TemplateDictionary root("all");
            LOG_ROOT_DEBUG<<rsp.forum_list_size();
            for(int i=0;i<rsp.forum_list_size();i++)
            {
                const ForumHead &info=rsp.forum_list(i);
                ctemplate::TemplateDictionary* sub=root.AddSectionDictionary("forum_list");
                sub->SetValue("id",std::to_string(info.number()));
                sub->SetValue("title",info.title());
                sub->SetValue("author","MindbniM");
            }
            ctemplate::Template* temp=ctemplate::Template::GetTemplate(path,ctemplate::DO_NOT_STRIP);
            temp->Expand(&rp.body,&root);
            //std::cout<<rp.body<<std::endl;
            rp.set_header("Content-Type","text/html; charset=utf-8");
        }
        void get_all_question(const httplib::Request &re, httplib::Response &rp)
        {
            LOG_ROOT_DEBUG<<"收到查看全部题目请求";
            GetQuestionListRequest req;
            GetQuestionListResponse rsp;
            std::shared_ptr<brpc::Channel> channel=_sm->choose(QUESTION_SERVER);
            if(channel==nullptr)
            {
                LOG_ROOT_ERROR<<"题库子服务未找到";
                return;
            }
            QuestionService_Stub stub(channel.get());
            brpc::Controller cntl;
            stub.GetQuestionList(&cntl,&req,&rsp,nullptr);
            if(cntl.Failed())
            {
                LOG_ROOT_ERROR<<"调用题库子服务失败";
                return;
            }

            std::string path=TEMP_PATH+"all_question.html";
            ctemplate::TemplateDictionary root("all");
            for(int i=0;i<rsp.question_list_size();i++)
            {
                const QuestionHeadList &info=rsp.question_list(i);
                ctemplate::TemplateDictionary* sub=root.AddSectionDictionary("question_list");
                sub->SetValue("number",std::to_string(info.number()));
                sub->SetValue("title",info.title());
                sub->SetValue("star",info.star());
            }
            ctemplate::Template* temp=ctemplate::Template::GetTemplate(path,ctemplate::DO_NOT_STRIP);
            temp->Expand(&rp.body,&root);
            rp.set_header("Content-Type","text/html; charset=utf-8");
        }

        void get_one_question(const httplib::Request &re, httplib::Response &rp)
        {
            int id=std::stoi(re.matches[1]);
            std::string language=re.get_param_value("lang");
            if(language.empty()) language="cpp";
            GetQuestionRequest req;
            req.set_number(id);
            req.set_laguage(language);
            GetQuestionResponse rsp;
            std::shared_ptr<brpc::Channel> channel=_sm->choose(QUESTION_SERVER);
            if(channel==nullptr)
            {
                LOG_ROOT_ERROR<<"题库子服务未找到";
                return;
            }
            QuestionService_Stub stub(channel.get());
            brpc::Controller cntl;
            stub.GetQuestion(&cntl,&req,&rsp,nullptr);
            if(cntl.Failed())
            {
                LOG_ROOT_ERROR<<"调用题库子服务失败";
                return;
            }
             // 1. 形成路径
            std::string src_html = TEMP_PATH + "one_question.html";

            
            QuestionInfo* info=rsp.mutable_question_info();
            ctemplate::TemplateDictionary root("one_question");
            root.SetValue("number", std::to_string(info->number()));
            root.SetValue("title", info->title());
            root.SetValue("star", info->star());
            root.SetValue("desc", info->description());
            root.SetValue("pre_code", info->pre_code());

            //3. 获取被渲染的html
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);
           
            //4. 开始完成渲染功能
            tpl->Expand(&rp.body, &root);
            rp.set_header("Content-Type","text/html; charset=utf-8");
        }
        void get_question_test(const std::string& language,int id,string& include,string& test)
        {
            GetQuestionCodeRequest req;
            req.set_number(id);
            req.set_laguage(language);
            GetQuestionCodeResponse rsp;
            std::shared_ptr<brpc::Channel> channel=_sm->choose(QUESTION_SERVER);
            if(channel==nullptr)
            {
                LOG_ROOT_ERROR<<"题库子服务未找到";
                return;
            }
            QuestionService_Stub stub(channel.get());
            brpc::Controller cntl;
            stub.GetQuestionCode(&cntl,&req,&rsp,nullptr);
            if(cntl.Failed())
            {
                LOG_ROOT_ERROR<<"调用题库子服务失败";
                return;
            }
            include=rsp.include();
            test=rsp.test();
        }
        void get_question(int id,const std::string& language,QuestionInfo& info)
        {
            GetQuestionRequest req;
            req.set_number(id);
            req.set_laguage(language);
            GetQuestionResponse rsp;
            std::shared_ptr<brpc::Channel> channel=_sm->choose(QUESTION_SERVER);
            if(channel==nullptr)
            {
                LOG_ROOT_ERROR<<"题库子服务未找到";
                return;
            }
            QuestionService_Stub stub(channel.get());
            brpc::Controller cntl;
            stub.GetQuestion(&cntl,&req,&rsp,nullptr);
            if(cntl.Failed())
            {
                LOG_ROOT_ERROR<<"调用题库子服务失败";
                return;
            }
            info=*rsp.mutable_question_info();
        }
        void judge(const httplib::Request &re, httplib::Response &rp)
        {
            LOG_ROOT_DEBUG<<"收到判题请求";
            int id=std::stoi(re.matches[1]);
            Json::Value root;
            JSON::UnSerializa(re.body,root);
            string include,test;
            get_question_test(root["language"].asString(),id,include,test);
            QuestionInfo info;
            get_question(id,root["language"].asString(),info);


            CompileRequest req;
            req.set_code(include+"\n"+root["code"].asString()+"\n"+test);
            req.set_language(root["language"].asString());
            req.set_cpu_limit(info.cpu_limit());
            req.set_mem_limit(info.mem_limit());
            CompileResponse rsp;
            std::shared_ptr<brpc::Channel> channel=_sm->choose(COMMUNITY_SERVER);
            if(channel==nullptr)
            {
                LOG_ROOT_ERROR<<"判题子服务未找到";
                return;
            }
            CompileService_Stub stub(channel.get());
            brpc::Controller cntl;
            stub.Compile(&cntl,&req,&rsp,nullptr);
            if(cntl.Failed())
            {
                LOG_ROOT_ERROR<<"调用判题子服务失败";
                return;
            }
            //{  
            //    "reason": "",//状态码描述  
            //    "status": ,//结果状态码
            //    [可选]"stderr":"",//如果运行成功的标准错误  
            //    [可选]"stdout": ""//如果运行成功的标准输出  
            //}  
            Json::Value out;
            out["reason"]=rsp.reason();
            out["status"]=rsp.status();
            if(rsp.status()==0)
            {
                out["stderr"]=rsp.stderr();
                out["stdout"]=rsp.stdout();
            }
            std::string out_str;
            JSON::Serializa(out,out_str);
            rp.set_content(out_str,"application/json;charset=utf-8");
        }
        void login(const httplib::Request &re, httplib::Response &rp)
        {
            LOG_ROOT_DEBUG<<"收到登录请求";
            UserLoginReq req;
            UserLoginRsp rsp;
            std::shared_ptr<brpc::Channel> channel=_sm->choose(USER_SERVER);
            if(channel==nullptr)
            {
                LOG_ROOT_ERROR<<"用户子服务未找到";
                return;
            }
            UserService_Stub stub(channel.get());
            brpc::Controller cntl;
            stub.UserLogin(&cntl,&req,&rsp,nullptr);
            if(cntl.Failed())
            {
                LOG_ROOT_ERROR<<"调用用户子服务失败";
                return;
            }
        }

    private:
        ServiceManager::ptr _sm;
        httplib::Server _server;
    };
    class GatewayServiceBuild
    {
    public:
        void make_dis_object(const std::string &dis_host, const std::string& base_dir )
        {
            _service_manager = std::make_shared<ServiceManager>();
            _service_manager->add_concern(USER_SERVER);
            _service_manager->add_concern(QUESTION_SERVER);
            _service_manager->add_concern(COMMUNITY_SERVER);
            _service_manager->add_concern(FORUM_SERVER);
            auto put_cb = std::bind(&ServiceManager::onServiceOnline, _service_manager.get(), std::placeholders::_1, std::placeholders::_2);
            auto del_cb = std::bind(&ServiceManager::onServiceOffline, _service_manager.get(), std::placeholders::_1, std::placeholders::_2);
            _discover= std::make_shared<Discovery>(dis_host, put_cb, del_cb);
            _discover->discover(base_dir);
        }
        GatewayService::ptr newGatewayService()
        {
            return std::make_shared<GatewayService>(_service_manager);
        }
    private:
        ServiceManager::ptr _service_manager;
        Discovery::ptr _discover;
    };
}
