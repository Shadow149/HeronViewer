#include "Preview.h"

#include "Image.h"
#include "Utils.h"
#include "Widgets.h"

void Preview::render()
{
	ImGui::Begin(name.c_str());
	ImGui::Image((ImTextureID)p_img_->get_image_id(), get_resize_size(p_img_->get_width(), p_img_->get_height()));
	ImGui::End();
}
