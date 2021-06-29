#ifndef INCLUDE_ARITHMETIC
#define INCLUDE_ARITHMETIC

#include <math.h>
#include <random>
#include <DirectXMath.h>
#include <yvals_core.h>

namespace Constants
{
	constexpr float Epsilon = 0.000001f;
	constexpr float Pi = 3.141592654f;
}
using namespace Constants;

namespace Variables
{
	static std::random_device	rd;
#if _M_IX86
	using RandomEngine = std::mt19937;
#else
	using RandomEngine = std::mt19937_64;
#endif
	static RandomEngine			mt(rd());
}
using namespace Variables;

/****************************************************************
	Square.
****************************************************************/
_NODISCARD inline float Pow2(const float& x)
{
	return (x * x);
}

/****************************************************************
	Convert degrees to radians.
****************************************************************/
_NODISCARD inline float ToRadian(const float& x)
{
	return (x * 0.0174533f);
}

/****************************************************************
	Convert radians to degrees.
****************************************************************/
_NODISCARD inline float ToAngle(const float& x)
{
	return (x * 57.29577951308f);
}

/****************************************************************
	Normalize radians.
****************************************************************/
inline float NormalizationRadian(float& radian)
{
	while (radian > Constants::Pi) { radian -= (Constants::Pi * 2.0f); }
	while (radian < -Constants::Pi) { radian += (Constants::Pi * 2.0f); }
	return radian;
}

/****************************************************************
	Generates a specified range of random numbers.
****************************************************************/
_NODISCARD inline float Random(float min, float max)
{
	if (max < min)
	{
		float con = min;
		min = max;
		max = con;
	}
	std::uniform_real_distribution<float> rand(min, max);
	return rand(Variables::mt);
}
_NODISCARD inline int Random(int min, int max)
{
	if (max < min)
	{
		int con = min;
		min = max;
		max = con;
	}
	std::uniform_int_distribution<int> rand(min, max);
	return rand(Variables::mt);
}

/****************************************************************
	It is an 8-byte structure.
	There are float type x, y elements.
	It inherits DirectX :: XMFLOAT2 and has some operators.
****************************************************************/
typedef struct VECTOR2 :public DirectX::XMFLOAT2
{
	VECTOR2() :DirectX::XMFLOAT2(0.0f, 0.0f) {}
	VECTOR2(float x, float y) :DirectX::XMFLOAT2(x, y) {}
	VECTOR2(const VECTOR2& v) :DirectX::XMFLOAT2(v.x, v.y) {}
	_NODISCARD VECTOR2	operator+	()const { return { x, y }; }
	_NODISCARD VECTOR2	operator-	()const { return { -x, -y }; }
	_NODISCARD VECTOR2	operator+	(VECTOR2 v)const { return { x + v.x,y + v.y }; }
	_NODISCARD VECTOR2	operator-	(VECTOR2 v)const { return { x - v.x,y - v.y }; }
	_NODISCARD VECTOR2	operator*	(VECTOR2 v)const { return { x * v.x,y * v.y }; }
	_NODISCARD VECTOR2	operator*	(float s)const { return { x * s,y * s }; }
	_NODISCARD VECTOR2	operator/	(VECTOR2 v)const { return { x / v.x,y / v.y }; }
	_NODISCARD VECTOR2	operator/	(float s)const { return { x / s,y / s }; }
	VECTOR2	operator+=	(VECTOR2 v) { return *this = (*this + v); }
	VECTOR2	operator-=	(VECTOR2 v) { return *this = (*this - v); }
	VECTOR2	operator*=	(VECTOR2 v) { return *this = (*this * v); }
	VECTOR2	operator*=	(float s) { return *this = (*this * s); }
	VECTOR2	operator/=	(VECTOR2 v) { return *this = (*this / v); }
	VECTOR2	operator/=	(float s) { return *this = (*this / s); }
	_NODISCARD bool	operator==	(VECTOR2 v)const { return x == v.x && y == v.y; }
	_NODISCARD bool	operator!=	(VECTOR2 v)const { return x != v.x || y != v.y; }
}FLOAT2, float2;

/****************************************************************
	Operator with a scalar whose vector is on the left.
****************************************************************/
_NODISCARD inline VECTOR2 operator*(const float& s, const VECTOR2& v) { return { s * v.x,s * v.y }; }
_NODISCARD inline VECTOR2 operator/(const float& s, const VECTOR2& v) { return { s / v.x,s / v.y }; }

