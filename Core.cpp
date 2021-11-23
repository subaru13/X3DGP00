/********************************************************************/
/*							インクルード							*/
/********************************************************************/
#include "C8H10N4O2/Include.h"
#include <fstream>
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


IDepthShadowMap	shadow_map;
ISceneConstant	scene_constant;
ISkinnedMesh	skinned_meshes[2];
FLOAT4X4		world[2];
FLOAT3			dir;
float			dis = 10.0f;
CameraControl	camera;
Key lock{ 'L' };
bool flag = false;
/*********************************************************************
			＠ゲームループ前に行う初期化処理
			シーンの追加などを記入してください。
*********************************************************************/
void initializationProcessBeforeLoop(ID3D11Device* device)
{
	shadow_map = makeDepthShadowMap(device, SCREEN_WIDTH, SCREEN_HEIGHT);
	scene_constant = makeSceneConstant(device);
	skinned_meshes[0] = makeSkinnedMesh(device, "data/Slime.fbx");
	skinned_meshes[1] = makeSkinnedMesh(device, "data/cube.001.2.fbx");
}

/*********************************************************************
			＠更新処理
			シーンマネージャーの更新処理とは別に実行されます。
			常時更新したい処理はこちらに記入してください。
*********************************************************************/
void updateProcess(float elapsed_time)
{
	static float bias = shadow_map->getBias() * 10.0f;
	static FLOAT3 shadow_color = shadow_map->getShadowColor();
	ImGui::ColorEdit3("shadow_color", &shadow_color.x);
	ImGui::SliderFloat3("dir", &dir.x, -1.0f, 1.0f);
	ImGui::SliderFloat("bias", &bias, 0.001f, 1, "%.6f / 10");
	ImGui::SliderFloat("dis", &dis, 1.f, 300.0f, "%.6f");
	if (flag)camera.update(elapsed_time);
	if (lock.down())flag = !flag;
	scene_constant->setView(camera.getView());
	scene_constant->setProjection(camera.getProjection());
}
/*********************************************************************
			＠描画処理
			シーンマネージャーの描画処理とは別に実行されます。
			常時描画したい処理はこちらに記入してください。
*********************************************************************/
void drawingProcess(ID3D11DeviceContext* immediate_context, float elapsed_time)
{
	scene_constant->send(immediate_context);
	shadow_map->blit(immediate_context ,
		[&](ID3D11PixelShader** pixel_shader[2])
	{
		skinned_meshes[0]->render(immediate_context, pixel_shader[DepthShadow::FETCH_PIXEL_SHADER_ID::FPSI_SKINNED_MESH], world[0], NULL);
		skinned_meshes[1]->render(immediate_context, pixel_shader[DepthShadow::FETCH_PIXEL_SHADER_ID::FPSI_SKINNED_MESH], world[1], NULL);
	}, dir, dis);
}

/*********************************************************************
			＠ループ終了後の終了処理
			解放処理などをこちらに記入してください。
*********************************************************************/
void endProcessAfterTheLoopEnds()
{
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