#pragma comment ( lib, "winmm.lib" )
#pragma comment ( lib, "Xaudio2.lib")

#include "CXAUDIO.h"
#include <assert.h>

AudioResource::AudioResource(std::string filename)
	:hmmio(NULL), pcm_data(NULL), pcm_data_size(), wave_format()
{
	assert(!filename.empty());
	hmmio = mmioOpenA(filename.data(), NULL, MMIO_ALLOCBUF | MMIO_READ);
	assert(hmmio != NULL);
	MMCKINFO main_chunk;
	MMCKINFO sub_chunk;
	main_chunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	MMRESULT mr = mmioDescend(hmmio, &main_chunk, NULL, MMIO_FINDRIFF);
	assert(mr == 0);
	sub_chunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mr = mmioDescend(hmmio, &sub_chunk, &main_chunk, MMIO_FINDCHUNK);
	assert(mr == 0);
	mr = mmioRead(hmmio, (HPSTR)&wave_format, sizeof(wave_format)) != sizeof(wave_format);
	assert(mr == 0);
	assert(wave_format.wFormatTag == WAVE_FORMAT_PCM);
	wave_format.cbSize = 0;
	mr = mmioAscend(hmmio, &sub_chunk, 0);
	assert(mr == 0);
	sub_chunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mr = mmioDescend(hmmio, &sub_chunk, &main_chunk, MMIO_FINDCHUNK);
	assert(mr == 0);
	pcm_data_size = sub_chunk.cksize;
}

AudioResource::~AudioResource()
{
	if (hmmio) { mmioClose(hmmio, 0); }
	if (pcm_data != NULL) { delete pcm_data; }
}

LONG AudioResource::read(BYTE* buf, DWORD size)
{
	return (hmmio != NULL) ? mmioRead(hmmio, (HPSTR)buf, size) : 0;
}

bool AudioResource::read()
{
	return pcm_data ? true : read(pcm_data = new BYTE[pcm_data_size], pcm_data_size) > 0;
}

LONG AudioResource::seek(LONG offset)
{
	return (hmmio != NULL) ? mmioSeek(hmmio, offset, SEEK_CUR) : 0;
}

CXADevice::CXADevice()
	:engine(NULL), mastering_voice(NULL)
{
	XAudio2Create(&engine);
	engine->CreateMasteringVoice(&mastering_voice);
}

HRESULT CXADevice::createSourceVoice(IXAudio2SourceVoice** source_voice,
	const WAVEFORMATEX* source_format) const
{
	return engine->CreateSourceVoice(source_voice, source_format, XAUDIO2_VOICE_USEFILTER);
}

HRESULT CXADevice::enableMute() const { return mastering_voice->SetVolume(0.0f); }

HRESULT CXADevice::disableMute() const { return mastering_voice->SetVolume(1.0f); }

bool CXADevice::isEnableMute() const
{
	float v = 0.0f;
	mastering_voice->GetVolume(&v);
	return (1.0f - v) > FLT_EPSILON;
}

XAUDIO2_VOICE_DETAILS CXADevice::getDetails() const
{
	XAUDIO2_VOICE_DETAILS details{};
	mastering_voice->GetVoiceDetails(&details);
	return details;
}

CXADevice::~CXADevice()
{
	if (mastering_voice) mastering_voice->DestroyVoice();
	if (engine) engine->Release();
}

CXAudio::CXAudio(CXADevice* device, std::string filename)
	:source_voice(NULL), resource(new AudioResource(filename)),device_details(device->getDetails())
{
	assert(resource);
	HRESULT hr = device->createSourceVoice(&source_voice, &resource->getWaveFormat());
	assert(hr == S_OK);
}

CXAudio::CXAudio(CXADevice* device, std::shared_ptr<AudioResource> _resource)
	:source_voice(NULL), resource(_resource), device_details(device->getDetails())
{
	assert(resource);
	HRESULT hr = device->createSourceVoice(&source_voice, &resource->getWaveFormat());
	assert(hr == S_OK);
}

