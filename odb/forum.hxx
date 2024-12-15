#pragma once;
#include <string>
#include <odb/core.hxx>
#include <cstddef>
#include <odb/nullable.hxx>
namespace MindbniM
{
    #pragma db object table("forum")
    class Forum
    {
        friend class odb::access;
    public:
        int id(){return _id;}
        void title(const std::string& title) { _title = title; }
        std::string title() const { return _title; }
        void content(const std::string& content) { _content = content; }
        std::string content() const { return _content; }
    private:
        #pragma db id auto
        int _id;
        #pragma db type("varchar(64)") index 
        std::string _title;
        #pragma db type("text")
        std::string _content;
    };
}