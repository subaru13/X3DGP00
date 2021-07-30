#ifndef INCLUDE_SIMPLE_SOUND_DEVICE
#define INCLUDE_SIMPLE_SOUND_DEVICE

#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"shlwapi.lib")
#include <wchar.h>
#include <string>
#include <mmsystem.h>
#include <shlwapi.h>
#include <thread>
#include <Windows.h>

/*********************************************************************************************
**						This is a device that just makes a simple sound.					**
**						If there is a space in the file path,								**
**						the file cannot be opened properly.									**
**						"wav" and "mp3" are supported.										**
**						All member functions return 0 on success.							**
**						If it fails, it returns anything else.								**
*********************************************************************************************/
class SimpleSoundDevice
{
public:
	static constexpr unsigned int BUFFER_SIZE = 256U;
private:
#ifdef UNICODE
	typedef std::wstring String;
	typedef wchar_t Character;
#else
	typedef std::string String;
	typedef char Character;
#endif // UNICODE
	static void assertMessage(MCIERROR errCord)
	{
		if (errCord != 0L)
		{
			Character buff[BUFFER_SIZE] = {};
			mciGetErrorString(errCord, buff, BUFFER_SIZE);
			if (MessageBox(NULL, buff, TEXT("ERROR!!"), MB_ICONHAND) == IDOK)
			{
				exit(0);
			}
		}
	}
	const String pass;
	bool is_stop;
	bool effectiveness_thread;
	HWND hwnd;
	Character buff[BUFFER_SIZE];
	static void loopThread(SimpleSoundDevice* sound)
	{
#ifdef _DEBUG
		OutputDebugString(TEXT("The thread has started.\n"));
#endif // DEBUG
		sound->effectiveness_thread = true;
		Character buff[BUFFER_SIZE] = {};
		while (true)
		{
			if (!sound->is_stop)
			{
				assertMessage(sound->getMode(buff));
				if (buff == String(TEXT("stopped")))sound->play(false);
			}
			else break;
		}
		sound->effectiveness_thread = false;
#ifdef _DEBUG
		OutputDebugString(TEXT("The thread has terminated.\n"));
#endif // DEBUG
	}

public:
	SimpleSoundDevice(String pass, HWND hwnd = NULL)
		:pass(pass), is_stop(true), effectiveness_thread(false), hwnd(hwnd), buff(TEXT(""))
	{
		assertMessage(mciSendString(String(TEXT("open ") + pass).c_str(), NULL, 0U, hwnd));
	}

	virtual ~SimpleSoundDevice()
	{
		assertMessage(stop());
		while (effectiveness_thread);
		assertMessage(mciSendString(String(TEXT("close ") + pass).c_str(), NULL, 0u, hwnd));
	}

	virtual MCIERROR play(bool loop = false)
	{
		is_stop = false;
		if (loop)
		{
			std::thread L_Thread(SimpleSoundDevice::loopThread, this);
			L_Thread.detach();
			return 0L;
		}
		return mciSendString(String(TEXT("play ") + pass + TEXT(" from 0")).c_str(), NULL, 0u, hwnd);
	}

	virtual MCIERROR stop()
	{
		is_stop = true;
		return mciSendString(String(_TEXT("stop ") + pass).c_str(), NULL, 0u, hwnd);
	}

	virtual MCIERROR resume()
	{
		assertMessage(getMode(buff));
		return (buff == String(_TEXT("paused"))) ? mciSendString(String(_TEXT("resume ") + pass).c_str(), NULL, 0u, hwnd) : 0L;
	}

	virtual MCIERROR pause()
	{
		assertMessage(getMode(buff));
		return (buff == String(TEXT("playing"))) ? mciSendString(String(TEXT("pause ") + pass).c_str(), NULL, 0u, hwnd) : 0L;
	}

	virtual MCIERROR getPosition(int* result)
	{
		Character buff[BUFFER_SIZE];
		MCIERROR errCord = mciSendString(String(TEXT("status ") + pass + TEXT(" position")).c_str(), buff, BUFFER_SIZE, hwnd);
		(*result) = StrToInt(buff);
		return errCord;
	}

	virtual MCIERROR getLength(int* result)
	{
		Character buff[BUFFER_SIZE];
		MCIERROR errCord = mciSendString(String(_TEXT("status ") + pass + _TEXT(" length")).c_str(), buff, BUFFER_SIZE, hwnd);
		(*result) = StrToInt(buff);
		return errCord;
	}

	virtual MCIERROR getMode(Character(&result)[BUFFER_SIZE])
	{
		return mciSendString(String(TEXT("status ") + pass + TEXT(" mode")).c_str(), result, BUFFER_SIZE, hwnd);
	}

	virtual MCIERROR setTimeFormatMilliseconds()
	{
		return mciSendString(String(TEXT("set ") + pass + TEXT(" time format milliseconds")).c_str(), NULL, 0u, hwnd);
	}

	SimpleSoundDevice& operator=(SimpleSoundDevice&) = default;
	SimpleSoundDevice(SimpleSoundDevice&) = default;
};

#endif // !INCLUDE_SIMPLE_SOUND_DEVICE