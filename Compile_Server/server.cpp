#include"Compile.hpp"
#include"server.hpp"
int main()
{
    ADD_APPENDER_STDOUT(Log_util::log_level::DEBUG);
    Compile::Compile_file("test");
    return 0;
}