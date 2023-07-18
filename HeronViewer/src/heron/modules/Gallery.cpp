#include "Gallery.h"

#include "gl_texture.h"
#include "Heron.h"
#include "serialise.h"
#include "glad/glad.h"
#include "Utils.h"

// Simple helper function to load an image into a OpenGL texture with common settings
bool load_texture_from_preview(const cat_item item, GLuint* out_texture)
{

    // TODO hard coded 3!
    const unsigned size = item.hprev_height * item.hprev_width * 3 * sizeof(GLfloat);
    auto* image_data = static_cast<GLfloat*>(malloc(size));
    if (s_prev_read(image_data, item.hprev_location, size) < 0)
        return false;

    gl_texture texture(-1);
    // FOR COOL GLITCHY EFFECT: GL_RGB, GL_RGB, GL_UNSIGNED_BYTE
    texture.gen(item.hprev_width, item.hprev_height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    free(image_data);

    *out_texture = texture.get_id();
    return true;
}

void Gallery::init_panel()
{
    for (const auto& item : catalog_->get())
    {
        GLuint id;
        if (!load_texture_from_preview(item.second, &id)) continue;
        catalog_textures_[item.first] = id;
    }
}

void Gallery::on_focus()
{
    if (h_window_->write_image_caches()) // TODO do smart texture update
        init_panel();
}

void Gallery::render_panel()
{

    ImGui::SliderInt("Scale", &cells_per_width_, 2, 15);
	const float max_width = ImGui::GetWindowWidth();
    const auto cell_size = ImVec2(max_width / cells_per_width_, max_width / cells_per_width_);
    int n = 0;
    ImVec2 pos = ImGui::GetCursorPos();
    for (const auto& item : catalog_->get())
    {
        char buf[255];
        const cat_item c_item = item.second;

        auto prev_size = ImVec2(200, 200);
        resize_image(c_item.hprev_width, c_item.hprev_height, cell_size.x, prev_size.x, prev_size.y);

        sprintf_s(buf, "##%d", item.first);

        ImGui::PushID(n);
        ImGui::SetCursorPos(ImVec2(pos.x, pos.y));
        if (ImGui::Selectable(buf, selected_item_ == n, 0, cell_size)){
            selected_item_ = n;
            h_window_->load_item(c_item);
		}
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(pos.x, pos.y));
        ImGui::Image((ImTextureID)catalog_textures_[item.first], prev_size);
        ImGui::PopID();
        n++;
        if (pos.x + cell_size.x + 20 < max_width)
        {
            ImGui::SameLine();
            pos.x += cell_size.x + 20;
        }
        else
        {
            pos.x = 20;
            pos.y += cell_size.y + 20;
        }
    }

}
