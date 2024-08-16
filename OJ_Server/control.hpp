#pragma once
#include"model.hpp"
#include"view.hpp"
class Control
{
public:
    Control() 
    {}
    bool get_all_question(std::string& html)
    {
        if(_v.AllExpandHtml(_m,html))
        {
            return true;
        }
        return false;
    }
    bool get_number_question(std::string& html)
    {
        
    }
private:
    Model _m;
    View _v;
};