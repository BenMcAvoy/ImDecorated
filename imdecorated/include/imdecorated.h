#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ImDecorated {
class Window {
public:
	Window(int width, int height, const char* title);
	~Window();

	void gui(std::function<void(GLFWwindow*)> callback);

	void launch();

private:
	GLFWwindow* context = nullptr;
	std::function<void(GLFWwindow*)> guiCallback;
};
}
