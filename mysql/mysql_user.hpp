#pragma once
#include"odb.hpp"
#include"user-odb.hxx"
namespace MindbniM
{
    class UserTable
    {
    public:
        using ptr=std::shared_ptr<UserTable>;
        UserTable(std::shared_ptr<odb::mysql::database> mysql):_mysql(mysql)
        {}
        bool insert(User& user)
        {
            try
            {
                odb::transaction t(_mysql->begin());
                _mysql->persist(user);
                t.commit();
            }
            catch(const std::exception& e)
            {
                LOG_ROOT_ERROR<<"新增用户失败: "<<e.what();
                return false;
            }
            return true;
        }
        std::shared_ptr<User> get(const std::string& nickname)
        {
            std::shared_ptr<User> ret;
            try
            {
                odb::transaction t(_mysql->begin());
                ret.reset(_mysql->query_one<User>(odb::query<User>::nickname==nickname));
                t.commit();
            }
            catch(const std::exception& e)
            {
                LOG_ROOT_ERROR<<"查询用户失败: "<<e.what();
                return nullptr;
            }
            return ret;
        }
        std::shared_ptr<User> select_by_id(const std::string& uid)
        {
            std::shared_ptr<User> ret;
            try
            {
                odb::transaction t(_mysql->begin());
                ret.reset(_mysql->query_one<User>(odb::query<User>::user_id==uid));
                t.commit();
            }
            catch(const std::exception& e)
            {
                LOG_ROOT_ERROR<<"查询用户失败: "<<e.what();
                return nullptr;
            }
            return ret;
        }
        bool update(User::ptr user)
        {
            try
            {
                odb::transaction t(_mysql->begin());
                _mysql->update(user.get());
                t.commit();
            }
            catch(const std::exception& e)
            {
                LOG_ROOT_ERROR<<"更新用户失败: "<<e.what();
                return false;
            }
            return true;
        }
    private:
        std::shared_ptr<odb::mysql::database> _mysql;
    };
}