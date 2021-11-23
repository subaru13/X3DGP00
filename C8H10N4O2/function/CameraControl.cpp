#include "CameraControl.h"
#include "KeyInput.h"
#include "..//FrameworkConfig.h"

#define SWING_WIDTH toRadian(60.0f)

CameraControl::CameraControl()
	:pos(0.0f, 0.0f, -10.0f), target(0.0f, 0.0f, 0.0f), up_vector(0.0f, 1.0f, 0.0f),
	fov(toRadian(30.0f)), width(static_cast<float>(SCREEN_WIDTH)), height(static_cast<float>(SCREEN_HEIGHT)),
	znear(0.1f), zfar(1000.0f), l_fulcrum(), r_fulcrum(), c_fulcrum(){}

void CameraControl::fit()
{
	FLOAT4X4 view{}, matrix{};
	view = getView();
	for (size_t y = 0; y < 3; ++y)
	{
		for (size_t x = 0; x < 3; ++x)
		{
			matrix.m[y][x] = view.m[x][y];
		}
	}

	XMVECTOR q = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&matrix));
	float x = DirectX::XMVectorGetX(q);
	float y = DirectX::XMVectorGetY(q);
	float z = DirectX::XMVectorGetZ(q);
	float w = DirectX::XMVectorGetW(q);

	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;

	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;

	// 1 - 2y^2 - 2z^2
	float m00 = 1.0f - (2.0f * y2) - (2.0f * z2);

	// 2xy + 2wz
	float m01 = (2.0f * xy) + (2.0f * wz);

	// 2xy - 2wz
	float m10 = (2.0f * xy) - (2.0f * wz);

	// 1 - 2x^2 - 2z^2
	float m11 = 1.0f - (2.0f * x2) - (2.0f * z2);

	// 2xz + 2wy
	float m20 = (2.0f * xz) + (2.0f * wy);

	// 2yz+2wx
	float m21 = (2.0f * yz) - (2.0f * wx);

	// 1 - 2x^2 - 2y^2
	float m22 = 1.0f - (2.0f * x2) - (2.0f * y2);

	if (nearlyEqual(m21, 1.0f))
	{
		attitude.x = __pi_ / 2.0f;
		attitude.y = 0;
	}
	else if (nearlyEqual(m21, -1.0f))
	{
		attitude.x = -__pi_ / 2.0f;
		attitude.y = 0;
	}
	else
	{
		attitude.x = std::asin(-m21);
		attitude.y = std::atan2(m20, m22);
	}
}

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
