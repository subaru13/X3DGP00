#include "Easing.h"
#include <math.h>

#define __cos_			cosf
#define __sin_			sinf
#define __pow_			powf
#define __sqrt_			sqrtf

#define __prototype_declaration_(func)\
float _eI##func		(float);\
float _eO##func		(float);\
float _eIO##func	(float);

#define __find_(id)	static_cast<unsigned char>(id)

//****************************************************************************

__prototype_declaration_(Sine);
__prototype_declaration_(Quad);
__prototype_declaration_(Cubic);
__prototype_declaration_(Quart);
__prototype_declaration_(Quint);
__prototype_declaration_(Expo);

//****************************************************************************

using EaseFunc	= float(*)(float);

static const EaseFunc ease_map[] =
{
	_eISine		, _eOSine	, _eIOSine	,
	_eIQuad		, _eOQuad	, _eIOQuad	,
	_eICubic	, _eOCubic	, _eIOCubic	,
	_eIQuart	, _eOQuart	, _eIOQuart	,
	_eIQuint	, _eOQuint	, _eIOQuint	,
	_eIExpo		, _eOExpo	, _eIOExpo	,
};

float easing(EASING_ID id, float x)
{
	return (*ease_map[__find_(id)])(x);
}

//****************************************************************************

float _eISine(float x)
{
	return 1.0f - __cos_((x * 3.14f) / 2.0f);
}

float _eOSine(float x)
{
	return __sin_((x * 3.14f) / 2.0f);
}

float _eIOSine(float x)
{
	return -(__cos_(x * 3.14f) - 1.0f) / 2.0f;
}

float _eIQuad(float x)
{
	return __pow_(x, 2.0f);
}

float _eOQuad(float x)
{
	return 1.0f - __pow_(1.0f - x, 2.0f);
}

float _eIOQuad(float x)
{
	return x < 0.5f ?
		2.0f * __pow_(x, 2.0f) :
		1.0f - __pow_(-2.0f * x + 2.0f, 2.0f) / 2.0f;
}

float _eICubic(float x)
{
	return __pow_(x, 3.0f);
}

float _eOCubic(float x)
{
	return 1.0f - __pow_(1.0f - x, 3.0f);
}

float _eIOCubic(float x)
{
	return x < 0.5f ?
		4.0f * __pow_(x, 3.0f) :
		1.0f - __pow_(-2.0f * x + 2.0f, 3.0f) / 2.0f;
}

float _eIQuart(float x)
{
	return __pow_(x, 4.0f);
}

float _eOQuart(float x)
{
	return 1.0f - __pow_(1.0f - x, 4.0f);
}

float _eIOQuart(float x)
{
	return x < 0.5f ?
		8.0f * __pow_(x, 4.0f) :
		1.0f - __pow_(-2.0f * x + 2.0f, 4.0f) / 2.0f;
}

float _eIQuint(float x)
{
	return __pow_(x, 5.0f);
}

float _eOQuint(float x)
{
	return 1.0f - __pow_(1.0f - x, 5.0f);
}

float _eIOQuint(float x)
{
	return x < 0.5f ?
		16.0f * __pow_(x, 5.0f) :
		1.0f - __pow_(-2.0f * x + 2.0f, 5.0f) / 2.0f;
}

float _eIExpo(float x)
{
	return x == 0.0f ? 0.0f : __pow_(2.0f, 10.0f * x - 10.0f);
}

float _eOExpo(float x)
{
	return x == 1.0f ? 1.0f : 1.0f - __pow_(2.0f, -10.0f * x);
}

float _eIOExpo(float x)
{
	return x == 0.0f ? 0.0f :
		x == 1.0f ? 1.0f :
		x < 0.5f ? __pow_(2.0f, 20.0f * x - 10.0f) / 2.0f :
		(2.0f - __pow_(2.0f, -20.0f * x + 10.0f)) / 2.0f;
}

#undef __cos_
#undef __sin_
#undef __pow_
#undef __sqrt_
#undef __prototype_declaration_
#undef __find_