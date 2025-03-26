#pragma once

#include "hzpch.h"
#include "Hazel/Core.h"

namespace Hazel {

	// Hazel 事件系统目前是阻塞的，即事件发生时会立刻被派发并处理。
	// 在未来，可能会使用事件缓冲区（Event Bus）来存储事件，
	// 并在更新阶段的“事件处理”部分统一处理它们。

	// 事件类型枚举，表示不同的事件类别
	enum class EventType
	{
		None = 0,
		WindowClose,    // 窗口关闭
		WindowResize,   // 窗口大小改变
		WindowFocus,    // 窗口获得焦点
		WindowLostFocus,// 窗口失去焦点
		WindowMoved,    // 窗口移动
		AppTick,        // 应用程序时钟周期
		AppUpdate,      // 应用程序更新
		AppRender,      // 应用程序渲染
		KeyPressed,     // 按键按下
		KeyReleased,    // 按键释放
		MouseButtonPressed,  // 鼠标按钮按下
		MouseButtonReleased, // 鼠标按钮释放
		MouseMoved,     // 鼠标移动
		MouseScrolled   // 鼠标滚动
	};

	// 事件类别枚举，使用位运算标识事件类别
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0), // 应用程序相关事件
		EventCategoryInput = BIT(1),       // 输入相关事件
		EventCategoryKeyboard = BIT(2),    // 键盘事件
		EventCategoryMouse = BIT(3),       // 鼠标事件
		EventCategoryMouseButton = BIT(4)  // 鼠标按钮事件
	};

	// 事件类型宏：定义静态和虚函数，以便在运行时和编译时获取事件类型
#define EVENT_CLASS_TYPE(type) \
	static EventType GetStaticType() { return EventType::##type; }\
	virtual EventType GetEventType() const override { return GetStaticType(); }\
	virtual const char* GetName() const override { return #type; }

	// 事件类别宏：定义一个虚函数，用于返回事件所属的类别
#define EVENT_CLASS_CATEGORY(category) \
	virtual int GetCategoryFlags() const override { return category; }

	// 事件基类，所有事件都应继承自此类
	class HAZEL_API Event
	{
	public:
		bool Handled = false;
		// 纯虚函数，所有派生类都必须实现
		virtual EventType GetEventType() const = 0; // 获取事件类型
		virtual const char* GetName() const = 0;    // 获取事件名称
		virtual int GetCategoryFlags() const = 0;   // 获取事件类别标志
		virtual std::string ToString() const { return GetName(); } // 默认返回事件名称

		// 判断事件是否属于某个类别
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	};

	// 事件分发器，用于派发特定类型的事件
	class EventDispatcher
	{
		// 定义事件处理函数类型，接收 T 类型的事件并返回 bool
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		// 构造函数，接收一个事件实例
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		// 事件派发模板函数
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			// 判断事件类型是否匹配
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				// 调用处理函数，并标记事件已处理
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event; // 引用一个事件对象
	};

	// 重载 `<<` 操作符，使事件可以直接输出到流
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}
