#include "Preview.h"

#include "Image.h"
#include "Widgets.h"

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


void Preview::render()
{
	ImGui::Begin(name.c_str(), &visible);
	ImGui::Image((ImTextureID)p_img_->get_image_id(), get_resize_size(p_img_->get_width(), p_img_->get_height()));
	ImGui::End();
}
