#ifndef INCLUDE_CONSTANT_BUFFER
#define INCLUDE_CONSTANT_BUFFER

#include <d3d11.h>
#include <assert.h>
#include <wrl.h>
using namespace Microsoft::WRL;
/*********************************************************************************
	A class that manages constant buffers.
	Define the type of the variable to be registered
	in the constant buffer when declaring the entity.
	However, the byte width must be a multiple of 16.
*********************************************************************************/
template <class T>
class ConstantBuffer
{
private:
	ComPtr<ID3D11Buffer> buffer = nullptr;
public:
	/**************************************************************
		A variable registered in the constant buffer.
		Sends the data stored in this variable to the shader.
	**************************************************************/
	T data;

	/**************************************************************
		Create a constant buffer.
		Please pass the pointer of ID3D11Device.
		Also, if necessary, pass the argument of the constructor
		of the variable to be registered in the constant buffer.
	**************************************************************/
	template<class... Args>
	ConstantBuffer(ID3D11Device* device, Args... args)
		:buffer(nullptr), data(args...)
	{
		assert(device && "The device is invalid.");
		assert(sizeof(T) % 16 == 0 && "constant buffer's need to be 16 byte aligned");
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(T);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		HRESULT hr = device->CreateBuffer(&bd, NULL, buffer.ReleaseAndGetAddressOf());
		assert(SUCCEEDED(hr));
	}

	/**************************************************************
		Transfers the shader's constant buffer.
		The arguments set the destination slot number
		and type of shader to send.
	**************************************************************/
	void send(ID3D11DeviceContext* immediate_context, UINT slot,
		bool use_vs = true,
		bool use_ps = true,
		bool use_ds = true,
		bool use_hs = true,
		bool use_gs = true)
	{
		assert(immediate_context && "The context is invalid.");
		immediate_context->UpdateSubresource(buffer.Get(), 0u, nullptr, &data, 0u, 0u);
		if (use_vs)immediate_context->VSSetConstantBuffers(slot, 1U, buffer.GetAddressOf());
		if (use_ps)immediate_context->PSSetConstantBuffers(slot, 1U, buffer.GetAddressOf());
		if (use_ds)immediate_context->DSSetConstantBuffers(slot, 1U, buffer.GetAddressOf());
		if (use_hs)immediate_context->HSSetConstantBuffers(slot, 1U, buffer.GetAddressOf());
		if (use_gs)immediate_context->GSSetConstantBuffers(slot, 1U, buffer.GetAddressOf());
	}

	virtual ~ConstantBuffer() = default;
	ConstantBuffer(const ConstantBuffer&) = delete;
	ConstantBuffer& operator=(const ConstantBuffer&) = delete;

	operator T& ()
	{
		return data;
	}

	T& operator=(T& other)
	{
		data = other;
		return data;
	}
};

#endif // !INCLUDE_CONSTANT_BUFFER
