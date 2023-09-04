#include "Gallery.h"

#include "gl_texture.h"
#include "Heron.h"
#include "serialise.h"
#include "glad/glad.h"

// Simple helper function to load an image into a OpenGL texture with common settings
bool load_texture_from_preview(const cat_item item, GLuint* out_texture)
{

    // TODO hard coded 3!
    const unsigned size = item.hprev_height * item.hprev_width * 3 * sizeof(GLfloat);
    auto* image_data = static_cast<GLfloat*>(malloc(size));
    if (s_prev_read(image_data, item.hprev_location, size) < 0)
        return false;

    gl_texture texture(-1);
    texture.gen(item.hprev_width, item.hprev_height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, image_data, 9729, 1);
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
    // glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
}

void Gallery::render_panel()
{
    ImGui::Image((ImTextureID) cat_img_, ImVec2(cat_img_width_, cat_img_height_));
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    ImGui::SliderInt("Scale", &cells_per_width_, 2, 15);
	const float max_width = ImGui::GetWindowWidth();

    const auto cell_size = ImVec2((max_width - (cell_padding_ * cells_per_width_) - ImGui::GetStyle().FramePadding.y) / cells_per_width_, 
                                  (max_width - (cell_padding_ * cells_per_width_)) / cells_per_width_);
    int n = 0;
    ImVec2 pos = ImGui::GetCurrentWindow()->DC.CursorPos;
    pos.y += ImGui::GetCurrentWindow()->DC.CurrLineTextBaseOffset - ImGui::GetStyle().FramePadding.y;
    for (const auto& item : catalog_->get())
    {
        char buf[255];
        const cat_item c_item = item.second;
        float image_x_offset = 0;
        float image_y_offset = 0;

        auto prev_size = ImVec2(200, 200);
        resize_image(c_item.hprev_width, c_item.hprev_height, cell_size.x, prev_size.x, prev_size.y);

        if (c_item.hprev_width < c_item.hprev_height) {
            image_x_offset = (cell_size.x - prev_size.x) / 2;
        } else {
            image_y_offset = (cell_size.y - prev_size.y) / 2;
        }

        sprintf(buf, "##%d", item.first);

        ImU8 drop_shadow_op = 50;
        float drop_shadow_size = 25.0f;

        ImGui::PushID(n);
        ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y));
        ImVec2 offset_pos = ImVec2(pos.x + image_x_offset, pos.y + image_y_offset);
        
        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(ImRect(offset_pos, ImVec2(offset_pos.x + prev_size.x, offset_pos.y + prev_size.y)), 
                ImGui::GetCurrentWindow()->GetID(buf), &hovered, &held, ImGuiButtonFlags_MouseButtonLeft);
        if (hovered && ImGui::IsMouseClicked(0)){
            // selected_item_ = n;
            h_window_->load_item(c_item);
		}

        if (hovered) {
            drop_shadow_op = 100;
            drop_shadow_size = 75.0f;
        }

        ImGui::ItemSize(ImRect(offset_pos, ImVec2(offset_pos.x + prev_size.x, offset_pos.y + prev_size.y)));
        RenderDropShadow((ImTextureID)shadow_img_, offset_pos, prev_size, drop_shadow_size, drop_shadow_op);

        ImGui::GetWindowDrawList()->AddImageRounded((ImTextureID)catalog_textures_[item.first], offset_pos, 
            ImVec2(offset_pos.x + prev_size.x, offset_pos.y + prev_size.y), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), 10);
        
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(offset_pos.x, offset_pos.y+10), 
            ImVec2(offset_pos.x + prev_size.x / 2, offset_pos.y+20+10), ImColor(0.0,0.0,0.0,0.5f), 10.0f, ImDrawCornerFlags_Right);
        ImGui::RenderTextClipped(ImVec2(offset_pos.x + 10, offset_pos.y+10), ImVec2(offset_pos.x + prev_size.x / 2, offset_pos.y+20+10), c_item.file_name, &c_item.file_name[(strlen(c_item.file_name))], NULL);

        // char hash_buf[255];
        // sprintf(hash_buf, "%ld", item.first);
        // ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(offset_pos.x, offset_pos.y+50), 
        //     ImVec2(offset_pos.x + prev_size.x - 10, offset_pos.y+20+50), ImColor(0.0,0.0,0.0,0.5f), 10.0f, ImDrawCornerFlags_Right);
        // ImGui::RenderTextClipped(ImVec2(offset_pos.x + 10, offset_pos.y+50), ImVec2(offset_pos.x + prev_size.x - 10, offset_pos.y+20+50), hash_buf, &c_item.file_name[(strlen(c_item.file_name)-1)], NULL);


        ImGui::PopID();
        n++;
        if (pos.x + cell_size.x + cell_padding_ <= max_width - ImGui::GetStyle().FramePadding.x)
        {
            pos.x += cell_size.x + cell_padding_;
        }
        else
        {
            pos.x = cell_padding_;
            pos.y += cell_size.y + cell_padding_;
        }
    }

}