/****************************************************************
	Normalize a 2D vector
****************************************************************/
_NODISCARD inline VECTOR2 Vec2Normalization(const VECTOR2& v)
{
	float length = sqrtf((v.x * v.x) + (v.y * v.y));
	return VECTOR2(v.x / length, v.y / length);
}

/****************************************************************
	Arithmetic of inner product of 2D vectors.
****************************************************************/
_NODISCARD inline float Vec2Dot(const VECTOR2& v1, const VECTOR2& v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y));
}

/****************************************************************
	Arithmetic the cross product of 2D vectors.
****************************************************************/
_NODISCARD inline float Vec2Cross(const VECTOR2& v1, const VECTOR2& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

/****************************************************************
	Calculate the magnitude (length) of a 2D vector.
****************************************************************/
_NODISCARD inline float Vec2Length(const VECTOR2& v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y));
}

/****************************************************************
	Calculates the square of
	the magnitude (length) of a 2D vector.
****************************************************************/
_NODISCARD inline float Vec2LengthSq(const VECTOR2& v)
{
	return ((v.x * v.x) + (v.y * v.y));
}

/****************************************************************
	Determine if the 2D vector is in a vertical relationship.
****************************************************************/
_NODISCARD inline bool Vec2IsVertical(const VECTOR2& v1, const VECTOR2& v2)
{
	float d = Vec2Dot(v1, v2);
	return (-Constants::Epsilon < d&& d < Constants::Epsilon);
}

/****************************************************************
	Determine if the 2D vectors are in parallel.
****************************************************************/
_NODISCARD inline bool Vec2IsParallel(const VECTOR2& v1, const VECTOR2& v2)
{
	float d = Vec2Cross(v1, v2);
	d *= d;
	return (-Constants::Epsilon < d&& d < Constants::Epsilon);
}

/****************************************************************
	Determine if the 2D vector has a sharp relationship.
****************************************************************/
_NODISCARD inline bool Vec2IsSharp(const VECTOR2& v1, const VECTOR2& v2)
{
	return (Vec2Dot(v1, v2) >= 0.0f);
}

/****************************************************************
	It is an 12-byte structure.
	There are float type x, y, z elements.
	It inherits DirectX :: XMFLOAT3 and has some operators.
****************************************************************/
typedef struct VECTOR3 :public DirectX::XMFLOAT3
{
	VECTOR3() :DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) {}
	VECTOR3(float x, float y, float z) :DirectX::XMFLOAT3(x, y, z) {}
	VECTOR3(const VECTOR2& v, float z = 0.0f) :DirectX::XMFLOAT3(v.x, v.y, z) {}
	VECTOR3(const VECTOR3& v) :DirectX::XMFLOAT3(v.x, v.y, v.z) {}
	_NODISCARD VECTOR3	operator+	()const { return { x, y,z }; }
	_NODISCARD VECTOR3	operator-	()const { return { -x, -y,-z }; }
	_NODISCARD VECTOR3	operator+	(VECTOR3 v)const { return { x + v.x,y + v.y,z + v.z }; }
	_NODISCARD VECTOR3	operator-	(VECTOR3 v)const { return { x - v.x,y - v.y,z - v.z }; }
	_NODISCARD VECTOR3	operator*	(VECTOR3 v)const { return { x * v.x,y * v.y,z * v.z }; }
	_NODISCARD VECTOR3	operator*	(float s)const { return { x * s,y * s,z * s }; }
	_NODISCARD VECTOR3	operator/	(VECTOR3 v)const { return { x / v.x,y / v.y,z / v.z }; }
	_NODISCARD VECTOR3	operator/	(float s)const { return { x / s,y / s,z / s }; }
	VECTOR3	operator+=	(VECTOR3 v) { return *this = (*this + v); }
	VECTOR3	operator-=	(VECTOR3 v) { return *this = (*this - v); }
	VECTOR3	operator*=	(VECTOR3 v) { return *this = (*this * v); }
	VECTOR3	operator*=	(float s) { return *this = (*this * s); }
	VECTOR3	operator/=	(VECTOR3 v) { return *this = (*this / v); }
	VECTOR3	operator/=	(float s) { return *this = (*this / s); }
	_NODISCARD bool	operator==	(VECTOR3 v)const { return x == v.x && y == v.y && z == v.z; }
	_NODISCARD bool	operator!=	(VECTOR3 v)const { return x != v.x || y != v.y || z != v.z; }
} FLOAT3, float3;

