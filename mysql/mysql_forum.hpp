#pragma once
#include"odb.hpp"
#include"forum-odb.hxx"
namespace MindbniM
{
    class ForumTable
    {  
    public:
        using ptr=std::shared_ptr<ForumTable>;
        ForumTable(std::shared_ptr<odb::mysql::database> mysql):db(mysql)
        {}
        void insert(Forum& val)
        {
            try
            {
                odb::transaction t(db->begin());
                db->persist(val);
                t.commit();
            }
            catch(const std::exception& e)
            {
                LOG_ROOT_ERROR<<"插入论坛失败: "<<e.what();
            }
        }
        std::shared_ptr<Forum> get(int id)
        {
            std::shared_ptr<Forum> res;
            try
            {
                odb::transaction t(db->begin());
                res.reset(db->query_one<Forum>(odb::query<Forum>::id==id));
                t.commit();
            }
            catch(const std::exception& e)
            {
                LOG_ROOT_ERROR<<"查询论坛失败: "<<e.what();
                return nullptr;
            }
            return res;
        }
        std::vector<Forum> getall()
        {
            std::vector<Forum> res;
            try
            {
                odb::transaction t(db->begin());
                odb::result<Forum> r=db->query<Forum>();
                for(auto& q:r)
                {
                    res.push_back(q);
                }
                t.commit();
            }
            catch(const std::exception& e)
            {
                LOG_ROOT_ERROR<<"查询论坛失败: "<<e.what();
            }
            return res;
        }
    private:
        std::shared_ptr<odb::mysql::database> db;
    };
} 
