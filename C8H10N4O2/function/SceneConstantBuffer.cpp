#include "SceneConstantBuffer.h"

SceneConstant::SceneConstant(ID3D11Device* device)
	:constant_buffer(device), view(), projection()
{
	XMVECTOR eye{ XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f) };
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