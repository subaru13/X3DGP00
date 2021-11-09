/********************************************************************/
/*							インクルード							*/
/********************************************************************/
#include "C8H10N4O2/Include.h"

/********************************************************************/
/*							プロトタイプ宣言						*/
/********************************************************************/

/*********************************************************************
			＠実行順について

	initializationProcessBeforeLoop
			↓
	debugModeInitializationProcessBeforeLoop(デバック時のみ)
			↓	(ループ開始)
	SceneManager::edit
			↓
	updateProcess
			↓
	debugModeUpdateProcess(デバック時のみ)
			↓
	SceneManager::render
			↓
	drawingProcess
			↓
	debugModeDrawingProcess(デバック時のみ)
			↓	(ループ終了)
	SceneManager::release
			↓
	endProcessAfterTheLoopEnds
			↓
	debugModeEndProcessAfterTheLoopEnds(デバック時のみ)

*********************************************************************/



#include <xaudio2fx.h>

class Reverb :public CXAPO
{
public:
	XAUDIO2FX_REVERB_PARAMETERS parameter{};
public:
	Reverb(CXADevice* device) :CXAPO(device) {}

	virtual void process(IXAudio2SourceVoice* source_voice, const AudioResource* resource)override
	{
		IUnknown* unknown;
		XAudio2CreateReverb(&unknown);
		XAUDIO2FX_REVERB_I3DL2_PARAMETERS i3dl2 = XAUDIO2FX_I3DL2_PRESET_GENERIC;
		ReverbConvertI3DL2ToNative(&i3dl2, &parameter);
		XAUDIO2_EFFECT_DESCRIPTOR descriptor{};
		descriptor.InitialState = TRUE;
		descriptor.OutputChannels = resource->getWaveFormat().nChannels;
		descriptor.pEffect = unknown;
		XAUDIO2_EFFECT_CHAIN chain{};
		chain.EffectCount = 1;
		chain.pEffectDescriptors = &descriptor;
		source_voice->SetEffectChain(&chain);
		source_voice->SetEffectParameters(0, &parameter, sizeof(XAUDIO2FX_REVERB_PARAMETERS));
		unknown->Release();
		int matrix_size = (int)device_details.InputChannels * resource->getWaveFormat().nChannels;
		float* output_matrix = new float[matrix_size];
		output_matrix[0] = 0.0f;
		output_matrix[1] = 1.0f;
		output_matrix[2] = 0.0f;
		output_matrix[3] = 1.0f;
		source_voice->SetOutputMatrix(NULL,
			resource->getWaveFormat().nChannels,
			device_details.InputChannels, output_matrix);

		delete[] output_matrix;
	}
	virtual ~Reverb()override {}
};

IAudioDevice				mmm_device;
IAudio						mmm_audio;
IAPO<Reverb>				mmm_reverb;
IAudioResource				mmm_resource;


/*********************************************************************
			＠ゲームループ前に行う初期化処理
			シーンの追加などを記入してください。
*********************************************************************/
void initializationProcessBeforeLoop(ID3D11Device* device)
{
	mmm_device = makeAudioDevice();
	mmm_resource = makeAudioResource("wwww.wav");
	mmm_resource->read();
	mmm_reverb = makeAPO<Reverb>(mmm_device.get());
	mmm_audio = makeAudio(mmm_device.get(), mmm_resource);
	mmm_audio->play(mmm_reverb.get());
}

/*********************************************************************
			＠更新処理
			シーンマネージャーの更新処理とは別に実行されます。
			常時更新したい処理はこちらに記入してください。
*********************************************************************/
void updateProcess(float elapsed_time)
{
	ImGui::Text(mmm_device->isEnableMute() ? "mmm_device->isEnableMute() == true" : "mmm_device->isEnableMute() == false");
	
	if (ImGui::Button("mute"))mmm_device->enableMute();
	ImGui::SameLine();
	if (ImGui::Button("no mute"))mmm_device->disableMute();
	if (ImGui::Button("stop"))mmm_audio->stop();
	ImGui::SameLine();
	if (ImGui::Button("play"))mmm_audio->play(mmm_reverb.get());
	ImGui::SameLine();
	if (ImGui::Button("pause"))mmm_audio->pause();
	ImGui::SameLine();
	if (ImGui::Button("resume"))mmm_audio->resume();
}
/*********************************************************************
			＠描画処理
			シーンマネージャーの描画処理とは別に実行されます。
			常時描画したい処理はこちらに記入してください。
*********************************************************************/
void drawingProcess(ID3D11DeviceContext* immediate_context, float elapsed_time)
{
}

/*********************************************************************
			＠ループ終了後の終了処理
			解放処理などをこちらに記入してください。
*********************************************************************/
void endProcessAfterTheLoopEnds()
{
	mmm_audio.reset();
	mmm_reverb.reset();
	mmm_resource.reset();
	mmm_device.reset();
}

#ifdef _DEBUG
/*********************************************************************
						デバック用
*********************************************************************/

/*********************************************************************
			＠ゲームループ前に行うデバック用の初期化処理
			シーンの追加などを記入してください。
*********************************************************************/
void debugModeInitializationProcessBeforeLoop(ID3D11Device* device)
{
}

/*********************************************************************
			＠デバック用の更新処理
			シーンマネージャーの更新処理とは別に実行されます。
			常時更新したい処理はこちらに記入してください。
*********************************************************************/
void debugModeUpdateProcess(float elapsed_time)
{
}

/*********************************************************************
			＠デバック用の描画処理
			シーンマネージャーの描画処理とは別に実行されます。
			常時描画したい処理はこちらに記入してください。
*********************************************************************/
void debugModeDrawingProcess(ID3D11DeviceContext* immediate_context, float elapsed_time)
{
}

/*********************************************************************
			＠ループ終了後のデバック用の終了処理
			解放処理などをこちらに記入してください。
*********************************************************************/
void debugModeEndProcessAfterTheLoopEnds()
{
}
#endif // _DEBUG