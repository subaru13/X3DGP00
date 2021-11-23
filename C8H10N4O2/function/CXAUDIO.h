#pragma once

#include <xaudio2.h>
#include <string>
#include <memory>
#include <vector>
#include <map>

class AudioResource;
class CXADevice;
class CXAPO;
class CXAudio;

class AudioResource
{
private:
	HMMIO			hmmio;
	BYTE*			pcm_data;
	DWORD			pcm_data_size;
	WAVEFORMATEX	wave_format;
public:
	AudioResource(std::string filename);
	~AudioResource();
	LONG read(BYTE* buf, DWORD size);
	bool read();
	LONG seek(LONG offset);
	const BYTE* getPcmData()const { return pcm_data; }
	const DWORD& getPcmDataSize()const { return pcm_data_size; }
	const WAVEFORMATEX& getWaveFormat()const { return wave_format; }
};

class CXADevice
{
private:
	IXAudio2*				engine;
	IXAudio2MasteringVoice*	mastering_voice;
public:
	CXADevice();
	HRESULT createSourceVoice(IXAudio2SourceVoice**,const WAVEFORMATEX*)const;
	HRESULT enableMute()const;
	HRESULT disableMute()const;
	bool isEnableMute()const;
	XAUDIO2_VOICE_DETAILS getDetails()const;
	~CXADevice();
};

class CXAPO
{
public:
	CXAPO() {}
	virtual void process(IXAudio2SourceVoice*, const AudioResource*, const XAUDIO2_VOICE_DETAILS&) = 0;
	virtual ~CXAPO() {}
};

class CXAudio
{
private:
	IXAudio2SourceVoice*			source_voice;
	std::shared_ptr<AudioResource>	resource;
	const XAUDIO2_VOICE_DETAILS		device_details;
public:
	CXAudio(CXADevice* device, std::string filename);
	CXAudio(CXADevice* device, std::shared_ptr<AudioResource> resource);
	virtual void process(CXAPO* apo);
	virtual void play(CXAPO* apo = NULL, bool loop = false);
	virtual void stop();
	virtual void pause();
	virtual void resume();
	virtual void setVolume(float volume);
	virtual float getVolume()const;
	virtual const AudioResource* getResource()const { return resource.get(); }
	virtual ~CXAudio();
};
