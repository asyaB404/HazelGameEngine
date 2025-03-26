#include "hzpch.h"
#include "WindowsWindow.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/KeyEvent.h"

#include <glad/glad.h>

namespace Hazel {

	// ��̬��������� GLFW �Ƿ��Ѿ���ʼ��
	static bool s_GLFWInitialized = false;

	// GLFW ����ص�����
	static void GLFWErrorCallback(int error, const char* description)
	{
		HZ_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	// �������ڶ���ľ�̬����������һ�� WindowsWindow ʵ��
	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	// WindowsWindow ���캯�������� Init ����
	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	// WindowsWindow �������������� Shutdown ����
	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	// ���ڳ�ʼ������
	void WindowsWindow::Init(const WindowProps& props)
	{
		// �洢��������
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		// ��¼���ڴ�����־
		HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		// ��� GLFW ��δ��ʼ��������г�ʼ��
		if (!s_GLFWInitialized)
		{
			// ��ʼ�� GLFW
			int success = glfwInit();
			HZ_CORE_ASSERT(success, "Could not initialize GLFW!");

			// ���� GLFW ����ص�
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		// ���� GLFW ����
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window); // �� OpenGL ������
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "Failed to initialize Glad!");
		glfwSetWindowUserPointer(m_Window, &m_Data); // �󶨴�������
		SetVSync(true); // ���ô�ֱͬ��

		// ���ô��ڴ�С�仯�ص�
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				// �������ڴ�С�ı��¼�
				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		// ���ô��ڹرջص�
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		// ���ü����¼��ص�
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

		// ������갴���¼��ص�
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

		// �����������¼��ص�
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		// ��������ƶ��¼��ص�
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	}

	// �رմ��ڣ����� GLFW ���ڶ���
	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	// ���ڸ��·����������¼�������������
	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();  // ���� GLFW �¼�
		glfwSwapBuffers(m_Window); // ����ǰ�󻺳���
	}

	// ���� VSync����ֱͬ����
	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1); // ������ֱͬ��
		else
			glfwSwapInterval(0); // �رմ�ֱͬ��

		m_Data.VSync = enabled; // ���� VSync ״̬
	}

	// ��ȡ VSync ״̬
	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

} // namespace Hazel
