#include "../pch.h"
#include "WindowsInput.h"
#include "Window.h"
#include "../Render/ViewPort.h"

using namespace Doom;

Input* Input::s_Instance = new WindowsInput();

void Doom::WindowsInput::Clearimp()
{
	pressedMouseButtons.clear();
	pressedKeysButtons.clear();
}

bool WindowsInput::IsKeyPressedimp(int keycode)
{
	auto iter = pressedKeysButtons.find(keycode);
	if (iter != pressedKeysButtons.end()) {
		return iter->second;
	}

	double time = glfwGetTime();
	const auto& window = static_cast<GLFWwindow*>(Window::GetWindow());
	if (!ViewPort::Instance()->IsActive)
		return GL_FALSE;
	auto state = glfwGetKey(window, keycode);
	if (state == GLFW_PRESS) {
		auto iter = pressedButtonsTime.find(keycode);
		if (iter == pressedButtonsTime.end()) {
			pressedButtonsTime.insert(std::make_pair(keycode, time));
			pressedKeysButtons.insert(std::make_pair(keycode, true));
			return true;
		}

		if (time - iter->second < DeltaTime::deltatime * 4) {
			iter->second = time;
			pressedButtonsTime.insert(std::make_pair(keycode, time));
			pressedKeysButtons.insert(std::make_pair(keycode, false));
			return false;
		}

		iter->second = glfwGetTime();
		pressedKeysButtons.insert(std::make_pair(keycode, true));
		return true;
	}
	else {
		pressedKeysButtons.insert(std::make_pair(keycode, false));
		return false;
	}
}

bool WindowsInput::IsMousePressedDownimp(int keycode) {
	const auto& window = static_cast<GLFWwindow*>(Window::GetWindow());

	if (!ViewPort::Instance()->IsHovered && (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive() || ImGui::IsAnyWindowFocused())) {
		return GLFW_FALSE;
	}

	if (ViewPort::Instance()->IsHovered) {
		auto state = glfwGetMouseButton(window, keycode);
		return state == GLFW_PRESS;
	}
}

bool Doom::WindowsInput::IsKeyPressedDownimp(int keycode)
{
	const auto& window = static_cast<GLFWwindow*>(Window::GetWindow());
	if (!ViewPort::Instance()->IsActive)
		return GL_FALSE;
	auto state = glfwGetKey(window, keycode);
	return state == GLFW_PRESS;
}

bool WindowsInput::IsMousePressedimp(int keycode) {

	auto iter = pressedMouseButtons.find(keycode);
	if (iter != pressedMouseButtons.end()) {
		return iter->second;
	}

	double time = glfwGetTime();

	const auto& window = static_cast<GLFWwindow*>(Window::GetWindow());
	
	if (!ViewPort::Instance()->IsHovered && (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive() || ImGui::IsAnyWindowFocused())) {
		return GLFW_FALSE;
	}
	
	if (ViewPort::Instance()->IsHovered) {
		auto state = glfwGetMouseButton(window, keycode);
		if (state == GLFW_PRESS) {
			auto iter = pressedButtonsTime.find(keycode);
			if (iter == pressedButtonsTime.end()) {
				pressedButtonsTime.insert(std::make_pair(keycode, time));
				pressedMouseButtons.insert(std::make_pair(keycode, true));
				return true;
			}

			if (time - iter->second < DeltaTime::deltatime * 3) {
				iter->second = time;
				pressedMouseButtons.insert(std::make_pair(keycode, false));
				return false;
			}
			iter->second = glfwGetTime();
			pressedMouseButtons.insert(std::make_pair(keycode, true));
			return true;
		}
		else {
			return false;
			pressedMouseButtons.insert(std::make_pair(keycode, false));
		}
	}
}
