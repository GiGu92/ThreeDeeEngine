#pragma once

#include <array>
#include <vector>
#include "RendererCommon.h"

class ITexture;


struct MaterialTexture
{
	enum class Purpose { COLOR, NORMAL, OTHER, _COUNT, };

	ITexture* tex = nullptr;
	Purpose purpose = Purpose::COLOR;
};

class Material
{
public:
	Material(const std::array<ResId, (int)RenderPass::_COUNT>& shaders_);
	void setTexture(ShaderStage stage, unsigned int slot, ITexture* tex, MaterialTexture::Purpose purpose = MaterialTexture::Purpose::COLOR);
	void set(RenderPass render_pass);

private:
	std::array<ResId, (int)RenderPass::_COUNT> shaders;
	std::array<std::vector<MaterialTexture>, (int)ShaderStage::GRAPHICS_STAGE_COUNT> textures;
};
