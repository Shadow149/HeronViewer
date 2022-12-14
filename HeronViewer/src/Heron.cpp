#include "Heron.h"
#include "FileDialog.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Heron::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
		clip = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE) {
		clip = false;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		b4 = true;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
		b4 = false;
	}
}

void Heron::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float scale = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		glm::vec3 s = glm::vec3(1 + yoffset * scale, 1 + yoffset * scale, 0);
		view_scale += yoffset * scale;
		image->scale(s);
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		glm::vec3 off = glm::vec3(yoffset * scale, 0, 0);
		view_pos += off;
		image->translate(off);
	}
	else {
		glm::vec3 off = glm::vec3(xoffset * scale, yoffset * scale, 0);
		view_pos += off;
		image->translate(off);
	}
}

void Heron::setImagePath(std::string s) {
	imgFilePath = s;
}

void Heron::loadImage(std::string filePath, std::string fileName) {
	Console::log("Change Image : " + filePath);
	imgFile = filePath;
	image->imageLoader = std::thread(&Image::getImage, image, imgFile.c_str());
	editor->updateFile(fileName, filePath);
	editor->loaded(true);
	image->resetView();
}

void Heron::unloadImage() {
	image->unload();
	editor->reset();
	editor->loaded(false);
	image->resetView();
}

void Heron::static_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Heron* that = static_cast<Heron*>(glfwGetWindowUserPointer(window));
	that->scroll_callback(window, xoffset, yoffset);
}

void Heron::initGlfw()
{
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, static_scroll_callback);
}

void Heron::initModules() {
	fileDialog = new FileDialog(this, "File Dialog");
	image = new Image("Image", &hist);
	hist = new Histogram(image, "Histogram");
	editor = new Editor(image, "Editor");
	modules.push_back(new MainPanel(this, "Main"));
	modules.push_back(fileDialog);
	modules.push_back(editor);
	modules.push_back(hist);
	modules.push_back(new Curve(image, "Curve"));
	modules.push_back(new Console("Console"));
	modules.push_back(new Overlay("Overlay"));
	modules.push_back(Preferences::instance());
	modules.push_back(image);

	fpsMetric = Overlay::registerMetric();
	frameTime = Overlay::registerMetric();
	renderTime = Overlay::registerMetric();
	imGuiRenderTime = Overlay::registerMetric();
}

std::vector<Module*> Heron::getModules() {
	return modules;
}

std::string Heron::getFileDialogKey() {
	return fileDialog->name;
}

void Heron::saveImage() {
	editor->updateConfigFile();
}

void Heron::calcTime() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	*fpsMetric = "Fps: " + std::to_string(1 / deltaTime);
#ifdef SHOW_FPS
	Overlay::updateFps(1.0f / deltaTime);
#endif
	*frameTime = "Frame Time: " + std::to_string(deltaTime);
	
}

void Heron::render()
{
	initGlfw();
	initModules();
	initImGui();

	for (Module* m : modules) {
		m->init();
	}

	while (!glfwWindowShouldClose(window))
	{
		calcTime();
		processInput(window);

		float start = glfwGetTime();
		image->glrender(&clip, &b4);
		*renderTime = "Gl Render Time: " + std::to_string(glfwGetTime() - start);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		start = glfwGetTime();

		ImGuiRenderInit();

		for (Module* m : modules) {
			//m->setStyle();
			m->render();
		}

		ImGuiRender();

		*imGuiRenderTime = "ImGui Render Time: " + std::to_string(glfwGetTime() - start);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGuiCleanUp();

	for (Module* m : modules) {
		m->cleanup();
	}

	glfwTerminate();

}
