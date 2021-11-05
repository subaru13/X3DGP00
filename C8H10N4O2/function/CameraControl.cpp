#include "CameraControl.h"
#include "KeyInput.h"
#include "..//FrameworkConfig.h"

#define SWING_WIDTH toRadian(60.0f)

CameraControl::CameraControl()
	:pos(0.0f, 0.0f, -10.0f), target(0.0f, 0.0f, 0.0f), up_vector(0.0f, 1.0f, 0.0f),
	fov(toRadian(30.0f)), width(static_cast<float>(SCREEN_WIDTH)), height(static_cast<float>(SCREEN_HEIGHT)),
	znear(0.1f), zfar(1000.0f), l_fulcrum(), r_fulcrum(), c_fulcrum(){}

FLOAT4X4 CameraControl::getView() const
{
	XMVECTOR eye{ XMVectorSet(pos.x, pos.y, pos.z, 1.0f) };
	XMVECTOR focus;
	if (pos != target)
	{
		focus = XMVectorSet(target.x, target.y, target.z, 1.0f);
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

void CameraControl::update(float elapsed_time, float move_speed, float rotation_speed)
{
	const Mouse* mouse = Mouse::instance();
	const Key& l_key = mouse->getL();
	const Key& r_key = mouse->getR();
	const Key& c_key = mouse->getC();
	const POINT& mouse_pos = mouse->getPos();
	if (l_key.down())
	{
		l_fulcrum = mouse_pos;
	}
	else if (l_key.hold())
	{
		POINT drg = POINT{ mouse_pos.x - l_fulcrum.x, mouse_pos.y - l_fulcrum.y };

		if (fabsl(drg.x) > 64L)
		{
			if (vec3Dot(VECTOR3(0, 1, 0), up_vector) < 0)drg.x *= -1;
			attitude.y += drg.x > 0 ? elapsed_time * 0.174533f * rotation_speed : -elapsed_time * 0.174533f * rotation_speed;
		}

		if (fabsl(drg.y) > 64L)
		{
			attitude.x += drg.y > 0 ? elapsed_time * 0.174533f * rotation_speed : -elapsed_time * 0.174533f * rotation_speed;
		}

		attitude.x = std::clamp(attitude.x, -SWING_WIDTH, SWING_WIDTH);

		FLOAT4X4 rotation4x4{};
		XMStoreFloat4x4(&rotation4x4, RotationMatrix(attitude));
		target = pos + vec3Normalize(VECTOR3(rotation4x4._31, rotation4x4._32, rotation4x4._33));
		up_vector = vec3Normalize(VECTOR3(rotation4x4._21, rotation4x4._22, rotation4x4._23));
	}

	if (r_key.down())
	{
		r_fulcrum = mouse_pos;
	}
	else if (r_key.hold())
	{
		POINT drg = POINT{ mouse_pos.x - r_fulcrum.x, mouse_pos.y - r_fulcrum.y };
		FLOAT4X4 rotation4x4{};
		XMStoreFloat4x4(&rotation4x4, RotationMatrix(attitude));

		VECTOR3 forward = vec3Normalize(VECTOR3(rotation4x4._31, rotation4x4._32, rotation4x4._33));
		VECTOR3 right = vec3Normalize(VECTOR3(rotation4x4._11, rotation4x4._12, rotation4x4._13));

		if (fabsl(drg.x) > 64L)
		{
			pos += drg.x > 0 ? elapsed_time * right * move_speed : -elapsed_time * right * move_speed;
		}

		if (fabsl(drg.y) > 64L)
		{
			pos += drg.y > 0 ? -elapsed_time * forward * move_speed : elapsed_time * forward * move_speed;
		}

		target = pos + forward;
	}


	if (c_key.down())
	{
		c_fulcrum = mouse_pos;
	}
	else if (c_key.hold())
	{
		POINT drg = POINT{ mouse_pos.x - c_fulcrum.x, mouse_pos.y - c_fulcrum.y };
		FLOAT4X4 rotation4x4{};
		XMStoreFloat4x4(&rotation4x4, RotationMatrix(attitude));

		VECTOR3 forward = vec3Normalize(VECTOR3(rotation4x4._31, rotation4x4._32, rotation4x4._33));
		VECTOR3 right = vec3Normalize(VECTOR3(rotation4x4._11, rotation4x4._12, rotation4x4._13));

		if (fabsl(drg.x) > 64L)
		{
			pos += drg.x > 0 ? elapsed_time * right * move_speed : -elapsed_time * right * move_speed;
		}

		if (fabsl(drg.y) > 64L)
		{
			pos += drg.y > 0 ? -elapsed_time * up_vector * move_speed : elapsed_time * up_vector * move_speed;
		}

		target = pos + forward;
	}
}
