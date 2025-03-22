#pragma once
#include "Hazel/Application.h"

#ifdef HZ_PLATFORM_WINDOWS
extern Hazel::Application* Hazel::CreateApplication();
int main(int argc, char** argv)
{
    printf("Hello World!\n");
    auto app = Hazel::CreateApplication();
    app->Run();
    delete app;
}

#endif // 
