#pragma once
#include <string>
#include <cstddef> 
#include<memory>
#include <odb/nullable.hxx>
#include <odb/core.hxx>
namespace MindbniM
{
    #pragma db object table("user")
    class User
    {
        friend class odb::access;
    public:
        using ptr=std::shared_ptr<User>;
        User()=default;
        User(const std::string& uid,const std::string& nickname,const std::string& password):_user_id(uid),_nickname(nickname),_password(password)
        {}
        std::string user_id() const { return _user_id; }
        void user_id(const std::string& uid) { _user_id = uid; }
        std::string nickname() const { return _nickname; }
        void nickname(const std::string& nickname) { _nickname = nickname; }
        std::string password() const { return _password; }
        void password(const std::string& password) { _password = password; }
    public:
        #pragma db id auto
        unsigned long _id;                          //主键id
        #pragma db type("varchar(64)") index unique
        std::string _user_id;                       //用户id
        #pragma db type("varchar(64)") index unique
        std::string _nickname;       //用户名称 
        #pragma db type("varchar(64)")
        std::string _password;       //密码
    };
}