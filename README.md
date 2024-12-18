![概要](/img/OJ.png "项目概要")  
# 组件框架介绍
## ETCD
## bRPC
## ODB+MySQL
## Redis
## log
# 编译判题子服务  
&nbsp;&nbsp;&nbsp;&nbsp;提供对用户提交的代码的编译运行服务, 由网关组织完整代码, 使用RPC远程调用该服务
&nbsp;&nbsp;&nbsp;&nbsp;**现在已经支持C/Cpp/Java/Python**  
**提供以下接口**
``` proto
message CompileRequest 
{
    string code = 1;                //用户提交的代码+测试代码
    string input = 2;               //用户用来自测输入
    string language = 3;            //代码的语言类型
    int32 cpu_limit = 4;            //对这份代码的时间限制  
    int32 mem_limit = 5;            //内存限制  
}
message CompileResponse 
{
    string reason = 1;              //状态码描述
    int32 status = 2;               //结果状态码
    optional string stderr = 3;     //如果运行成功的标准错误
    optional string stdout = 4;     //如果运行成功的标准输出
}
service CompileService 
{
    rpc Compile(CompileRequest) returns (CompileResponse);
}
```
## 大概思路
- 编译模块通过创建子进程执行g++,gcc,javac等命令完成,如果编译错误,向临时文件输出信息  
- 运行模块通过创建子进程执行可执行程序或者调用python解释器,重定向标准输入,输出,错误到临时文件  
- 编译运行模块分析状态码,组织响应
- 使用bRPC包装
# 题目管理子服务
&nbsp;&nbsp;&nbsp;&nbsp;提供对题目的增删改查
**提供以下接口**
``` protobuf
message QuestionHeadList 
{
    int64 number = 1;              //题目编号
    string title = 2;               //题目标题
    string star = 3;                //题目难度
}
message QuestionInfo
{
    int64 number = 1;              //题目编号
    string title = 2;               //题目标题
    string star = 3;                //题目难度
    string description = 4;         //题目描述
    string pre_code = 5;            //题目预设代码
    int32 cpu_limit = 6;            //对这份代码的时间限制  
    int32 mem_limit = 7;            //内存限制
}
//获取所有题目列表
message GetQuestionListRequest 
{
    string request_id = 1;          //请求ID
}
message GetQuestionListResponse 
{
    repeated QuestionHeadList question_list = 1; //题目列表
}
//获取单个题目详情
message GetQuestionRequest
{
    int64 number=1;                  //题目编号
    string laguage=2;                //编程语言
}
message GetQuestionResponse
{
    QuestionInfo question_info=1;   //题目详情
}
//搜索题目
message SearchQuestionRequest
{
    string keyword=1;                //关键字
}
message SearchQuestionResponse
{
    repeated QuestionHeadList question_list=1; //题目列表
}
//获取题目隐藏代码
message GetQuestionCodeRequest
{
    int64 number=1;                  //题目编号
    string laguage=2;                //编程语言
}
message GetQuestionCodeResponse
{
    string include=1;               //头文件
    string test=2;                  //测试代码
}
service QuestionService
{
    rpc GetQuestionList(GetQuestionListRequest) returns (GetQuestionListResponse);
    rpc GetQuestion(GetQuestionRequest) returns (GetQuestionResponse);
    rpc SearchQuestion(SearchQuestionRequest) returns (SearchQuestionResponse);
    rpc GetQuestionCode(GetQuestionCodeRequest) returns (GetQuestionCodeResponse);
}
```
## 大概思路
- 使用ODB映射数据库增删改查
- 使用bRPC包装
# 文章管理子服务
&nbsp;&nbsp;&nbsp;&nbsp;提供对文章的获取, 文章以Makedown的形式存在数据库, 使用工具解析为html
**接口**
``` protobuf
syntax = "proto3";
package MindbniM;
option cc_generic_services = true;
message ForumHead
{
    int64 number = 1;               //帖子编号
    string title = 2;               //帖子标题
    string author = 3;              //作者
}
message ForumInfo
{
    ForumHead forum_head = 1;       //帖子头部
    string content = 2;             //帖子内容
}
//获取所有帖子列表
message GetForumListRequest
{
    string request_id = 1;          //请求ID
}
message GetForumListResponse
{
    repeated ForumHead forum_list = 1; //帖子列表
}
//获取单个帖子详情
message GetForumRequest
{
    int64 number = 1;               //帖子编号
}
message GetForumResponse
{
    ForumInfo forum_info = 1;       //帖子详情
}
service ForumService
{
    rpc GetForumList(GetForumListRequest) returns (GetForumListResponse);
    rpc GetForum(GetForumRequest) returns (GetForumResponse);
}
```
## 大概思路
- 使用ODB映射数据库增删改查
- 使用bRPC包装
# 用户管理子服务
&nbsp;&nbsp;&nbsp;&nbsp;提供对用户管理
**提供以下RPC调用**
``` protobuf
message UserInfo
{
    string user_id = 1;         //用户 ID
    string nickname = 2;        //昵称
}

//用户名注册 
message UserRegisterReq 
{
    string request_id = 1;
    string nickname = 2;
    string password = 3;
} 
message UserRegisterRsp 
{
    string request_id = 1;
    bool success = 2;
    optional string errmsg = 3;
} 
//----------------------------
//用户名登录 
message UserLoginReq 
{
    string request_id = 1;
    string nickname = 2;
    string password = 3;
} 
message UserLoginRsp 
{
    string request_id = 1;
    bool success = 2;
    optional string errmsg = 3;
    optional string login_session_id = 4;
} 
//----------------------------
//个人信息获取-这个只用于获取当前登录用户的信息
message GetUserInfoReq 
{
    string request_id = 1;
    optional string user_id = 2;
    optional string session_id = 3;
} 
message GetUserInfoRsp 
{
    string request_id = 1;
    bool success = 2;
    optional string errmsg = 3; 
    optional UserInfo user_info = 4;
} 
//用户名修改
message SetUserNicknameReq 
{
    string request_id = 1;
    optional string user_id = 2;
    optional string session_id = 3;
    string nickname = 4;
} 
message SetUserNicknameRsp 
{
    string request_id = 1;
    bool success = 2;
    string errmsg = 3;
}
//----------------------------
service UserService 
{
    rpc UserRegister(UserRegisterReq) returns (UserRegisterRsp);
    rpc UserLogin(UserLoginReq) returns (UserLoginRsp);
    rpc GetUserInfo(GetUserInfoReq) returns (GetUserInfoRsp);
    rpc SetUserNickname(SetUserNicknameReq) returns(SetUserNicknameRsp);
}
```
# 网关服务
&nbsp;&nbsp;&nbsp;&nbsp;对网页请求应答, 转发给各种子服务并组织响应
使用httplib库
``` cpp
"/all-question"     //获取题目列表
"/question/(\d+)/"  //获取单个题目
"/judge/(\d+)       //编译判题
"/login"            //用户登录
"/all-forum"        //获取文章列表
```