/****************************************************************
	Operator with a scalar whose vector is on the left.
****************************************************************/
_NODISCARD inline VECTOR3 operator*(const float& s, const VECTOR3& v) { return { s * v.x,s * v.y,s * v.z }; }
_NODISCARD inline VECTOR3 operator/(const float& s, const VECTOR3& v) { return { s / v.x,s / v.y,s / v.z }; }

/****************************************************************
	Normalize a 3D vector
****************************************************************/
_NODISCARD inline VECTOR3 Vec3Normalization(const VECTOR3& v)
{
	float length = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return VECTOR3(v.x / length, v.y / length, v.z / length);
}

/****************************************************************
	Arithmetic of inner product of 3D vectors.
****************************************************************/
_NODISCARD inline float Vec3Dot(const VECTOR3& v1, const VECTOR3& v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

/****************************************************************
	Arithmetic the cross product of 3D vectors.
****************************************************************/
_NODISCARD inline VECTOR3 Vec3Cross(const VECTOR3& v1, const VECTOR3& v2)
{
	float x = (v1.y * v2.z) - (v1.z * v2.y);
	float y = (v1.z * v2.x) - (v1.x * v2.z);
	float z = (v1.x * v2.y) - (v1.y * v2.x);
	return VECTOR3(x, y, z);
}

/****************************************************************
	Calculate the magnitude (length) of a 3D vector.
****************************************************************/
_NODISCARD inline float Vec3Length(const VECTOR3& v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

/****************************************************************
	Calculates the square of
	the magnitude (length) of a 3D vector.
****************************************************************/
_NODISCARD inline float Vec3LengthSq(const VECTOR3& v)
{
	return ((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

/****************************************************************
	Determine if the 3D vector is in a vertical relationship.
****************************************************************/
_NODISCARD inline bool Vec3IsVertical(const VECTOR3& v1, const VECTOR3& v2)
{
	float d = Vec3Dot(v1, v2);
	return (-Constants::Epsilon < d&& d < Constants::Epsilon);
}

/****************************************************************
	Determine if the 3D vectors are in parallel.
****************************************************************/
_NODISCARD inline bool Vec3IsParallel(const VECTOR3& v1, const VECTOR3& v2)
{
	float d = Vec3LengthSq(Vec3Cross(v1, v2));
	return (-Constants::Epsilon < d&& d < Constants::Epsilon);
}

/****************************************************************
	Determine if the 3D vector has a sharp relationship.
****************************************************************/
_NODISCARD inline bool Vec3IsSharp(const VECTOR3& v1, const VECTOR3& v2)
{
	return (Vec3Dot(v1, v2) >= 0.0f);
}

typedef DirectX::XMFLOAT4 VECTOR4, FLOAT4, float4;

/****************************************************************
	Convert from color code to RGBA.
****************************************************************/
_NODISCARD inline FLOAT4 ColorCodeToRGBA(unsigned long color_code)
{
	FLOAT4 rgba{};

	rgba.x = ((color_code >> 24) & 0x000000FF) / 255.0f;
	rgba.y = ((color_code >> 16) & 0x000000FF) / 255.0f;
	rgba.z = ((color_code >> 8) & 0x000000FF) / 255.0f;
	rgba.w = ((color_code >> 0) & 0x000000FF) / 255.0f;

	return rgba;
}

typedef DirectX::XMFLOAT4X4	FLOAT4X4, float4x4;

const FLOAT4X4 __CST[]
{
	{ -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
	{ -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
};

#define CST_RHS_Y	XMLoadFloat4x4(&__CST[0])
#define CST_LHS_Y	XMLoadFloat4x4(&__CST[1])
#define CST_RHS_Z	XMLoadFloat4x4(&__CST[2])
#define CST_LHS_Z	XMLoadFloat4x4(&__CST[3])

typedef DirectX::XMMATRIX MATRIX;

#define RotationMatrix(angles)	XMMatrixRotationRollPitchYaw(angles.x,angles.y,angles.z)
#define TransformMatrix(pos)	XMMatrixTranslation(pos.x, pos.y, pos.z)
#define ScalingMatrix(scales)	XMMatrixScaling(scales.x, scales.y, scales.z)

using namespace DirectX;

#endif