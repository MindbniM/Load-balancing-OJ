#include"gateway_service.hpp"
using namespace MindbniM;
DEFINE_string(log_file, "stdout", "指定日志的输出文件");
DEFINE_int32(log_level, 1, "指定日志输出等级");
DEFINE_string(base_service, "/service", "服务监控根目录");
DEFINE_string(registry_host, "http://127.0.0.1:2379", "服务注册中心地址");
int main()
{
    GatewayServiceBuild gb;
    LoggerManager::GetInstance()->InitRootLog(FLAGS_log_file,FLAGS_log_level);
    gb.make_dis_object(FLAGS_registry_host,FLAGS_base_service);
    GatewayService::ptr p=gb.newGatewayService();
    p->listen(8848);
    return 0;
}
