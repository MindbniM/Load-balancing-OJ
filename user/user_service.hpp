#pragma once
#pragma once
#include <jsoncpp/json/json.h>
#include"etcd.hpp"
#include"channel.hpp"
#include"user.pb.h"
#include<brpc/server.h>
#include"mysql_user.hpp"
#include"redis_data.hpp"
#include"uuid.hpp"
namespace MindbniM
{
    class UserServiceImpl :public UserService 
    {
        bool nickname_check(const std::string &nickname)
        {
            if (nickname.size() < 1 || nickname.size() > 100)
            {
                return false;
            }
            return true;
        }
        bool password_check(const std::string &password)
        {
            if (password.size() < 6 || password.size() > 15)
                return false;
            return true;
        }
    public:
    UserServiceImpl(std::shared_ptr<odb::mysql::database> mysql,std::shared_ptr<sw::redis::Redis> redis)
    {
        _usertable=std::make_shared<UserTable>(mysql);
        _session=std::make_shared<Session>(redis);
        _status=std::make_shared<Status>(redis);
    }
    //rpc UserRegister(UserRegisterReq) returns (UserRegisterRsp);
    //rpc UserLogin(UserLoginReq) returns (UserLoginRsp);
    //rpc GetUserInfo(GetUserInfoReq) returns (GetUserInfoRsp);
    //rpc SetUserNickname(SetUserNicknameReq) returns(SetUserNicknameRsp);
    void UserRegister(google::protobuf::RpcController* controller,const UserRegisterReq* req,UserRegisterRsp* rsp,google::protobuf::Closure* done) override
    {
        brpc::ClosureGuard done_guard(done);
        LOG_ROOT_DEBUG << "用户注册请求";
        auto errctl = [req, rsp](const std::string &err) -> void
        {
            rsp->set_request_id(req->request_id());
            rsp->set_success(false);
            rsp->set_errmsg(err);
        };
        // 1. 从请求中取出昵称和密码
        std::string nickname = req->nickname();
        std::string password = req->password();
        // 2. 检查昵称是否合法（只能包含字母，数字，连字符-，下划线_，长度限制 3~15 之间）
        bool ret = nickname_check(nickname);
        if (!ret)
            return errctl("昵称太长或太短或包含非法字符");
        // 3. 检查密码是否合法（长度限制 6~15 之间）
        ret = password_check(password);
        if (!ret)
            return errctl("密码太长或太短");
        // 4. 根据昵称在数据库进行判断是否昵称已存在
        User::ptr p = _usertable->get(nickname);
        if (p)
            return errctl("昵称已存在");
        // 5. 向数据库新增数据
        std::string uid = UUID::Get();
        User newuser(uid, nickname, password);
        ret = _usertable->insert(newuser);
        if (!ret)
        {
            LOG_ROOT_ERROR << "mysql插入新用户 " << nickname << " 失败";
            return errctl("用户注册失败,未知原因");
        }
        rsp->set_request_id(req->request_id());
        rsp->set_success(true);
    }
    void UserLogin(google::protobuf::RpcController* controller,const UserLoginReq* req,UserLoginRsp* rsp,google::protobuf::Closure* done) override
    {
        brpc::ClosureGuard done_guard(done);
        auto errctl = [req, rsp](const std::string &err) -> void
        {
            rsp->set_request_id(req->request_id());
            rsp->set_success(false);
            rsp->set_errmsg(err);
        };
        // 1. 从请求中取出昵称和密码
        std::string nickname = req->nickname();
        std::string password = req->password();
        // 2. 通过昵称获取用户信息，进行密码是否一致的判断
        User::ptr user = _usertable->get(nickname);
        if (!user || user->password() != password)
            return errctl("用户名或密码错误");
        // 3. 根据 redis 中的登录标记信息是否存在判断用户是否已经登录。如果存在就删除会话id,俗称挤号
        if (_status->exist(user->_user_id))
        {
            // session中 ssid->uid
            // status中 uid->status可互相拿到
            std::string ssid = _status->get(user->_user_id);
            _session->remove(ssid);
            _status->remove(user->_user_id);
        }
        // 4. 构造会话 ID，生成会话键值对，向 redis 中添加会话信息以及登录标记信息
        std::string ssid = UUID::Get();
        _session->append(ssid, user->_user_id);
        _status->append(user->_user_id, ssid);
        // 5. 组织响应，返回生成的会话
        rsp->set_request_id(req->request_id());
        rsp->set_success(true);
        rsp->set_login_session_id(ssid);
    }
    void GetUserInfo(google::protobuf::RpcController* controller,const GetUserInfoReq* req,GetUserInfoRsp* rsp,google::protobuf::Closure* done) override
    {
        brpc::ClosureGuard done_guard(done);
        brpc::ClosureGuard guard(done);
        auto errctl = [req, rsp](const std::string &err) -> void
        {
            rsp->set_request_id(req->request_id());
            rsp->set_success(false);
            rsp->set_errmsg(err);
        };
        // 1. 从请求中取出用户 ID
        std::string uid = req->user_id();
        // 2. 通过用户 ID，从数据库中查询用户信息
        User::ptr user = _usertable->select_by_id(uid);
        if (!user)
        {
            LOG_ROOT_ERROR << "已登录用户 " << uid << " 不存在";
            return errctl("用户不存在");
        }
        UserInfo *ret = rsp->mutable_user_info();
        ret->set_user_id(user->user_id());
        ret->set_nickname(user->nickname());
        // 3. 组织响应，返回用户信息
        rsp->set_request_id(req->request_id());
        rsp->set_success(true);
    }
    void SetUserNickname(google::protobuf::RpcController* controller,const SetUserNicknameReq* req,SetUserNicknameRsp* rsp,google::protobuf::Closure* done) override
    {
        brpc::ClosureGuard done_guard(done);
        auto errctl = [req, rsp](const std::string &err) -> void
        {
            rsp->set_request_id(req->request_id());
            rsp->set_success(false);
            rsp->set_errmsg(err);
        };
        //1. 从请求中取出用户 ID 与昵称
        std::string user_id=req->user_id();
        std::string nickname=req->nickname();
        //2. 检查昵称是否合法
        bool ret=nickname_check(nickname);
        if(!ret) return errctl("昵称不合法");
        //3. 从数据库通过用户 ID 进行用户信息查询，判断用户是否存在
        User::ptr p=_usertable->select_by_id(user_id);
        if(!p) return errctl("用户不存在");
        //4. 更新用户昵称
        p->_nickname=nickname;
        ret=_usertable->update(p);
        if(!ret)
        {
            LOG_ROOT_ERROR<<"更新用户昵称失败";
            return errctl("更新用户昵称失败");
        }
        //6. 组织响应，返回更新成功与否
        rsp->set_request_id(req->request_id());
        rsp->set_success(true);
    }
    private:
        UserTable::ptr _usertable;
        Session::ptr _session;
        Status::ptr _status;
    };
    class UserServer
    {
    public:
        using ptr=std::shared_ptr<UserServer>;
        UserServer(Registry::ptr reg,std::shared_ptr<brpc::Server> rpc):_reg_client(reg),_rpc_server(rpc)
        {}
        void start()
        {
            _rpc_server->RunUntilAskedToQuit();
        }
    private:
        Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
    };
    class UserServerBuild
    {
    public:
        void make_reg_object(const std::string& reg_host,const std::string& service_name,const std::string& service_host)
        {
            _reg_client=std::make_shared<Registry>(reg_host);
            _reg_client->registry(service_name,service_host);
        }
        void make_mysql_object( const std::string &User, const std::string &pswd, const std::string &host, const std::string &db, const std::string &cset,int port, int conn_pool_count)
        {
            _mysql=ODBFactory::create(User,pswd,host,db,cset,port,conn_pool_count);
        }
        void make_redis_object(const std::string &host, int port, int db)
        {                                                                                                                                                                             
            _redis = RedisFactory::create(host, port, db);
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
            if(_redis==nullptr)
            {
                LOG_ROOT_ERROR<<"未初始化redis";
                abort();
            }
            _rpc_server=std::make_shared<brpc::Server>();
            UserServiceImpl* User_service=new UserServiceImpl(_mysql,_redis);
            int ret=_rpc_server->AddService(User_service,brpc::ServiceOwnership::SERVER_OWNS_SERVICE);
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
        UserServer::ptr newUserServer()
        {
            if(_rpc_server==nullptr)
            {
                LOG_ROOT_ERROR<<"未初始化rpc服务器";
                abort();
            }
            return std::make_shared<UserServer>(_reg_client,_rpc_server);
        }
    private:
        Registry::ptr _reg_client;
        std::shared_ptr<brpc::Server> _rpc_server;
        std::shared_ptr<odb::mysql::database> _mysql;
        std::shared_ptr<sw::redis::Redis> _redis;
    };
}