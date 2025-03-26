#pragma once

#include "hzpch.h"         // Ԥ����ͷ�ļ������ٱ���
#include "Application.h"   // Ӧ�ó����������
#include "Log.h"           // ��־ϵͳ
#include <glad/glad.h>

namespace Hazel {

	// �궨�壺����Ա�����󶨵� std::bind ���ǰ����ص�this����
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

// ���캯�����������ڲ������¼��ص�
	Application::Application() {
		// ʹ������ָ�봴�����ڶ���
		m_Window = std::unique_ptr<Window>(Window::Create());

		// ���¼�����ص�
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	// ���������������⴦��
	Application::~Application() {}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}

	// �¼�������
	void Application::OnEvent(Event& e)
	{
		// �����¼��ַ������ַ���ǰ�¼�
		EventDispatcher dispatcher(e);

		// ����¼��Ƿ�Ϊ WindowCloseEvent�����ڹر��¼���
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		// ��¼�¼���־������¼����ַ�����ʾ
		HZ_CORE_TRACE("{0}", e.ToString());
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	// Ӧ�ó�������ѭ��
	void Application::Run() {
		HZ_CORE_INFO("Application Started"); // ��¼Ӧ�ó���������Ϣ

		// ��ѭ������������ʱ����ִ��
		while (m_Running)
		{
			// ���ñ�����ɫ����ɫ��
			glClearColor(1, 0, 1, 1);

			// �����ɫ������
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			// ���´��ڣ����������¼��ȣ�
			m_Window->OnUpdate();
		}
	}

	// �����ڹر��¼�
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false; // �رմ���ʱֹͣ��ѭ��
		return true;       // �¼����ɹ�����
	}

}
