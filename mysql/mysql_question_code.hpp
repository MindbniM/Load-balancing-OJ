#include"odb.hpp"
#include"question_code-odb.hxx"
namespace MindbniM
{
    class QuestionCodeTable
    {
    public:
        using ptr=std::shared_ptr<QuestionCodeTable>;
        QuestionCodeTable(std::shared_ptr<odb::mysql::database> mysql):_mysql(mysql)
        {}
        std::shared_ptr<QuestionsCode> get(int question_id,const std::string& language)
        {
            std::shared_ptr<QuestionsCode> res;
            try
            {
                odb::transaction t(_mysql->begin());
                res.reset(_mysql->query_one<QuestionsCode>(odb::query<QuestionsCode>::question_id==question_id&&odb::query<QuestionsCode>::language==language));
                t.commit();
            }
            catch(const std::exception& e)
            {
                LOG_ROOT_ERROR<<"查询题目代码失败: "<<e.what();
                return nullptr;
            }
            return res;
        }
    private:
        std::shared_ptr<odb::mysql::database> _mysql;
    };
}