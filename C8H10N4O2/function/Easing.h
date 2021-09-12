#pragma once

typedef enum class __EASE_ID_ :
	unsigned char
{
	I_SINE,		O_SINE,		IO_SINE,
	I_QUAD,		O_QUAD,		IO_QUAD,
	I_CUBIC,	O_CUBIC,	IO_CUBIC,
	I_QUART,	O_QUART,	IO_QUART,
	I_QUINT,	O_QUINT,	IO_QUINT,
	I_EXPO,		O_EXPO,		IO_EXPO,
}EASING_ID;

float easing(EASING_ID id, float x);
