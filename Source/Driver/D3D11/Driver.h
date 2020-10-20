#pragma once

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

#include <memory>
#include <map>
#include <wrl/client.h>

#include <Driver/IDriver.h>

namespace drv_d3d11
{
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	class Texture;
	class Buffer;
	class RenderState;
	class ShaderSet;
	class InputLayout;

	class Driver : public IDriver
	{
	public:
		static Driver* get();

		bool init(void* hwnd, int display_width, int display_height) override;
		void shutdown() override;
		void resize(int display_width, int display_height) override;

		ITexture* createTexture(const TextureDesc& desc) override;
		IBuffer* createBuffer(const BufferDesc& desc) override;
		ResId createRenderState(const RenderStateDesc& desc) override;
		ResId createShaderSet(const ShaderSetDesc& desc) override;
		ResId createInputLayout(const InputLayoutElementDesc* descs, unsigned int num_descs, ResId shader_set) override;

		void setIndexBuffer(ResId* res_id) override;
		void setVertexBuffer(ResId* res_id) override;
		void setConstantBuffer(ShaderStage stage, unsigned int slot, ResId* res_id) override;
		void setBuffer(ShaderStage stage, unsigned int slot, ResId* res_id) override;
		void setRwBuffer(unsigned int slot, ResId* res_id) override;
		void setTexture(ShaderStage stage, unsigned int slot, ResId* res_id) override;
		void setRwTexture(unsigned int slot, ResId* res_id) override;
		void setRenderTarget(ResId* target_id, ResId* depth_id) override;
		void setRenderTargets(unsigned int num_targets, ResId** target_ids, ResId* depth_id) override;
		void setRenderState(ResId* res_id) override;

		const DriverSettings& getSettings() override { return settings; };
		void setSettings(const DriverSettings& new_settings) override;

		ID3D11Device* getDevice() { return device.Get(); }
		ID3D11DeviceContext* getContext() { return context.Get(); }

		ResId registerTexture(Texture* tex);
		void unregisterTexture(ResId id);
		ResId registerBuffer(Buffer* buf);
		void unregisterBuffer(ResId id);
		ResId registerRenderState(RenderState* rs);
		void unregisterRenderState(ResId id);
		ResId registerShaderSet(ShaderSet* shader_set);
		void unregisterShaderSet(ResId id);
		ResId registerInputLayout(InputLayout* input_layout);
		void unregisterInputLayout(ResId id);

	private:
		bool initResolutionDependentResources(int display_width, int display_height);
		void closeResolutionDependentResources();

		DriverSettings settings;

		HWND hWnd;

		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;
		ComPtr<IDXGISwapChain> swapchain;
		std::unique_ptr<Texture> backbuffer;

		std::map<ResId, Texture*> textures;
		std::map<ResId, Buffer*> buffers;
		std::map<ResId, RenderState*> renderStates;
		std::map<ResId, ShaderSet*> shaders;
		std::map<ResId, InputLayout*> inputLayouts;

		ResId defaultRenderState;
	};
}