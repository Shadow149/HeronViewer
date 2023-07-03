#include "Window.h"


Window::Window(const int w, const int h) {
	scr_width_ = w;
	scr_height_ = h;
}

int Window::init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	window_ = glfwCreateWindow(scr_width_, scr_height_, "Heron", nullptr, nullptr);
	if (window_ == nullptr)
	{
		Console::log("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window_);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Console::log("Failed to initialize GLAD");
		return -1;
	}
	glfwMaximizeWindow(window_);

	glfwSwapInterval(1);

	Console::log("OpenGL init complete!");

	Console::log("=== CONTEXT INFO ===");
	Console::log("VERSION: " + std::string((char*)glGetString(GL_VERSION)));
	Console::log("VENDOR: " + std::string((char*)(glGetString(GL_VENDOR))));
	Console::log("RENDERER: " + std::string((char*)(glGetString(GL_RENDERER))));
	Console::log("===");

	GLint foo = 0;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_COUNT, &foo);
	Console::log("MAX WORK GROUPS: " + std::to_string(foo));
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_SIZE, &foo);
	Console::log("MAX WORK GROUP SIZE: " + std::to_string(foo));
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &foo);
	Console::log("MAX INVS: " + std::to_string(foo));
	glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &foo);
	Console::log("MAX SHARED MEMORY: " + std::to_string(foo));

	return 1;
}

void Window::set_background() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Window::init_im_gui() const
{
	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImNodes::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window_, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF("./fonts/OpenSans-Regular.ttf", 16);
	io.Fonts->AddFontFromFileTTF("./fonts/OpenSans-Regular.ttf", 16);
	io.Fonts->AddFontFromFileTTF("./fonts/OpenSans-Regular.ttf", 32);
	io.Fonts->AddFontFromFileTTF("./fonts/OpenSans-Regular.ttf", 11);
	io.Fonts->AddFontFromFileTTF("./fonts/OpenSans-Regular.ttf", 11);
	io.Fonts->Build();


	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->GrabRounding = 100.0f;
	style->FramePadding = ImVec2(10, 4);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.145f, 0.145f, 0.145f, 1.00f);

	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.504f, 0.504f, 0.504f, 1.00f);

	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);

	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.091, 0.091, 0.091, 1);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.091, 0.091, 0.091, 1);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.091, 0.091, 0.091, 1);

	style->Colors[ImGuiCol_Button] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);

	style->Colors[ImGuiCol_Tab] = ImVec4(0.404f, 0.404f, 0.404f, 1.00f);
	style->Colors[ImGuiCol_TabHovered] = ImVec4(0.504f, 0.504f, 0.504f, 1.00f);
	style->Colors[ImGuiCol_TabActive] = ImVec4(0.504f, 0.504f, 0.504f, 1.00f);


}

void Window::im_gui_render_init() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport();
}

void Window::im_gui_clean_up()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();
}

void Window::im_gui_render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
