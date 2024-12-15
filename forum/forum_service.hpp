#pragma once
#pragma once
#include <jsoncpp/json/json.h>
#include"etcd.hpp"
#include"channel.hpp"
#include"forum.pb.h"
#include<brpc/server.h>
#include"mysql_forum.hpp"
namespace MindbniM
{
    class ForumServiceImpl :public ForumService 
    {
    public:
    ForumServiceImpl(std::shared_ptr<odb::mysql::database> mysql)
    {
        _qt=std::make_shared<ForumTable>(mysql);
    }
    //rpc GetForumList(GetForumListRequest) returns (GetForumListResponse);
    //rpc GetForum(GetForumRequest) returns (GetForumResponse);
    void GetForumList(google::protobuf::RpcController* controller, const GetForumListRequest* req, GetForumListResponse* rsp, google::protobuf::Closure* done)
    {
        LOG_ROOT_DEBUG<<"获取文章列表请求";
        brpc::ClosureGuard guard(done);
        std::vector<Forum> res=_qt->getall();
        LOG_ROOT_DEBUG<<"获得"<<res.size()<<"个文章"; 
        for(auto& q:res)
        {
            ForumHead* qhl=rsp->add_forum_list();
            qhl->set_number(q.id());
            qhl->set_title(q.title());
            qhl->set_author("MindbniM");
        }
    }
    void GetForum(google::protobuf::RpcController* controller, const GetForumRequest* req, GetForumResponse* rsp, google::protobuf::Closure* done)
    {
        LOG_ROOT_DEBUG<<"获取单个文章请求 id:"<<req->number();
        brpc::ClosureGuard guard(done);
        std::shared_ptr<Forum> res=_qt->get(req->number());
        if(res==nullptr)
        {
            LOG_ROOT_ERROR<<"没有找到该文章";
            return ;
        }
        auto info=rsp->mutable_forum_info();
        info->mutable_forum_head()->set_number(res->id());
        info->mutable_forum_head()->set_title(res->title());
        info->set_content(res->content());
    }
    private:
        ForumTable::ptr _qt;
    };
    class ForumServer
    {
    public:
        using ptr=std::shared_ptr<ForumServer>;
        ForumServer(Registry::ptr reg,std::shared_ptr<brpc::Server> rpc):_reg_client(reg),_rpc_server(rpc)
        {}
        void start()
        {
            _rpc_server->RunUntilAskedToQuit();
        }
    private:
        Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };
    class ForumServerBuild
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
            ForumServiceImpl* question_service=new ForumServiceImpl(_mysql);
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
        ForumServer::ptr newForumServer()
        {
            if(_rpc_server==nullptr)
            {
                LOG_ROOT_ERROR<<"未初始化rpc服务器";
                abort();
            }
            return std::make_shared<ForumServer>(_reg_client,_rpc_server);
        }
    private:
        Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
        std::shared_ptr<odb::mysql::database> _mysql;
    };
}
