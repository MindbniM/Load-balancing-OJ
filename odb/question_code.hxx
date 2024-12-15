#pragma once

#include <string>
#include <odb/core.hxx>
#include <cstddef>
#include <odb/nullable.hxx>

#pragma db object table("question_code")
class QuestionsCode
{
public:
    QuestionsCode() = default;
    int question_id() const { return _question_id; }
    void question_id(int question_id){_question_id=question_id;}
    std::string include() const { return _include==nullptr?"":*_include; }
    void include(const std::string& include) { _include = include; }
    const std::string& header() const { return _header; }
    void header(const std::string& header) { _header = header; }
    const std::string& tail() const { return _tail; }
    void tail(const std::string& tail) { _tail = tail; }
    const std::string& language() const { return _language; }
    void language(const std::string& language) { _language = language; }


private:
    friend class odb::access;
    // 数据库字段
    #pragma db id auto
    int id_; // 主键，自增

    #pragma db type("int") not_null
    int _question_id; // 题目id
    #pragma db type("text") 
    odb::nullable<std::string> _include;//题目头文件
    #pragma db type("text") not_null
    std::string _header; //题目主体
    #pragma db type("text") not_null
    std::string _tail; //题目尾部(测试)
    #pragma db type("varchar(32)") not_null
    std::string _language; //语言
};
