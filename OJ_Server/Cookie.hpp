#include<iostream>
#include<sstream>
#include<memory>
#include<string>
#include<unordered_map>
#include"mysql.hpp"
struct User
{
    std::string name;
    std::string password;
    size_t num;//统计完成的题目
};
namespace SQL
{
    std::string FIND="select * from users where name='";
}
class Cookies
{
    using Cookie=uint32_t;
    Cookie get_Cookie(const std::string& name)
    {
        uint32_t cookie=0;
        for(auto& c:name)
        {
            cookie+=c*11;
        }
        return cookie;
    }
public:
    Cookie push(const std::string& name,const std::string& password)
    {
        std::string str=_m.exec(SQL::FIND+name+"'");
        if(str!="")
        {
            std::istringstream os(str);
            User u;
            os>>u.name>>u.password>>u.password;
            if(password==u.password)
            {
                Cookie coo=get_Cookie(u.name);
                _map[coo]=u;
                return coo;
            }
            else return 1;
        }
        else return 0;
    }
private:
    std::unordered_map<Cookie,User> _map;
    mysql_util::mysql _m;
};