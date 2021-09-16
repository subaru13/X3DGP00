#pragma once

typedef enum class __EASE_ID_ :
	unsigned char
{
	I_SINE,		/* 正弦	イン		*/
	O_SINE,		/* 正弦	アウト		*/
	IO_SINE,	/* 正弦	イン-アウト */
	I_QUAD,		/* 二次	イン		*/
	O_QUAD,		/* 二次	アウト		*/
	IO_QUAD,	/* 二次	イン-アウト */
	I_CUBIC,	/* 三次 イン		*/
	O_CUBIC,	/* 三次	アウト		*/
	IO_CUBIC,	/* 三次	イン-アウト */
	I_QUART,	/* 四次 イン		*/
	O_QUART,	/* 四次	アウト		*/
	IO_QUART,	/* 四次	イン-アウト */
	I_QUINT,	/* 五次 イン		*/
	O_QUINT,	/* 五次	アウト		*/
	IO_QUINT,	/* 五次	イン-アウト */
	I_EXPO,		/* 指数 イン		*/
	O_EXPO,		/* 指数	アウト		*/
	IO_EXPO,	/* 指数	イン-アウト */
}EASING_ID;

/// <summary>
/// xの値による緩和
/// </summary>
/// <param name="id">緩和の仕方</param>
/// <param name="x">x(0~1)</param>
/// <returns></returns>
float easing(EASING_ID id, float x);
