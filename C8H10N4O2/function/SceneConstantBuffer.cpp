#include "SceneConstantBuffer.h"

SceneConstant::SceneConstant(ID3D11Device* device)
	:constant_buffer(device), view(), projection()
{
	XMVECTOR eye{ XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f) };
	XMVECTOR focus{ XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
	XMVECTOR up{ XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
	XMMATRIX v{ XMMatrixLookAtLH(eye, focus, up) };
	XMStoreFloat4x4(&view, v);
	XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(toRadian(30.0f), 1.77f, 0.1f, 1000.0f));
	constant_buffer.data.light_direction = FLOAT4(-1, -1, 1, 0);
	constant_buffer.data.camera_position = FLOAT4(0, 0, -10.0f, 0);
}

void SceneConstant::send(ID3D11DeviceContext* immediate_context)
{
	FLOAT4X4* view_projection = &constant_buffer.data.view_projection;
	XMStoreFloat4x4(view_projection, XMLoadFloat4x4(&view) * XMLoadFloat4x4(&projection));
	constant_buffer.send(immediate_context, 7, true, true);
}

inline void View360Degree::setPosition(const FLOAT3& position)
{
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR t{};
	DirectX::XMVECTOR up{};

	FLOAT3 target_f3[6] =
	{
		/* +x , -x */{1.0f,0.0f,0.0f}, {-1.0f,0.0f,0.0f},
		/* +y , -y */{0.0f,1.0f,0.0f}, {0.0f,-1.0f,0.0f},
		/* +z , -z */{0.0f,0.0f,1.0f}, {0.0f,0.0f,-1.0f},
	};

	FLOAT3 up_f3[6] =
	{
		{0.0f,1.0f,0.0f}, {0.0f,1.0f,0.0f},
		{0.0f,0.0f,-1.0f}, {0.0f,0.0f,-1.0f},
		{0.0f,1.0f,0.0f}, {0.0f,1.0f,0.0f},
	};

	for (size_t i = 0; i < 6; ++i)
	{
		t = DirectX::XMLoadFloat3(&(position + target_f3[i]));
		up = DirectX::XMLoadFloat3(&up_f3[i]);

		DirectX::XMStoreFloat4x4(&view_matrices[i],
			DirectX::XMMatrixLookAtLH(p, t, up));
	}
}
