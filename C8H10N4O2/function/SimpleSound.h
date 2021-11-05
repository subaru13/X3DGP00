#ifndef INCLUDE_SIMPLE_SOUND_DEVICE
#define INCLUDE_SIMPLE_SOUND_DEVICE

#pragma comment ( lib, "winmm.lib" )
#pragma comment ( lib, "Xaudio2.lib")

#include <xaudio2.h>
#include <memory>
#include <assert.h>

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

class CXAMaster
{
private:
	IXAudio2* p_ixaudio2 = nullptr;
	IXAudio2MasteringVoice* p_master_voice = nullptr;
	CXAMaster()
	{
		HRESULT hr = S_OK;
		hr = XAudio2Create(&p_ixaudio2);
		assert(hr == S_OK);
		hr = p_ixaudio2->CreateMasteringVoice(&p_master_voice);
		assert(hr == S_OK);
	}
public:
	~CXAMaster()
	{
		if (p_master_voice != nullptr)
		{
			p_master_voice->DestroyVoice();
			p_master_voice = nullptr;
		}
		if (p_ixaudio2 != nullptr)
		{
			p_ixaudio2->Release();
			p_ixaudio2 = nullptr;
		}
	}

	static std::shared_ptr<CXAMaster>& getInstance()
	{
		static std::shared_ptr<CXAMaster> ins{ new CXAMaster() };
		return ins;
	}

	void createSourceVoice(
		IXAudio2SourceVoice** source_voice,
		WAVEFORMATEX* wformat)
	{
		HRESULT hr = p_ixaudio2->CreateSourceVoice(source_voice, wformat);
		assert(hr == S_OK);
	}

	void enableMute()
	{
		HRESULT hr = p_master_voice->SetVolume(0.0f);
		assert(hr == S_OK);
	}

	void disableMute()
	{
		HRESULT hr = p_master_voice->SetVolume(1.0f);
		assert(hr == S_OK);
	}

};

class CXAudio
{
protected:
	std::shared_ptr<CXAMaster>	cxa_master;
	IXAudio2SourceVoice*		source_voice;
	WAVEFORMATEXTENSIBLE		wformat;
	BYTE*						chunk_data;
	XAUDIO2_BUFFER				xaudio2_buffer;
	float						volume;
protected:
	HRESULT findChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
	{
		HRESULT hr = S_OK;
		if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
			return HRESULT_FROM_WIN32(GetLastError());
		DWORD dwChunkType;
		DWORD dwChunkDataSize;
		DWORD dwRIFFDataSize = 0;
		DWORD dwFileType;
		DWORD bytesRead = 0;
		DWORD dwOffset = 0;
		while (hr == S_OK)
		{
			DWORD dwRead;
			if (FALSE == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			if (FALSE == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			switch (dwChunkType)
			{
			case fourccRIFF:
				dwRIFFDataSize = dwChunkDataSize;
				dwChunkDataSize = 4;
				if (FALSE == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
					hr = HRESULT_FROM_WIN32(GetLastError());
				break;
			default:
				if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
					return HRESULT_FROM_WIN32(GetLastError());
			}
			dwOffset += sizeof(DWORD) * 2;
			if (dwChunkType == fourcc)
			{
				dwChunkSize = dwChunkDataSize;
				dwChunkDataPosition = dwOffset;
				return S_OK;
			}
			dwOffset += dwChunkDataSize;
			if (bytesRead >= dwRIFFDataSize) return S_FALSE;
		}
		return S_OK;
	}
	HRESULT readChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
	{
		HRESULT hr = S_OK;
		if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
			return HRESULT_FROM_WIN32(GetLastError());
		DWORD dwRead;
		if (FALSE == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}
public:
	CXAudio(const wchar_t* filename, float volume = 1.0f)
		:cxa_master(CXAMaster::getInstance()),
		source_voice(nullptr), wformat(),
		chunk_data(nullptr), xaudio2_buffer(),
		volume(volume)
	{
		HANDLE file = CreateFileW(
			filename,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL, OPEN_EXISTING,
			NULL, NULL);
		assert(INVALID_HANDLE_VALUE != file);
		assert(INVALID_SET_FILE_POINTER != SetFilePointer(file, NULL, NULL, FILE_BEGIN));
		DWORD chunk_size;
		DWORD chunk_position;
		findChunk(file, fourccRIFF, chunk_size, chunk_position);
		DWORD filetype;
		readChunkData(file, &filetype, sizeof(DWORD), chunk_position);
		assert(filetype == fourccWAVE);
		findChunk(file, fourccFMT, chunk_size, chunk_position);
		readChunkData(file, &wformat, chunk_size, chunk_position);
		findChunk(file, fourccDATA, chunk_size, chunk_position);
		chunk_data = new BYTE[chunk_size];
		readChunkData(file, chunk_data, chunk_size, chunk_position);
		xaudio2_buffer.AudioBytes = chunk_size;
		xaudio2_buffer.pAudioData = chunk_data;
		xaudio2_buffer.Flags = XAUDIO2_END_OF_STREAM;
		cxa_master->createSourceVoice(&source_voice, &wformat.Format);
	}

	virtual void play(bool loop = true)
	{
		stop();
		HRESULT hr = S_OK;
		xaudio2_buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : FALSE;
		hr = source_voice->SubmitSourceBuffer(&xaudio2_buffer);
		assert(hr == S_OK);
		hr = source_voice->Start();
		assert(hr == S_OK);
		hr = source_voice->SetVolume(volume);
		assert(hr == S_OK);
	}

	virtual void stop()
	{
		HRESULT hr = S_OK;
		hr = source_voice->Stop();
		assert(hr == S_OK);
		hr = source_voice->FlushSourceBuffers();
		assert(hr == S_OK);
	}

	virtual void pause()
	{
		HRESULT hr = S_OK;
		hr = source_voice->Stop();
		assert(hr == S_OK);
	}

	virtual void resume()
	{
		HRESULT hr = S_OK;
		hr = source_voice->Start();
		assert(hr == S_OK);
	}

	virtual void setVolume(float _volume)
	{
		volume = _volume;
		HRESULT hr = S_OK;
		hr = source_voice->SetVolume(volume);
		assert(hr == S_OK);
	}

	virtual const float& getVolume()const { return volume; }

	virtual ~CXAudio()
	{
		if (source_voice != nullptr)
		{
			source_voice->DestroyVoice();
			source_voice = nullptr;
		}
		if (chunk_data != nullptr)
		{
			delete[] chunk_data;
			chunk_data = nullptr;
		}
		cxa_master.reset();
	}
};

#undef fourccRIFF
#undef fourccDATA
#undef fourccFMT
#undef fourccWAVE
#undef fourccXWMA
#undef fourccDPDS
#endif // !INCLUDE_SIMPLE_SOUND_DEVICE