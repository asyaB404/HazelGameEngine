#include "Application.h"
#include "Log.h"
#include "Events/ApplicationEvent.h"


namespace Hazel {
	Application::Application() {}
	Application::~Application() {}
	void Application::Run() {
		HZ_CORE_INFO("Application Started");
        WindowResizeEvent e(1280, 720);
        if (e.IsInCategory(EventCategoryApplication))
        {
            HZ_TRACE(e);
        }
        if (e.IsInCategory(EventCategoryInput))
        {
            HZ_TRACE(e);
        }

        while (true);
	}
}