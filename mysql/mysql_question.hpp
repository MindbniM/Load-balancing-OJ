#include"odb.hpp"
#include"question-odb.hxx"
namespace MindbniM
{
    class QuestionTable
    {
    public:
        using ptr=std::shared_ptr<QuestionTable>;
        QuestionTable(std::shared_ptr<odb::mysql::database> mysql)
            : _mysql(mysql)
        {}
        bool insert(Questions& q)
        {
            try
            {
                odb::transaction t(_mysql->begin());
                _mysql->persist(q);
                t.commit();
            }
            catch (const odb::exception& e)
            {
                LOG_ROOT_ERROR << "新增题目失败: " << e.what();
                return false;
            }
            return true;
        }
        std::vector<Questions> getall()
        {
            std::vector<Questions> res;
            try
            {
                odb::transaction t(_mysql->begin());
                odb::result<Questions> r(_mysql->query<Questions>());
                for (auto& q : r)
                {
                    res.push_back(q);
                }
                t.commit();
            }
            catch (const odb::exception& e)
            {
                LOG_ROOT_ERROR << "获取题目失败: " << e.what();
            }
            return res;
        }
        std::vector<Questions> search(const std::string& key)
        {
            std::vector<Questions> res;
            try
            {
                odb::transaction t(_mysql->begin());
                odb::result<Questions> r(_mysql->query<Questions>(odb::query<Questions>::title.like("%" + key + "%")));
                for (auto& q : r)
                {
                    res.push_back(q);
                }
                t.commit();
            }
            catch (const odb::exception& e)
            {
                LOG_ROOT_ERROR << "搜索题目失败: " << e.what();
            }
            return res;
        }
        std::shared_ptr<Questions> get(int id)
        {
            std::shared_ptr<Questions> res;
            try
            {
                odb::transaction t(_mysql->begin());
                res.reset( _mysql->query_one<Questions>(odb::query<Questions>::id == id));
                t.commit();
            }
            catch (const odb::exception& e)
            {
                LOG_ROOT_ERROR << "获取题目失败: " << e.what();
            }
            return res;
        }
    private:
        std::shared_ptr<odb::mysql::database> _mysql;
    };
}