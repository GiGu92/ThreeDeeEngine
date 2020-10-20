#include "Buffer.h"

#include <assert.h>

using namespace drv_d3d11;

Buffer::Buffer(const BufferDesc& desc_) : desc(desc_)
{
	HRESULT hr;

	D3D11_BUFFER_DESC bd{};
	bd.ByteWidth = desc.elementByteSize * desc.numElements;
	bd.Usage = (D3D11_USAGE)desc.usageFlags;
	bd.BindFlags = desc.bindFlags;
	bd.CPUAccessFlags = desc.cpuAccessFlags;
	bd.MiscFlags = desc.miscFlags;
	bd.StructureByteStride = desc.elementByteSize;
	hr = get_drv()->getDevice()->CreateBuffer(&bd, nullptr, &resource);
	assert(SUCCEEDED(hr));

	createViews();
	
	id = get_drv()->registerBuffer(this);
}

Buffer::~Buffer()
{
	get_drv()->unregisterBuffer(id);
}

void Buffer::updateData(const void* src_data)
{
	get_drv()->getContext()->UpdateSubresource(resource.Get(), 0, nullptr, src_data, 0, 0);
	// TODO: use Map for dynamic buffers
}

void Buffer::createViews()
{
	if (desc.bindFlags & BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = desc.numElements;
		get_drv()->getDevice()->CreateShaderResourceView(resource.Get(), &srvDesc, &srv);
	}
	if (desc.bindFlags & BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = desc.numElements;
		get_drv()->getDevice()->CreateUnorderedAccessView(resource.Get(), &uavDesc, &uav);
	}
}
