#pragma once
#include "glad/glad.h"
#include <string>

class gl_quad
{
public:
	void gen();
	void draw() const;
	void cleanup();

private:
	GLuint vao_{};
	GLuint vbo_{};
	GLuint ebo_{};
};

class gl_buffer
{
public:
	virtual ~gl_buffer() = default;

	virtual void gen(GLsizeiptr buffer_size, GLenum usage = GL_STREAM_READ, const void* data = nullptr);
	void clear(GLenum internal_format, GLenum format, GLenum type) const;
	void get_data(void* data) const;
	void bind() const { glBindBuffer(target_, id_); }
	GLsizeiptr get_buffer_size() const { return buffer_size_; }
	GLuint get_id() const { return id_; }

protected:
	explicit gl_buffer(const GLenum target) : target_(target) {}

protected:
	GLuint id_{};
	GLsizeiptr buffer_size_ = 0;
	GLenum target_;
};

class gl_pbo final : public gl_buffer
{
public:
	gl_pbo() : gl_buffer(GL_PIXEL_PACK_BUFFER) {}
};

class gl_ssbo final : public gl_buffer
{
public:
	explicit gl_ssbo(const int binding) :
		gl_buffer(GL_SHADER_STORAGE_BUFFER),
		binding_(binding){}

	void gen(GLsizeiptr buffer_size, GLenum usage = GL_STREAM_READ, const void* data = nullptr) override;

private:
	const int binding_;
};


class gl_texture
{
public:
	virtual ~gl_texture() = default;
	explicit gl_texture(const int binding) : binding_(binding){}

	virtual void gen(GLsizei w, GLsizei h, GLint internal_format, GLenum format, GLenum type, const void* data, GLint filter = GL_LINEAR);

	virtual void init(GLsizei w, GLsizei h, GLint internal_format, GLenum format, GLenum type, const void* data, GLint filter = GL_LINEAR);

	virtual void bind() const;

	GLuint get_id() const { return id_; }
protected:
	GLuint id_{};
	const int binding_;
};


class gl_image final : public gl_texture
{
public:
	explicit gl_image(const int binding) : gl_texture(binding) {}

	void init(GLsizei w, GLsizei h, GLint internal_format, GLenum format, GLenum type, const void* data, GLint filter = GL_LINEAR) override;

	void get_data_via_pbo(const gl_pbo* p_pbo, void* dst) const;
};

class gl_framebuffer_texture final : public gl_texture
{
public:
	explicit gl_framebuffer_texture(const int binding) : gl_texture(binding) {}

	void init(GLsizei w, GLsizei h, GLint internal_format, GLenum format, GLenum type, const void* data, GLint filter = GL_LINEAR) override;
	void bind() const override {}
};

class gl_framebuffer
{
public:
	virtual ~gl_framebuffer() = default;
	explicit gl_framebuffer() : framebuffer_texture_(0) {}

	void gen(GLsizei w, GLsizei h, GLint internal_format, GLenum format, GLenum type, const void* data, GLint filter = GL_LINEAR);

	void init_texture(GLsizei w, GLsizei h, GLint internal_format, GLenum format, GLenum type, const void* data, GLint filter = GL_LINEAR);

	GLuint get_texture_id() const { return framebuffer_texture_.get_id(); }

	void bind() const;

private:
	void init(GLsizei w, GLsizei h, GLint internal_format, GLenum format, GLenum type, const void* data, GLint filter = GL_LINEAR);

private:
	GLuint id_{};
	gl_framebuffer_texture framebuffer_texture_;
};