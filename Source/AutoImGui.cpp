#include "AutoImGui.h"

#include <cstring>
#include <map>
#include <3rdParty/ImGui/imgui.h>
#include <3rdParty/ImGui/implot.h>

using namespace autoimgui;

static std::map<const char*, bool> window_states;

bool autoimgui::is_window_opened(const char* window_name)
{
	return window_states[window_name];
}

bool autoimgui::set_window_opened(const char* window_name, bool opened)
{
	return window_states[window_name] = opened;
	// TODO seriialize and save this
}

void autoimgui::perform()
{
	const char* imGuiDemoWindowName = "Dear ImGui Demo window";
	const char* imPlotDemoWindowName = "ImPlot Demo window";

	// Construct main menu bar
	if (ImGui::BeginMainMenuBar())
	{
		// Function queue
		if (ImGuiFunctionQueue::functionHead != nullptr)
		{
			const char* currentGroup = ImGuiFunctionQueue::functionHead->group;
			bool currentGroupOpened = ImGui::BeginMenu(currentGroup);
			for (ImGuiFunctionQueue* q = ImGuiFunctionQueue::functionHead; q; q = q->next)
			{
				if (_stricmp(currentGroup, q->group) != 0)
				{
					if (currentGroupOpened)
						ImGui::EndMenu();
					currentGroup = q->group;
					currentGroupOpened = ImGui::BeginMenu(currentGroup);
				}
				if (currentGroupOpened)
				{
					if (ImGui::MenuItem(q->name, q->hotkey))
					{
						assert(q->function);
						q->function();
					}
				}
			}
			if (currentGroupOpened)
				ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("ImGui"))
		{
			ImGui::Separator();
			bool showImGuiDemoWindow = is_window_opened(imGuiDemoWindowName);
			bool oldShowImGuiDemoWindow = showImGuiDemoWindow;
			ImGui::MenuItem(imGuiDemoWindowName, nullptr, &showImGuiDemoWindow);
			if (showImGuiDemoWindow != oldShowImGuiDemoWindow)
				set_window_opened(imGuiDemoWindowName, showImGuiDemoWindow);
			bool showImPlotDemoWindow = is_window_opened(imPlotDemoWindowName);
			bool oldShowImPlotDemoWindow = showImPlotDemoWindow;
			ImGui::MenuItem(imPlotDemoWindowName, nullptr, &showImPlotDemoWindow);
			if (showImPlotDemoWindow != oldShowImPlotDemoWindow)
				set_window_opened(imPlotDemoWindowName, showImPlotDemoWindow);
			ImGui::EndMenu();
		}

		// Window queue
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGuiFunctionQueue::windowHead != nullptr)
			{
				for (ImGuiFunctionQueue* q = ImGuiFunctionQueue::windowHead; q; q = q->next)
				{
					q->opened = is_window_opened(q->name);
					bool oldOpened = q->opened;
					ImGui::MenuItem(q->name, q->hotkey, &q->opened);
					if (q->opened != oldOpened)
						set_window_opened(q->name, q->opened);
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	// Execute window functions
	for (ImGuiFunctionQueue* q = ImGuiFunctionQueue::windowHead; q; q = q->next)
	{
		q->opened = is_window_opened(q->name);
		if (q->opened)
		{
			assert(q->function);
			bool oldOpened = q->opened;
			ImGui::Begin(q->name, &q->opened, q->windowFlags);
			if (q->opened != oldOpened)
				set_window_opened(q->name, q->opened);
			q->function();
			ImGui::End();
		}
	}

	// Show demo windows
	{
		bool showImGuiDemoWindow = is_window_opened(imGuiDemoWindowName);
		if (showImGuiDemoWindow)
		{
			bool oldShowImGuiDemoWindow = showImGuiDemoWindow;
			ImGui::ShowDemoWindow(&showImGuiDemoWindow);
			if (showImGuiDemoWindow != oldShowImGuiDemoWindow)
				set_window_opened(imGuiDemoWindowName, showImGuiDemoWindow);
		}
		bool showImPlotDemoWindow = is_window_opened(imPlotDemoWindowName);
		if (showImPlotDemoWindow)
		{
			bool oldShowImPlotDemoWindow = showImPlotDemoWindow;
			ImPlot::ShowDemoWindow(&showImPlotDemoWindow);
			if (showImPlotDemoWindow != oldShowImPlotDemoWindow)
				set_window_opened(imPlotDemoWindowName, showImPlotDemoWindow);
		}
	}
}

ImGuiFunctionQueue* ImGuiFunctionQueue::windowHead = nullptr;
ImGuiFunctionQueue* ImGuiFunctionQueue::functionHead = nullptr;

ImGuiFunctionQueue::ImGuiFunctionQueue(
	const char* group_, const char* name_, const char* hotkey_, int priority_, unsigned int window_flags, ImGuiFuncPtr func, bool is_window)
	: group(group_), name(name_), hotkey(hotkey_), priority(priority_), windowFlags(window_flags), function(func)
{
	ImGuiFunctionQueue** head = is_window ? &windowHead : &functionHead;
	if (*head == nullptr)
	{
		*head = this;
		return;
	}

	ImGuiFunctionQueue* n = *head, * p = nullptr;
	for (; n; p = n, n = n->next)
	{
		int cmp = is_window ? 0 : _stricmp(group, n->group);
		if (cmp < 0 || (cmp == 0 && priority < n->priority))
		{
			// insert before
			next = n;
			if (p)
				p->next = this;
			else
				*head = this;
			return;
		}
	}
	p->next = this;
}