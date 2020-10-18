#pragma once

#include "DriverCommon.h"

class ITexture;
class IBuffer;

class IDriver
{
public:
	virtual bool init(void* hwnd, int display_width, int display_height) = 0;
	virtual void shutdown() = 0;
	virtual void resize(int display_width, int display_height) = 0;

	virtual ITexture* createTexture(const TextureDesc& desc) = 0;
	virtual IBuffer* createBuffer(const BufferDesc& desc) = 0;
	virtual ResId createRenderState(const RenderStateDesc& desc) = 0;

	virtual void setIndexBuffer(ResId* res_id) = 0;
	virtual void setVertexBuffer(ResId* res_id) = 0;
	virtual void setConstantBuffer(ShaderStage stage, unsigned int slot, ResId* res_id) = 0;
	virtual void setBuffer(ShaderStage stage, unsigned int slot, ResId* res_id) = 0;
	virtual void setRwBuffer(unsigned int slot, ResId* res_id) = 0;
	virtual void setTexture(ShaderStage stage, unsigned int slot, ResId* res_id) = 0;
	virtual void setRwTexture(unsigned int slot, ResId* res_id) = 0;
	virtual void setRenderTarget(ResId* target_id, ResId* depth_id) = 0;
	virtual void setRenderTargets(unsigned int num_targets, ResId** target_ids, ResId* depth_id) = 0;
	virtual void setRenderState(ResId* id) = 0;

	virtual const DriverSettings& getSettings() = 0;
	virtual void setSettings(const DriverSettings& new_settings) = 0;
};