#include "Window.h"

Window::Window(int w, int h) {
	SCR_WIDTH = w;
	SCR_HEIGHT = h;
}

int Window::init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Heron", NULL, NULL);
	if (window == NULL)
	{
		Console::log("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Console::log("Failed to initialize GLAD");
		return -1;
	}
	glfwMaximizeWindow(window);

	glfwSwapInterval(1);

	Console::log("OpenGL init complete!");

	return 1;
}

void Window::setBackground() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Window::initImGui()
{
	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGuiContext* context =  ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	//ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("OpenSans-Regular.ttf", 20);

	ImGui::GetStyle().GrabRounding = 100.0f;
	ImGui::GetStyle().FramePadding = ImVec2(10, 4);
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.145f, 0.145f, 0.145f, 1.00f);

	ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);
	ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.504f, 0.504f, 0.504f, 1.00f);

	ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);
	ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);
	ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);

	ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImVec4(0.091, 0.091, 0.091, 1);
	ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = ImVec4(0.091, 0.091, 0.091, 1);
	ImGui::GetStyle().Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.091, 0.091, 0.091, 1);

	ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);
	ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);
	ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);

	ImGui::GetStyle().Colors[ImGuiCol_Tab] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);
	ImGui::GetStyle().Colors[ImGuiCol_TabHovered] = ImVec4(0.504f, 0.504f, 0.504f, 1.00f);
	ImGui::GetStyle().Colors[ImGuiCol_TabActive] = ImVec4(0.504f, 0.504f, 0.504f, 1.00f);
}

void Window::ImGuiRenderInit() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport();
}

void Window::ImGuiCleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();
}

void Window::ImGuiRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
