#include "Vectorscope.h"

#include "Image.h"
#include "Widgets.h"

void Vectorscope::init()
{
}

void Vectorscope::render()
{
	ImGui::Begin(name.c_str(), &visible);
	hue_wheel(255 / 2, 255, 255, ImVec2(0, 0), 0, 30, true);
	ImGui::Image((ImTextureID)img->getVectorscopeID(), ImVec2(255, 255));
	ImGui::End();
}

void Vectorscope::cleanup()
{
}
