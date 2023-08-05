#include "gl_texture.h"

#include <cstring>

void gl_quad::gen()
{
	constexpr float vertices[] = {
		// positions          // colors           // texture coords
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
		1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		-1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left 
	};
	const unsigned indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);
	glGenBuffers(1, &ebo_);

	glBindVertexArray(vao_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void gl_quad::draw() const
{
	glBindVertexArray(vao_);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void gl_quad::cleanup()
{
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(1, &vbo_);
	glDeleteBuffers(1, &ebo_);
}

void gl_buffer::gen(const GLsizeiptr buffer_size, const GLenum usage, const void* data)
{
	buffer_size_ = buffer_size;
	glGenBuffers(1, &id_);
	glBindBuffer(target_, id_);
	glBufferData(target_, buffer_size, data, usage);
}

void gl_buffer::clear(const GLenum internal_format, const GLenum format, const GLenum type) const
{
	bind();
	glClearBufferData(target_, internal_format, format, type, nullptr);
}

void gl_buffer::get_data(void* data) const
{
	bind();
	glGetBufferSubData(target_, 0, buffer_size_, data);
}

void gl_ssbo::gen(const GLsizeiptr buffer_size, const GLenum usage, const void* data)
{
	gl_buffer::gen(buffer_size, usage, data);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_, id_);
}

void gl_texture::gen(const GLsizei w, const GLsizei h, const GLint internal_format, const GLenum format,
	const GLenum type, const void* data, const GLint filter)
{
	glGenTextures(1, &id_);
	init(w, h, internal_format, format, type, data, filter);
}

void gl_texture::init(const GLsizei w, const GLsizei h, const GLint internal_format, const GLenum format,
	const GLenum type, const void* data, const GLint filter)
{
	glBindTexture(GL_TEXTURE_2D, id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, type, data);
}

void gl_texture::bind() const
{
	glActiveTexture(GL_TEXTURE0 + binding_);
	glBindTexture(GL_TEXTURE_2D, id_);
}

void gl_image::init(const GLsizei w, const GLsizei h, const GLint internal_format, const GLenum format,
	const GLenum type, const void* data, const GLint filter)
{
	gl_texture::init(w, h, internal_format, format, type, data, filter);
	// TODO param for r/w
	glBindImageTexture(binding_, id_, 0, GL_FALSE, 0, GL_READ_WRITE, internal_format);
}

void gl_image::get_data_via_pbo(const gl_pbo* p_pbo, void* dst) const
{
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id_);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, p_pbo->get_id());
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	// TODO GL_RGB, GL_UNSIGNED_BYTE types hard-coded
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	const void* d = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	std::memcpy(dst, d, p_pbo->get_buffer_size());
}

void gl_framebuffer_texture::init(const GLsizei w, const GLsizei h, const GLint internal_format, const GLenum format,
	const GLenum type, const void* data, const GLint filter)
{
	gl_texture::init(w, h, internal_format, format, type, data, filter);
	// WARNING - always attaching to GL_COLOR_ATTACHMENT0 <-- 0th colour attachment
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, id_, 0);
}

void gl_framebuffer::gen(const GLsizei w, const GLsizei h, const GLint internal_format, const GLenum format,
	const GLenum type, const void* data, const GLint filter)
{
	glGenFramebuffers(1, &id_);
	init(w, h, internal_format, format, type, data, filter);
	constexpr GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);
}

void gl_framebuffer::init(const GLsizei w, const GLsizei h, const GLint internal_format, const GLenum format,
	const GLenum type, const void* data, const GLint filter)
{
	glBindFramebuffer(GL_FRAMEBUFFER, id_);
	framebuffer_texture_.gen(w, h, internal_format, format, type, data, filter);
}

void gl_framebuffer::init_texture(const GLsizei w, const GLsizei h, const GLint internal_format, const GLenum format,
	const GLenum type, const void* data, const GLint filter)
{
	framebuffer_texture_.init(w, h, internal_format, format, type, data, filter);
}

void gl_framebuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, id_);
}