void CXAudio::process(CXAPO* apo)
{
	if (apo) apo->process(source_voice, resource.get(), device_details);
	else
	{
		HRESULT hr = source_voice->SetEffectChain(NULL);
		assert(hr == S_OK);
		XAUDIO2_FILTER_PARAMETERS filter_parameter{ XAUDIO2_DEFAULT_FILTER_TYPE ,XAUDIO2_DEFAULT_FILTER_FREQUENCY ,XAUDIO2_DEFAULT_FILTER_ONEOVERQ };
		hr = source_voice->SetFilterParameters(&filter_parameter);
		assert(hr == S_OK);
		int matrix_size = (int)device_details.InputChannels * resource->getWaveFormat().nChannels;
		float* output_matrix = new float[matrix_size];
		for (int i = 0; i < matrix_size; i++) { output_matrix[i] = 0.5f; }
		source_voice->SetOutputMatrix(NULL,
			resource->getWaveFormat().nChannels,
			device_details.InputChannels, output_matrix);
		delete[] output_matrix;
	}
}

void CXAudio::play(CXAPO* apo, bool loop)
{
	stop();
	XAUDIO2_BUFFER xaudio2_buffer{};
	xaudio2_buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : XAUDIO2_NO_LOOP_REGION;
	xaudio2_buffer.AudioBytes = resource->getPcmDataSize();
	xaudio2_buffer.pAudioData = resource->getPcmData();
	if (xaudio2_buffer.pAudioData == NULL)
	{
		resource->read();
		xaudio2_buffer.pAudioData = resource->getPcmData();
	}
	xaudio2_buffer.Flags = XAUDIO2_END_OF_STREAM;
	HRESULT hr = source_voice->SubmitSourceBuffer(&xaudio2_buffer);
	assert(hr == S_OK);
	hr = source_voice->Start();
	assert(hr == S_OK);
	if (apo)apo->process(source_voice, resource.get(), device_details);
	else
	{
		hr = source_voice->SetEffectChain(NULL);
		assert(hr == S_OK);
		XAUDIO2_FILTER_PARAMETERS filter_parameter{ XAUDIO2_DEFAULT_FILTER_TYPE ,XAUDIO2_DEFAULT_FILTER_FREQUENCY ,XAUDIO2_DEFAULT_FILTER_ONEOVERQ };
		hr = source_voice->SetFilterParameters(&filter_parameter);
		assert(hr == S_OK);
		int matrix_size = (int)device_details.InputChannels * resource->getWaveFormat().nChannels;
		float* output_matrix = new float[matrix_size];
		for (int i = 0; i < matrix_size; i++) { output_matrix[i] = 0.5f; }
		source_voice->SetOutputMatrix(NULL,
			resource->getWaveFormat().nChannels,
			device_details.InputChannels, output_matrix);
		delete[] output_matrix;
	}
}

void CXAudio::stop()
{
	HRESULT hr = source_voice->Stop();
	assert(hr == S_OK);
	hr = source_voice->FlushSourceBuffers();
	assert(hr == S_OK);
}

void CXAudio::pause()
{
	HRESULT hr = S_OK;
	hr = source_voice->Stop();
	assert(hr == S_OK);
}

void CXAudio::resume()
{
	HRESULT hr = S_OK;
	hr = source_voice->Start();
	assert(hr == S_OK);
}

void CXAudio::setVolume(float volume)
{
	HRESULT hr = S_OK;
	hr = source_voice->SetVolume(volume);
	assert(hr == S_OK);
}

float CXAudio::getVolume() const
{
	float volume = 0.0f;
	source_voice->GetVolume(&volume);
	return volume;
}

CXAudio::~CXAudio()
{
	stop();
	if (source_voice)source_voice->DestroyVoice();
	if (resource) resource.reset();
}

