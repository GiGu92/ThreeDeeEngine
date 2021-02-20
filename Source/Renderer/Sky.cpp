#include "Sky.h"

#include <3rdParty/imgui/imgui.h>
#include <Util/AutoImGui.h>
#include <Driver/IBuffer.h>
#include "WorldRenderer.h"

namespace ImGui
{
	bool ColorEdit3Srgb(const char* label, float col[3], ImGuiColorEditFlags flags = 0)
	{
		for (int i = 0; i < 3; i++)
			col[i] = powf(col[i], 1.0f/2.2f);
		bool result = ColorEdit3(label, col, flags);
		for (int i = 0; i < 3; i++)
			col[i] = powf(col[i], 2.2f);
		return result;
	}
}

Sky::Sky()
{
	ShaderSetDesc skyShaderDesc("Sky", "Source/Shaders/Sky.shader");
	skyShaderDesc.shaderFuncNames[(int)ShaderStage::VS] = "DefaultPostFxVsFunc";
	skyShaderDesc.shaderFuncNames[(int)ShaderStage::PS] = "SkyPS";
	skyShader = drv->createShaderSet(skyShaderDesc);

	RenderStateDesc skyRsDesc;
	skyRsDesc.depthStencilDesc.depthWrite = false;
	skyRenderState = drv->createRenderState(skyRsDesc);

	BufferDesc cbDesc;
	cbDesc.bindFlags = BIND_CONSTANT_BUFFER;
	cbDesc.numElements = 1;
	cbDesc.name = "SkyCb";
	cbDesc.elementByteSize = sizeof(cbData);
	cb.reset(drv->createBuffer(cbDesc));
	cb->updateData(&cbData);

	setWorldRendererAmbientLighting();
}

void Sky::render()
{
	PROFILE_SCOPE("Sky");
	drv->setShader(skyShader, 0);
	drv->setRenderState(skyRenderState);
	drv->setConstantBuffer(ShaderStage::PS, 3, cb->getId());
	drv->draw(3, 0);
}

void Sky::gui()
{
	bool changed = false;

	changed |= ImGui::ColorEdit3Srgb("Top color", &cbData.topColor_Exponent.x);
	changed |= ImGui::DragFloat("Top exponent", &cbData.topColor_Exponent.w);
	changed |= ImGui::ColorEdit3Srgb("Horizon color", &cbData.horizonColor.x);
	changed |= ImGui::ColorEdit3Srgb("Bottom color", &cbData.bottomColor_Exponent.x);
	changed |= ImGui::DragFloat("Bottom exponent", &cbData.bottomColor_Exponent.w);
	changed |= ImGui::SliderFloat("Sky intensity", &cbData.skyIntensity_SunIntensity_SunAlpha_SunBeta.x, 0.f, 2.f);
	changed |= ImGui::SliderFloat("Sun intensity", &cbData.skyIntensity_SunIntensity_SunAlpha_SunBeta.y, 0.f, 5.f);
	changed |= ImGui::DragFloat("Sun alpha", &cbData.skyIntensity_SunIntensity_SunAlpha_SunBeta.z);
	changed |= ImGui::DragFloat("Sun beta", &cbData.skyIntensity_SunIntensity_SunAlpha_SunBeta.w);

	static bool applyToAmbientLighting = true;
	changed |= ImGui::Checkbox("Apply changes to ambient lighting", &applyToAmbientLighting);

	if (changed)
	{
		cb->updateData(&cbData);
		if (applyToAmbientLighting)
			setWorldRendererAmbientLighting();
	}
}

void Sky::setWorldRendererAmbientLighting()
{
	XMFLOAT4 bottomColor = cbData.bottomColor_Exponent;
	bottomColor.w = 1.f;
	XMFLOAT4 topColor = cbData.topColor_Exponent;
	topColor.w = 1.f;
	float skyIntensity = cbData.skyIntensity_SunIntensity_SunAlpha_SunBeta.x;
	wr->setAmbientLighting(bottomColor, topColor, skyIntensity);
}

REGISTER_IMGUI_WINDOW("Sky parameters", [] { wr->getSky().gui(); });