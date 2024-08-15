#pragma once
#include "mysql.hpp"
#include<map>
struct questionhead
{
    int number; // 题目编号，唯一
    std::string title;  // 题目的标题
    std::string star;   // 难度: 简单 中等 困难
};
struct question
{
    questionhead head;
    std::string desc;   // 题目的描述
    std::string header; // 题目预设给用户在线编辑器的代码
    std::string tail;   // 题目的测试用例，需要和header拼接，形成完整代码
    int cpu_limit;      // 题目的时间要求(S)
    int mem_limit;      // 题目的空间要求(KB)
};
namespace SQL
{
    const std::string EXIST="SELECT 1 FROM questions WHERE id =";
    const std::string TITLE_SQL="select title from questions where id=";
    const std::string STAR_SQL="select star from questions where id=";
    const std::string DESC_SQL="select question_desc from questions where id=";
    const std::string HEADER_SQL="select header from questions where id=";
    const std::string TAIL_SQL="select tail from questions where id=";
    const std::string CPU_LIMIT_SQL="select time_limit from questions where id=";
    const std::string MEM_LIMIT_SQL="select mem_limit from questions where id=";
}
class Model
{
public:
    Model()
    {
        int i=1;
        while(true)
        {
            question q;
            std::string sql=SQL::EXIST+std::to_string(i);
            if(_root.exec(sql.c_str())=="") break;
            q.head.number=i;
            sql=SQL::TITLE_SQL+std::to_string(i);
            q.head.title=_root.exec(sql.c_str());
            sql=SQL::STAR_SQL+std::to_string(i);
            q.head.star=_root.exec(sql.c_str());
            sql=SQL::DESC_SQL+std::to_string(i);
            q.desc=_root.exec(sql.c_str());
            sql=SQL::HEADER_SQL+std::to_string(i);
            q.header=_root.exec(sql.c_str());
            sql=SQL::TAIL_SQL+std::to_string(i);
            q.tail=_root.exec(sql.c_str());
            sql=SQL::CPU_LIMIT_SQL+std::to_string(i);
            q.cpu_limit=std::stoi(_root.exec(sql.c_str()));
            sql=SQL::MEM_LIMIT_SQL+std::to_string(i);
            q.mem_limit=std::stoi(_root.exec(sql.c_str()));

            _v[i]=q;
            i++;
        }
        std::cout<<i<<std::endl;
    }
    bool get_question(int id,question& q)
    {
        if(!_v.count(id)) return false;
        q=_v[id];
        return true;
    }
    void test()
    {
        int i= 1;
        while(_v.count(i))
        {
            std::cout<<"///"<<_v[i].head.number<<" "<<_v[i].head.title<<" "<<_v[i].head.star<<std::endl;
            std::cout<<"///"<<_v[i].desc<<std::endl;
            std::cout<<"///"<<_v[i].header<<std::endl;
            std::cout<<"///"<<_v[i].tail<<std::endl;
            std::cout<<"///"<<_v[i].cpu_limit<<" "<<_v[i].mem_limit<<std::endl;
            i++;
        }
    }
private:
    std::map<int,question> _v;
    mysql_util::mysql _root;
};