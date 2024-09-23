#include "imdecorated.h"

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <cstdlib>

using namespace ImDecorated;

Window::Window(int width, int height, const char* title) {
	spdlog::trace("Window::Window({}, {}, \"{}\")", width, height, title);

	// TODO: Why doesn't this get rid of the window decorations? (tested on KDE Plasma Wayland)
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	if (!glfwInit()) {
		const char* error;
		int code = glfwGetError(&error);
		spdlog::error("Failed to initialize GLFW: {} ({})", error, code);
		exit(EXIT_FAILURE);
	}

	context = glfwCreateWindow(width, height, title, nullptr, nullptr);

	if (!context) {
		const char* error;
		int code = glfwGetError(&error);
		spdlog::error("Failed to create GLFW window: {} ({})", error, code);
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(context);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		spdlog::error("Failed to initialize GLAD");
		exit(EXIT_FAILURE);
	}

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

#ifdef IMDECORATED_EXPERIMENTAL
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(context, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

Window::~Window() {
	spdlog::trace("Window::~Window()");

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(context);
	glfwTerminate();
}

void Window::gui(std::function<void(GLFWwindow*)> callback) {
	spdlog::trace("Window::gui({})", (void*)callback.target_type().name());
	guiCallback = callback;
}

void Window::launch() {
	spdlog::trace("Window::launch()");

	ImGuiIO& io = ImGui::GetIO();

	bool show_demo_window = true;

	bool window_should_be_open = true;

	while (!glfwWindowShouldClose(context)) {
		if (!window_should_be_open) {
			glfwSetWindowShouldClose(context, true);
		}

		glfwPollEvents();

		if (!guiCallback) {
			ImGui_ImplGlfw_Sleep(10);
			spdlog::warn("No GUI callback set, sleeping for 10ms");
			glClearColor(0.07f, 0.13f, 0.17f, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);
			glfwSwapBuffers(context);
			continue;
		}

		if (glfwGetWindowAttrib(context, GLFW_ICONIFIED) != 0) {
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(io.DisplaySize);
		if (window_should_be_open) {
			ImGui::Begin("ImDecorated", &window_should_be_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);
			guiCallback(context);
			ImGui::End();
		}

		ImGui::Render();

		int display_w, display_h;
		glfwGetFramebufferSize(context, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.07f, 0.13f, 0.17f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#ifdef IMDECORATED_EXPERIMENTAL
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
#endif

		glfwSwapBuffers(context);
	}
}
