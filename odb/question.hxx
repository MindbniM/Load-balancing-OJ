#pragma once

#include <string>
#include <odb/core.hxx>
#include <cstddef>
#include <odb/nullable.hxx>

#pragma db object table("questions")
class Questions
{
public:
    Questions() = default;

    Questions(const std::string& title, const std::string& star, const std::string& question_desc, int time_limit = 1, int mem_limit = 5000000)
        : title_(title),
          star_(star),
          question_desc_(question_desc),
          time_limit_(time_limit),
          mem_limit_(mem_limit)
    {}

    int id() const { return id_; }

    const std::string& title() const { return title_; }
    void title(const std::string& title) { title_ = title; }

    const std::string& star() const { return star_; }
    void star(const std::string& star) { star_ = star; }

    const std::string& question_desc() const { return question_desc_; }
    void question_desc(const std::string& question_desc) { question_desc_ = question_desc; }

    int time_limit() const { return time_limit_; }
    void time_limit(int time_limit) { time_limit_ = time_limit; }

    int mem_limit() const { return mem_limit_; }
    void mem_limit(int mem_limit) { mem_limit_ = mem_limit; }

private:
    friend class odb::access;
    // 数据库字段
    #pragma db id auto
    int id_; // 主键，自增

    #pragma db type("varchar(64)") not_null
    std::string title_; // 题目标题

    #pragma db type("varchar(8)") not_null
    std::string star_; // 题目难度

    #pragma db not_null
    std::string question_desc_; // 题目描述
    #pragma db type("int") 
    int time_limit_ = 1; // 时间限制，默认值为1
    #pragma db type("int") 
    int mem_limit_ = 5000000; // 空间限制，默认值为5000000
};
