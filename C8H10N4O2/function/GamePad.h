#pragma once
#include "Arithmetic.h"
#include <Windows.h>
#include <Xinput.h>

#pragma comment(lib,"xinput.lib")

class Pad final
{
private:
	bool			connection_flag;
	WORD			now;
	WORD			last;
	float			activity_time;
	VECTOR2			lstick;
	VECTOR2			rstick;
	Pad() :connection_flag(false), now(), last(), activity_time(0.0f), lstick(), rstick(){}
	WORD _shift(UCHAR transition)const { return (1 << transition); }
	WORD _trg()const { return now & (~last); }
	WORD _rel()const { return (~now) & last; }
	float _clamp01(float v)const { if (v > 1.0f)return 1.0f; if (v < -1.0f)return -1.0f; return v; }
public:

	static constexpr float __UNIT_STICK_ = 32767.0f;

	enum __LABEL_:UCHAR
	{
		PB_UP		= 0,
		PB_DOUN		= 1,
		PB_LEFT		= 2,
		PB_RIGHT	= 3,
		PB_START	= 4,
		PB_BACK		= 5,
		PB_L_STIC	= 6,
		PB_R_STIC	= 7,
		PB_L1		= 8,
		PB_R1		= 9,
		PB_L2		= 10,
		PB_R2		= 11,
		PB_A		= 12,
		PB_B		= 13,
		PB_X		= 14,
		PB_Y		= 15,
	};

	static Pad* instance()
	{
		static Pad ins;
		return &ins;
	}

	void update(float elapsed_time)
	{
		XINPUT_STATE state;
		if (DWORD dresult = XInputGetState(0, &state); dresult == ERROR_SUCCESS)
		{
			connection_flag = true;

			if ((state.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
				(state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
			{
				state.Gamepad.sThumbLX = 0;
			}
			if ((state.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
				(state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
			{
				state.Gamepad.sThumbLY = 0;
			}
			if ((state.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
				(state.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
			{
				state.Gamepad.sThumbRX = 0;
			}
			if ((state.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
				(state.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
			{
				state.Gamepad.sThumbRY = 0;
			}

			if (activity_time > 0)
			{
				activity_time -= elapsed_time;
			}
			else
			{
				vibration(-1.0f, 0.0f);
			}
			last = now;
			now = state.Gamepad.wButtons;

			if (state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				now |= _shift(__LABEL_::PB_L2);
			}
			if (state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				now |= _shift(__LABEL_::PB_R2);
			}

			lstick.x = state.Gamepad.sThumbLX / __UNIT_STICK_;
			lstick.y = state.Gamepad.sThumbLY / __UNIT_STICK_;
			rstick.x = state.Gamepad.sThumbRX / __UNIT_STICK_;
			rstick.y = state.Gamepad.sThumbRY / __UNIT_STICK_;
		}
		else if (dresult == ERROR_DEVICE_NOT_CONNECTED)
		{
			connection_flag = false;
			last = now = 0;
			lstick.x = 0.0f;
			lstick.y = 0.0f;
			rstick.x = 0.0f;
			rstick.y = 0.0f;
			activity_time = 0.0f;
		}
		else
		{
			LPWSTR msg{ 0 };
			FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
				NULL, GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				reinterpret_cast<LPWSTR>(&msg),NULL,NULL);
			OutputDebugStringW(msg);
		}
	}

	const bool& isConnection()const { return connection_flag; }
	bool isVibration()const { return activity_time > 0; }

	bool down(UCHAR key)const	{ return _trg() & _shift(key); }
	bool hold(UCHAR key)const	{ return now & _shift(key); }
	bool up(UCHAR key)const		{ return _rel() & _shift(key); }

	const VECTOR2& getRS()const { return rstick; }
	const VECTOR2& getLS()const { return lstick; }

	void vibration(float timer, float power = 0.5f)
	{
		activity_time = timer;
		WORD _power = (WORD)(65535 * power);
		XINPUT_VIBRATION speed{ _power ,_power };
		XInputSetState(0, &speed);
	}
};

using PAD_LABEL = Pad::__LABEL_;