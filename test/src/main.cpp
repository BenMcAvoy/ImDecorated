#include "imdecorated.h"

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <imgui.h>

int main() {
	spdlog::set_level(spdlog::level::trace);

	ImDecorated::Window window(800, 600, "ImDecorated");

	window.gui([&](GLFWwindow* context) {
		ImGui::Text("Hello, world!");
	});

	window.launch();
}
