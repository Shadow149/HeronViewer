#include "Utils.h"

// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float clamp(float v, const float l, const float h)
{
	if (v > h)
		v = h;
	if (v < l)
		v = l;
	return v;
}

void add_mat(const float* first, const float* second, float* result, const int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[(size * i) + j] = first[(size * i) + j] + second[(size * i) + j];
		}
	}
}

void sub_mat(const float* first, const float* second, float* result, const int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[(size * i) + j] = first[(size * i) + j] - second[(size * i) + j];
		}
	}
}

void scalar_mul(const float c, const float* ker, float* result, const int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[(size * i) + j] = ker[(size * i) + j] * c;
		}
	}
}

void normal_mat(const float* ker, float* result, const int size) {
	float sum = 0;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			sum += ker[(size * i) + j];
		}
	}
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			result[(size * i) + j] = ker[(size * i) + j] / sum;
		}
	}
}

std::string strip_extension(std::string s)
{
	const std::size_t p = s.find('.');
	if (p == std::string::npos)
		return "";
	std::string out = s.erase(p, s.length()-1);
	return out;
}


ImVec2 get_resize_size(const GLsizei width, const GLsizei height)
{
	ImVec2 preview_size;
	if (width > height && width > ImGui::GetWindowWidth())
	{
		preview_size.x = ImGui::GetWindowWidth();
		preview_size.y = static_cast<GLsizei>(ImGui::GetWindowWidth() * (static_cast<double>(height) / width));
	}
	else if (height > ImGui::GetWindowHeight())
	{
		const float win_height = ImGui::GetWindowHeight() - 30;
		preview_size.x = static_cast<GLsizei>(win_height * (static_cast<double>(width) / height));
		preview_size.y = win_height;
	}
	else
	{
		preview_size.x = width;
		preview_size.y = height;
	}
	return preview_size;
}

// https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#example-for-opengl-users
// Simple helper function to load an image into a OpenGL texture with common settings
bool load_texture_from_file(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}