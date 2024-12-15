#pragma once
#include"httplib.h"
#include"user.pb.h"
#include"question.pb.h"
#include"compile.pb.h"
#include"etcd.hpp"
#include"channel.hpp"
#include"json.hpp"
#include"forum.pb.h"
#include<ctemplate/template.h>

#define COMMUNITY_SERVER "/service/compile_service"
#define QUESTION_SERVER "/service/question_service"
#define USER_SERVER "/service/user_service"
#define FORUM_SERVER "/service/forum_service"

#define GET_ALL_QUESTION "/all-question"
#define GET_ONE_QUESTION R"(/question/(\d+)/)"
#define JUDGE R"(/judge/(\d+))"
#define LOGIN "/login"
#define GET_ALL_FORUM "/all-forum"
