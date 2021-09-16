#pragma once

typedef enum class __EASE_ID_ :
	unsigned char
{
	I_SINE,		/* ����	�C��		*/
	O_SINE,		/* ����	�A�E�g		*/
	IO_SINE,	/* ����	�C��-�A�E�g */
	I_QUAD,		/* ��	�C��		*/
	O_QUAD,		/* ��	�A�E�g		*/
	IO_QUAD,	/* ��	�C��-�A�E�g */
	I_CUBIC,	/* �O�� �C��		*/
	O_CUBIC,	/* �O��	�A�E�g		*/
	IO_CUBIC,	/* �O��	�C��-�A�E�g */
	I_QUART,	/* �l�� �C��		*/
	O_QUART,	/* �l��	�A�E�g		*/
	IO_QUART,	/* �l��	�C��-�A�E�g */
	I_QUINT,	/* �܎� �C��		*/
	O_QUINT,	/* �܎�	�A�E�g		*/
	IO_QUINT,	/* �܎�	�C��-�A�E�g */
	I_EXPO,		/* �w�� �C��		*/
	O_EXPO,		/* �w��	�A�E�g		*/
	IO_EXPO,	/* �w��	�C��-�A�E�g */
}EASING_ID;

/// <summary>
/// x�̒l�ɂ��ɘa
/// </summary>
/// <param name="id">�ɘa�̎d��</param>
/// <param name="x">x(0~1)</param>
/// <returns></returns>
float easing(EASING_ID id, float x);
