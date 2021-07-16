#include "camera_control.h"
#include "..//FrameworkConfig.h"

CameraControl::CameraControl()
	:pos(0.0f, 0.0f, -10.0f), traget(0.0f, 0.0f, 0.0f), up_vector(0.0f, 1.0f, 0.0f),
	fov(ToRadian(30.0f)),width(static_cast<float>(SCREEN_WIDTH)), height(static_cast<float>(SCREEN_HEIGHT)),
	znear(0.1f), zfar(1000.0f){}

FLOAT4X4 CameraControl::getView() const
{
	XMVECTOR eye{ XMVectorSet(pos.x, pos.y, pos.z, 1.0f) };
	XMVECTOR focus;
	if (pos != traget)
	{
		focus = XMVectorSet(traget.x, traget.y, traget.z, 1.0f);
	}
	else
	{
		focus = XMVectorSet(pos.x, pos.y, pos.z + 1.0f, 1.0f);
	}
	XMVECTOR up{ XMVectorSet(up_vector.x,up_vector.y, up_vector.z, 0.0f) };
	XMMATRIX v{ XMMatrixLookAtLH(eye, focus, up) };
	FLOAT4X4 view{};
	XMStoreFloat4x4(&view, v);
	return view;
}

FLOAT4X4 CameraControl::getProjection() const
{
	FLOAT4X4 projection{};
	XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(fov, width / height, znear, zfar));
	return projection;
}

FLOAT4X4 CameraControl::getOrthographic() const
{
	FLOAT4X4 orthographic{};
	XMStoreFloat4x4(&orthographic, XMMatrixOrthographicLH(width, height, znear, zfar));
	return orthographic;
}
