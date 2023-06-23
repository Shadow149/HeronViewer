#include "FileDialog.h"
#include "../Heron.h"

std::string escaped(const std::string& input)
{
    std::string output;
    output.reserve(input.size());
    for (const char c : input) {
        switch (c) {
        case '\a':  output += "\\a";        break;
        case '\b':  output += "\\b";        break;
        case '\f':  output += "\\f";        break;
        case '\n':  output += "\\n";        break;
        case '\r':  output += "\\r";        break;
        case '\t':  output += "\\t";        break;
        case '\v':  output += "\\v";        break;
        case '\\':  output += "\\\\";       break;
        default:    output += c;            break;
        }
    }

    return output;
}

void FileDialog::init()
{
	/*
	// Create thumbnails texture
	ImGuiFileDialog::Instance()->SetCreateThumbnailCallback([](IGFD_Thumbnail_Info* vThumbnail_Info) -> void{
		if (vThumbnail_Info &&
			vThumbnail_Info->isReadyToUpload &&
			vThumbnail_Info->textureFileDatas)
		{
			GLuint textureId;
			glGenTextures(1, &textureId);
			vThumbnail_Info->textureID = (void*)textureId;

			glBindTexture(GL_TEXTURE_2D, textureId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
				(GLsizei)vThumbnail_Info->textureWidth, (GLsizei)vThumbnail_Info->textureHeight,
				0, GL_RGBA, GL_UNSIGNED_BYTE, vThumbnail_Info->textureFileDatas);
			glFinish();
			glBindTexture(GL_TEXTURE_2D, 0);

			delete[] vThumbnail_Info->textureFileDatas;
			vThumbnail_Info->textureFileDatas = nullptr;

			vThumbnail_Info->isReadyToUpload = false;
			vThumbnail_Info->isReadyToDisplay = true;
		}
	});

	// TODO fix this lag
	// Destroy thumbnails texture
	ImGuiFileDialog::Instance()->SetDestroyThumbnailCallback([](IGFD_Thumbnail_Info* vThumbnail_Info){
		if (vThumbnail_Info)
		{
			GLuint texID = (GLuint)vThumbnail_Info->textureID;
			glDeleteTextures(1, &texID);
			glFinish();
		}
	});
	*/
}

void FileDialog::setStyle()
{
	ImGui::GetStyle().WindowMinSize = ImVec2(400, 200);
}


void FileDialog::render() {
	if (ImGuiFileDialog::Instance()->Display(name))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
            escape = escaped(filePathName);
			Console::log("Opening: " + escape);
			hWindow->setImagePath(filePath);
			hWindow->loadImage(escape, fileName);
		}
		ImGuiFileDialog::Instance()->Close();
	}
}



void FileDialog::cleanup()
{
}
