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
	static constexpr unsigned int BuffSize = 256U;
private:
#ifdef UNICODE
	typedef std::wstring String;
	typedef wchar_t Character;
#else
	typedef std::string String;
	typedef char Character;
#endif // UNICODE
	static void AssertMessage(MCIERROR errCord)
	{
		if (errCord != 0L)
		{
			Character buff[256u] = {};
			mciGetErrorString(errCord, buff, 256u);
			if (MessageBox(NULL, buff, TEXT("ERROR!!"), MB_ICONHAND) == IDOK)
			{
				exit(0);
			}
		}
	}
	const String Pass;
	bool isStop;
	bool EffectivenessThread;
	HWND hWnd;
	Character buff[BuffSize];
	static void LoopThread(SimpleSoundDevice* sound)
	{
#ifdef _DEBUG
		OutputDebugString(TEXT("The thread has started.\n"));
#endif // DEBUG
		sound->EffectivenessThread = true;
		Character buff[BuffSize] = {};
		while (true)
		{
			if (!sound->isStop)
			{
				AssertMessage(sound->GetMode(buff));
				if (buff == String(TEXT("stopped")))sound->Play(false);
			}
			else break;
		}
		sound->EffectivenessThread = false;
#ifdef _DEBUG
		OutputDebugString(TEXT("The thread has terminated.\n"));
#endif // DEBUG
	}

public:
	SimpleSoundDevice(String pass, HWND hwnd = NULL)
		:Pass(pass), isStop(true), EffectivenessThread(false), hWnd(hwnd), buff(TEXT(""))
	{
		AssertMessage(mciSendString(String(TEXT("open ") + Pass).c_str(), NULL, 0U, hWnd));
	}

	virtual ~SimpleSoundDevice()
	{
		AssertMessage(Stop());
		while (EffectivenessThread);
		AssertMessage(mciSendString(String(TEXT("close ") + Pass).c_str(), NULL, 0u, hWnd));
	}

	virtual MCIERROR Play(bool loop = false)
	{
		isStop = false;
		if (loop)
		{
			std::thread L_Thread(SimpleSoundDevice::LoopThread, this);
			L_Thread.detach();
			return 0L;
		}
		return mciSendString(String(TEXT("play ") + Pass + TEXT(" from 0")).c_str(), NULL, 0u, hWnd);
	}

	virtual MCIERROR Stop()
	{
		isStop = true;
		return mciSendString(String(_TEXT("stop ") + Pass).c_str(), NULL, 0u, hWnd);
	}

	virtual MCIERROR Resume()
	{
		AssertMessage(GetMode(buff));
		return (buff == String(_TEXT("paused"))) ? mciSendString(String(_TEXT("resume ") + Pass).c_str(), NULL, 0u, hWnd) : 0L;
	}

	virtual MCIERROR Pause()
	{
		AssertMessage(GetMode(buff));
		return (buff == String(TEXT("playing"))) ? mciSendString(String(TEXT("pause ") + Pass).c_str(), NULL, 0u, hWnd) : 0L;
	}

	virtual MCIERROR GetPosition(int* result)
	{
		Character buff[BuffSize];
		MCIERROR errCord = mciSendString(String(TEXT("status ") + Pass + TEXT(" position")).c_str(), buff, BuffSize, hWnd);
		(*result) = StrToInt(buff);
		return errCord;
	}

	virtual MCIERROR GetLength(int* result)
	{
		Character buff[BuffSize];
		MCIERROR errCord = mciSendString(String(_TEXT("status ") + Pass + _TEXT(" length")).c_str(), buff, BuffSize, hWnd);
		(*result) = StrToInt(buff);
		return errCord;
	}

	virtual MCIERROR GetMode(Character(&result)[BuffSize])
	{
		return mciSendString(String(TEXT("status ") + Pass + TEXT(" mode")).c_str(), result, BuffSize, hWnd);
	}

	virtual MCIERROR SetTimeFormatMilliseconds()
	{
		return mciSendString(String(TEXT("set ") + Pass + TEXT(" time format milliseconds")).c_str(), NULL, 0u, hWnd);
	}

	SimpleSoundDevice& operator=(SimpleSoundDevice&) = default;
	SimpleSoundDevice(SimpleSoundDevice&) = default;
};

#endif // !INCLUDE_SIMPLE_SOUND_DEVICE