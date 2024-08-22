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
    bool OneExpandHtml(const question& q,std::string& html,const std::string& language)
    {
         // 1. 形成路径
            std::string src_html = TEMP_PATH + "one_question.html";

            
            ctemplate::TemplateDictionary root("one_question");
            root.SetValue("number", std::to_string(q.head.number));
            root.SetValue("title", q.head.title);
            root.SetValue("star", q.head.star);
            root.SetValue("desc", q.desc);
            if(language=="java") root.SetValue("pre_code", q.java.header);
            else if(language=="python") root.SetValue("pre_code", q.py.header);
            else root.SetValue("pre_code", q.cpp.header);

            //3. 获取被渲染的html
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);
           
            //4. 开始完成渲染功能
            tpl->Expand(&html, &root);
            return true;
    }
private:
};