#pragma once
#include "mysql.hpp"
#include <map>
struct questionhead
{
    int number;        // 题目编号，唯一
    std::string title; // 题目的标题
    std::string star;  // 难度: 简单 中等 困难
};
struct question_code
{
    std::string include; // 题目预设的头文件
    std::string header;  // 题目预设给用户在线编辑器的代码
    std::string tail;    // 题目的测试用例，需要和header拼接，形成完整代码
};
struct question
{
    questionhead head;
    std::string desc; // 题目的描述
    question_code cpp;
    question_code java;
    question_code py;
    int cpu_limit; // 题目的时间要求(S)
    int mem_limit; // 题目的空间要求(KB)
};
namespace SQL
{
    const std::string EXIST = "SELECT 1 FROM questions WHERE id =";
    const std::string TITLE_SQL = "select title from questions where id=";
    const std::string STAR_SQL = "select star from questions where id=";
    const std::string DESC_SQL = "select question_desc from questions where id=";

    const std::string CPP_INCLUDE_SQL = "select include from question_code where language='cpp' and question_id=";
    const std::string CPP_HEADER_SQL = "select header from question_code where language='cpp' and question_id=";
    const std::string CPP_TAIL_SQL = "select tail from questions_code where language='cpp' and question_id=";
    const std::string JAVA_INCLUDE_SQL = "select include from question_code where language='java' and question_id=";
    const std::string JAVA_HEADER_SQL = "select header from question_code where language='java' and question_id=";
    const std::string JAVA_TAIL_SQL = "select tail from questions_code where language='java' and question_id=";
    const std::string PY_INCLUDE_SQL = "select include from question_code where language='python' and question_id=";
    const std::string PY_HEADER_SQL = "select header from question_code where language='python' and question_id=";
    const std::string PY_TAIL_SQL = "select tail from questions_code where language='python' and question_id=";

    const std::string CPU_LIMIT_SQL = "select time_limit from questions where id=";
    const std::string MEM_LIMIT_SQL = "select mem_limit from questions where id=";
}
class Model
{
public:
    using iterator = std::map<int, question>::const_iterator;
    Model()
    {
        int i = 1;
        while (true)
        {
            question q;
            std::string sql = SQL::EXIST + std::to_string(i);
            if (_root.exec(sql.c_str()) == "")
                break;
            q.head.number = i;
            std::string id = std::to_string(i);
            sql = SQL::TITLE_SQL + id;
            q.head.title = _root.exec(sql.c_str());
            sql = SQL::STAR_SQL + id;
            q.head.star = _root.exec(sql.c_str());
            sql = SQL::DESC_SQL + id;
            q.desc = _root.exec(sql.c_str());

            sql = SQL::CPP_HEADER_SQL + id;
            q.cpp.header = _root.exec(sql.c_str());
            sql = SQL::CPP_TAIL_SQL + id;
            q.cpp.tail = _root.exec(sql.c_str());
            sql = SQL::CPP_INCLUDE_SQL + id;
            q.cpp.include = _root.exec(sql.c_str());

            sql = SQL::JAVA_HEADER_SQL + id;
            q.java.header = _root.exec(sql.c_str());
            sql = SQL::JAVA_TAIL_SQL + id;
            q.java.tail = _root.exec(sql.c_str());
            sql = SQL::JAVA_INCLUDE_SQL + id;
            q.java.include = _root.exec(sql.c_str());

            sql = SQL::PY_HEADER_SQL + id;
            q.py.header = _root.exec(sql.c_str());
            sql = SQL::PY_TAIL_SQL + id;
            q.py.tail = _root.exec(sql.c_str());
            sql = SQL::PY_INCLUDE_SQL + id;
            q.py.include = _root.exec(sql.c_str());

            sql = SQL::CPU_LIMIT_SQL + id;
            q.cpu_limit = std::stoi(_root.exec(sql.c_str()));
            sql = SQL::MEM_LIMIT_SQL + id;
            q.mem_limit = std::stoi(_root.exec(sql.c_str()));

            _v[i] = q;
            i++;
        }
        LOG(Log_util::log_level::INFO, "获取%d个题目信息", i - 1);
    }
    bool get_question(int id, question &q)
    {
        if (!_v.count(id))
            return false;
        q = _v[id];
        return true;
    }
    void test()
    {
        int i = 1;
        while (_v.count(i))
        {
            std::cout << "///" << _v[i].head.number << " " << _v[i].head.title << " " << _v[i].head.star << std::endl;
            std::cout << "///" << _v[i].desc << std::endl;

            std::cout << "///" << _v[i].cpp.include << std::endl;
            std::cout << "///" << _v[i].cpp.header << std::endl;
            std::cout << "///" << _v[i].cpp.tail << std::endl;

            std::cout << "///" << _v[i].java.include << std::endl;
            std::cout << "///" << _v[i].java.header << std::endl;
            std::cout << "///" << _v[i].java.tail << std::endl;

            std::cout << "///" << _v[i].py.include << std::endl;
            std::cout << "///" << _v[i].py.header << std::endl;
            std::cout << "///" << _v[i].py.tail << std::endl;

            std::cout << "///" << _v[i].cpu_limit << " " << _v[i].mem_limit << std::endl;
            i++;
        }
    }
    iterator begin() const
    {
        return _v.begin();
    }
    iterator end() const
    {
        return _v.end();
    }

private:
    std::map<int, question> _v;
    mysql_util::mysql _root;
};