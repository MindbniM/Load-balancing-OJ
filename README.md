![概要](/img/OJ.png "项目概要")  
# Compile_Server模块  
&nbsp;&nbsp;&nbsp;&nbsp;提供对用户提交的代码的编译运行服务,可以有多个这样的服务部署在不同的机器上, 由OJ-Server模块将用户提交的代码负载均衡式的发送给Complie_Server模块
&nbsp;&nbsp;&nbsp;&nbsp;**对外接收一个json**
``` json5
{  
    "code": "",//用户提交的代码+测试代码
    "input": "",//用户用来自测输入
    "language": "",//代码的语言类型
    "cpu_limit": ,//对这份代码的时间限制  
    "mem_limit": //内存限制  
}
```
&nbsp;&nbsp;&nbsp;&nbsp;**内部编译运行用户提交的代码,返回一个json字符串**
``` json5
{  
    "reason": "",//状态码描述  
    "status": ,//结果状态码
    [可选]"stderr":"",//如果运行成功的标准错误  
    [可选]"stdout": ""//如果运行成功的标准输出  
}  
```
## 大概思路
- 编译模块通过创建子进程执行g++,gcc等命令完成,如果编译错误,向临时文件输出信息  
- 运行模块通过创建子进程执行可执行程序或者调用python解释器,重定向标准输入,输出,错误到临时文件  
- 编译运行模块分析状态码,填充json并输出  
- 使用cpp-httplib库把这些打包成服务即可
## 扩展 / 优化思路
- 可使用进程池, 避免编译运行服务频繁创建子进程
- 支持更多语言, 为了避免代码冗余, 可以设计Compile/Run为基类, 派生类可以有Compile_cpp,Run_cpp,Compile_py,Run_py,Compile_java,Run_java等, Compile_Run类只调用Compile/Run, 使用多态调用
- 即便是编译服务在其他机器上，也其实是不太安全的，可以将编译服务部署在docker
- 使用rest_rpc, 替换我们的httplib, 或者自己实现应用层
# OJ_Server模块
&nbsp;&nbsp;&nbsp;&nbsp;提供对用户的请求应答
1. 对网页请求应答, 返回我们的前端网页
2. 提供题目列表
3. 对用户提交的代码, 能负载均衡的分发给Compile_Server, 应答判题情况

&nbsp;&nbsp;&nbsp;&nbsp;**用户提交一个json**
``` json5
{  
    "code": "",//用户提交的代码  
    "input": "",//用户用来自测输入
    "language": "",//代码的语言类型
}
```
&nbsp;&nbsp;&nbsp;&nbsp;**返回经过Compile_Server处理的json**
``` json5
{  
    "reason": "",//状态码描述  
    "status": ,//结果状态码
    [可选]"stderr":"",//如果运行成功的标准错误  
    [可选]"stdout": ""//如果运行成功的标准输出  
}  
```
将状态码描述打印在网页上, 并输出判题情况
## 大致思路
- 使用httplib库构建服务器, ctemplate库渲染前端网页
- 使用MVC风格
    - M: Model,通常是和数据交互的模块，比如，对题库进行增删改查（MySQL）
    - V: view, 通常是拿到数据之后，要进行构建网页，渲染网页内容，展示给用户的(浏览器)
    - C: control, 控制器，就是我们的核心业务逻辑
- model模块用于交互数据库, 获取题目信息, 由于题目不多, 第一次访问时将全部题目信息加载到内存
- view模块用于渲染前端网页, 使用ctemplate库
- control模块整合上面两个功能, 同时提供负载均衡选择Compile_Server
## 扩展/优化思路
- 加入论坛功能
- 给每到题目加上题解/评论功能, 显示通过人数
- 加入登录功能, 记录登录用户的刷题记录
- 优化负载均衡算法
- 优化主界面, 展示热门题目







