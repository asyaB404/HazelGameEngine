#include "hzpch.h"
#include "WindowsWindow.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/KeyEvent.h"

#include <glad/glad.h>

namespace Hazel {

	// 静态变量，标记 GLFW 是否已经初始化
	static bool s_GLFWInitialized = false;

	// GLFW 错误回调函数
	static void GLFWErrorCallback(int error, const char* description)
	{
		HZ_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	// 创建窗口对象的静态方法，返回一个 WindowsWindow 实例
	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	// WindowsWindow 构造函数，调用 Init 方法
	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	// WindowsWindow 析构函数，调用 Shutdown 方法
	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	// 窗口初始化方法
	void WindowsWindow::Init(const WindowProps& props)
	{
		// 存储窗口属性
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		// 记录窗口创建日志
		HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		// 如果 GLFW 还未初始化，则进行初始化
		if (!s_GLFWInitialized)
		{
			// 初始化 GLFW
			int success = glfwInit();
			HZ_CORE_ASSERT(success, "Could not initialize GLFW!");

			// 设置 GLFW 错误回调
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		// 创建 GLFW 窗口
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window); // 绑定 OpenGL 上下文
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "Failed to initialize Glad!");
		glfwSetWindowUserPointer(m_Window, &m_Data); // 绑定窗口数据
		SetVSync(true); // 启用垂直同步

		// 设置窗口大小变化回调
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				// 触发窗口大小改变事件
				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		// 设置窗口关闭回调
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		// 设置键盘事件回调
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

		// 设置鼠标按键事件回调
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
				}
			});

		// 设置鼠标滚轮事件回调
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		// 设置鼠标移动事件回调
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	}

	// 关闭窗口，销毁 GLFW 窗口对象
	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	// 窗口更新方法，处理事件并交换缓冲区
	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();  // 处理 GLFW 事件
		glfwSwapBuffers(m_Window); // 交换前后缓冲区
	}

	// 设置 VSync（垂直同步）
	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1); // 开启垂直同步
		else
			glfwSwapInterval(0); // 关闭垂直同步

		m_Data.VSync = enabled; // 更新 VSync 状态
	}

	// 获取 VSync 状态
	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

} // namespace Hazel
