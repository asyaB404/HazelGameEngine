#pragma once

#include "hzpch.h"
#include "Hazel/Core.h"

namespace Hazel {

	// Hazel �¼�ϵͳĿǰ�������ģ����¼�����ʱ�����̱��ɷ�������
	// ��δ�������ܻ�ʹ���¼���������Event Bus�����洢�¼���
	// ���ڸ��½׶εġ��¼���������ͳһ�������ǡ�

	// �¼�����ö�٣���ʾ��ͬ���¼����
	enum class EventType
	{
		None = 0,
		WindowClose,    // ���ڹر�
		WindowResize,   // ���ڴ�С�ı�
		WindowFocus,    // ���ڻ�ý���
		WindowLostFocus,// ����ʧȥ����
		WindowMoved,    // �����ƶ�
		AppTick,        // Ӧ�ó���ʱ������
		AppUpdate,      // Ӧ�ó������
		AppRender,      // Ӧ�ó�����Ⱦ
		KeyPressed,     // ��������
		KeyReleased,    // �����ͷ�
		MouseButtonPressed,  // ��갴ť����
		MouseButtonReleased, // ��갴ť�ͷ�
		MouseMoved,     // ����ƶ�
		MouseScrolled   // ������
	};

	// �¼����ö�٣�ʹ��λ�����ʶ�¼����
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0), // Ӧ�ó�������¼�
		EventCategoryInput = BIT(1),       // ��������¼�
		EventCategoryKeyboard = BIT(2),    // �����¼�
		EventCategoryMouse = BIT(3),       // ����¼�
		EventCategoryMouseButton = BIT(4)  // ��갴ť�¼�
	};

	// �¼����ͺ꣺���徲̬���麯�����Ա�������ʱ�ͱ���ʱ��ȡ�¼�����
#define EVENT_CLASS_TYPE(type) \
	static EventType GetStaticType() { return EventType::##type; }\
	virtual EventType GetEventType() const override { return GetStaticType(); }\
	virtual const char* GetName() const override { return #type; }

	// �¼����꣺����һ���麯�������ڷ����¼����������
#define EVENT_CLASS_CATEGORY(category) \
	virtual int GetCategoryFlags() const override { return category; }

	// �¼����࣬�����¼���Ӧ�̳��Դ���
	class HAZEL_API Event
	{
	public:
		bool Handled = false;
		// ���麯�������������඼����ʵ��
		virtual EventType GetEventType() const = 0; // ��ȡ�¼�����
		virtual const char* GetName() const = 0;    // ��ȡ�¼�����
		virtual int GetCategoryFlags() const = 0;   // ��ȡ�¼�����־
		virtual std::string ToString() const { return GetName(); } // Ĭ�Ϸ����¼�����

		// �ж��¼��Ƿ�����ĳ�����
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	};

	// �¼��ַ����������ɷ��ض����͵��¼�
	class EventDispatcher
	{
		// �����¼����������ͣ����� T ���͵��¼������� bool
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		// ���캯��������һ���¼�ʵ��
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		// �¼��ɷ�ģ�庯��
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			// �ж��¼������Ƿ�ƥ��
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				// ���ô�������������¼��Ѵ���
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event; // ����һ���¼�����
	};

	// ���� `<<` ��������ʹ�¼�����ֱ���������
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}
