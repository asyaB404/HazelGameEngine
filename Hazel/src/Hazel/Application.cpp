#pragma once

#include "hzpch.h"         // 预编译头文件，加速编译
#include "Application.h"   // 应用程序类的声明
#include "Log.h"           // 日志系统
#include <glad/glad.h>

namespace Hazel {

	// 宏定义：将成员函数绑定到 std::bind 就是把隐藏的this参数
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

// 构造函数：创建窗口并设置事件回调
	Application::Application() {
		// 使用智能指针创建窗口对象
		m_Window = std::unique_ptr<Window>(Window::Create());

		// 绑定事件处理回调
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	// 析构函数（无特殊处理）
	Application::~Application() {}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}

	// 事件处理函数
	void Application::OnEvent(Event& e)
	{
		// 创建事件分发器，分发当前事件
		EventDispatcher dispatcher(e);

		// 检查事件是否为 WindowCloseEvent（窗口关闭事件）
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		// 记录事件日志，输出事件的字符串表示
		HZ_CORE_TRACE("{0}", e.ToString());
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	// 应用程序运行循环
	void Application::Run() {
		HZ_CORE_INFO("Application Started"); // 记录应用程序启动信息

		// 主循环：程序运行时不断执行
		while (m_Running)
		{
			// 设置背景颜色（紫色）
			glClearColor(1, 0, 1, 1);

			// 清空颜色缓冲区
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			// 更新窗口（处理输入事件等）
			m_Window->OnUpdate();
		}
	}

	// 处理窗口关闭事件
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false; // 关闭窗口时停止主循环
		return true;       // 事件被成功处理
	}

}
