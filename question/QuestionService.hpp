#pragma once
#pragma once
#include <jsoncpp/json/json.h>
#include"etcd.hpp"
#include"channel.hpp"
#include"question.pb.h"
#include<brpc/server.h>
#include"mysql_question.hpp"
#include"mysql_question_code.hpp"
namespace MindbniM
{
    class QuestionServiceImpl :public QuestionService 
    {
    public:
    QuestionServiceImpl(std::shared_ptr<odb::mysql::database> mysql)
    {
        _qt=std::make_shared<QuestionTable>(mysql);
        _qct=std::make_shared<QuestionCodeTable>(mysql);
    }
    void GetQuestionList(google::protobuf::RpcController* controller, const GetQuestionListRequest* req, GetQuestionListResponse* rsp, google::protobuf::Closure* done)
    {
        brpc::ClosureGuard guard(done);
        std::vector<Questions> res=_qt->getall();
        for(auto& q:res)
        {
            QuestionHeadList* qhl=rsp->add_question_list();
            qhl->set_number(q.id());
            qhl->set_title(q.title());
            qhl->set_star(q.star());
        }
    }
    void GetQuestion(google::protobuf::RpcController* controller, const GetQuestionRequest* req, GetQuestionResponse* rsp, google::protobuf::Closure* done)
    {
        LOG_ROOT_DEBUG<<"获取单个题目请求 id:"<<req->number()<<" language: "<<req->laguage();
        brpc::ClosureGuard guard(done);
        std::shared_ptr<Questions> res=_qt->get(req->number());
        std::shared_ptr<QuestionsCode> res_code=_qct->get(req->number(),req->laguage());
        if(res==nullptr)
        {
            LOG_ROOT_ERROR<<"没有找到该题目";
            return ;
        }
        if(res_code==nullptr)
        {
            LOG_ROOT_ERROR<<"没有找到该题目的代码";
            return ;
        }
        auto info=rsp->mutable_question_info();
        info->set_number(res->id());
        info->set_title(res->title());
        info->set_star(res->star());
        info->set_description(res->question_desc());
        info->set_pre_code(res_code->header());
        info->set_cpu_limit(res->time_limit());
        info->set_mem_limit(res->mem_limit());
    }
    void SearchQuestion(google::protobuf::RpcController* controller, const SearchQuestionRequest* req, SearchQuestionResponse* rsp, google::protobuf::Closure* done)
    {
        brpc::ClosureGuard guard(done);
        std::vector<Questions> res=_qt->search(req->keyword());
        for(auto& q:res)
        {
            QuestionHeadList* qhl=rsp->add_question_list();
            qhl->set_number(q.id());
            qhl->set_title(q.title());
            qhl->set_star(q.star());
        }
    }
    void GetQuestionCode(google::protobuf::RpcController* controller, const GetQuestionCodeRequest* req, GetQuestionCodeResponse* rsp, google::protobuf::Closure* done)
    {
        brpc::ClosureGuard guard(done);
        std::shared_ptr<QuestionsCode> res=_qct->get(req->number(),req->laguage());
        if(res==nullptr)
        {
            LOG_ROOT_ERROR<<"没有找到该题目的代码";
            return ;
        }
        rsp->set_include(res->include());
        rsp->set_test(res->tail());
    }
    private:
        QuestionTable::ptr _qt;
        QuestionCodeTable::ptr _qct;
    };
    class QuestionServer
    {
    public:
        using ptr=std::shared_ptr<QuestionServer>;
        QuestionServer(Registry::ptr reg,std::shared_ptr<brpc::Server> rpc):_reg_client(reg),_rpc_server(rpc)
        {}
        void start()
        {
            _rpc_server->RunUntilAskedToQuit();
        }
    private:
        Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };
    class QuestionServerBuild
    {
    public:
        void make_reg_object(const std::string& reg_host,const std::string& service_name,const std::string& service_host)
        {
            _reg_client=std::make_shared<Registry>(reg_host);
            _reg_client->registry(service_name,service_host);
        }
        void make_mysql_object( const std::string &user, const std::string &pswd, const std::string &host, const std::string &db, const std::string &cset,int port, int conn_pool_count)
        {
            _mysql=ODBFactory::create(user,pswd,host,db,cset,port,conn_pool_count);
        }
        void make_rpc_server(uint16_t port,int timeout,int thread_num)
        {
            if(_reg_client==nullptr)
            {
                LOG_ROOT_ERROR<<"未初始化服务注册模块";
                abort();
            }
            if(_mysql==nullptr)
            {
                LOG_ROOT_ERROR<<"未初始化mysql";
                abort();
            }
            _rpc_server=std::make_shared<brpc::Server>();
            QuestionServiceImpl* question_service=new QuestionServiceImpl(_mysql);
            int ret=_rpc_server->AddService(question_service,brpc::ServiceOwnership::SERVER_OWNS_SERVICE);
            if(ret<0)
            {
                LOG_ROOT_ERROR<<"添加rpc服务失败";
                return ;
            }
            brpc::ServerOptions op;
            op.idle_timeout_sec=timeout;
            op.num_threads=thread_num;
            ret=_rpc_server->Start(port,&op);
            if(ret<0)
            {
                LOG_ROOT_ERROR<<"rpc服务启动失败";
                return ;
            }
        }
        QuestionServer::ptr newQuestionServer()
        {
            if(_rpc_server==nullptr)
            {
                LOG_ROOT_ERROR<<"未初始化rpc服务器";
                abort();
            }
            return std::make_shared<QuestionServer>(_reg_client,_rpc_server);
        }
    private:
        Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
        std::shared_ptr<odb::mysql::database> _mysql;
    };
}
