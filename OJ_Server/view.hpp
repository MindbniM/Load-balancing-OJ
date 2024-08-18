#pragma once
#include"model.hpp"
#include<ctemplate/template.h>
const std::string TEMP_PATH="../temp/";
class View
{
public:
    bool AllExpandHtml(const Model& m,std::string& html)
    {
        std::string path=TEMP_PATH+"all_question.html";
        ctemplate::TemplateDictionary root("all");
        for(auto& p:m)
        {
            ctemplate::TemplateDictionary* sub=root.AddSectionDictionary("question_list");
            sub->SetValue("number",std::to_string(p.second.head.number));
            sub->SetValue("title",p.second.head.title);
            sub->SetValue("star",p.second.head.star);
        }
        ctemplate::Template* temp=ctemplate::Template::GetTemplate(path,ctemplate::DO_NOT_STRIP);
        temp->Expand(&html,&root);
        return true;
    }
    bool OneExpandHtml(const Model& m,int number,std::string& html)
    {
        return true;
    }
private:
};