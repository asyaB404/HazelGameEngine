#pragma once

#ifdef HZ_PLATFORM_WINDOWS
using namespace Hazel;

#include "Log.h"

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv)
{
    Log::Init();
    HZ_CORE_WARN("Initialized Log");
    int a = 5;
    HZ_INFO("Hello! Var={0}", a);
    /*Hazel::Log::GetCoreLogger()->warn("Initialized Log");
    Hazel::Log::GetClientLogger()->info("Hello World!");*/
    printf("Hello World!\n");
    auto app = CreateApplication();
    app->Run();
    delete app;
}

#endif 



