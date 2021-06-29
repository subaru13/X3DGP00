﻿/********************************************************************/
/*							インクルード							*/
/********************************************************************/
#include "C8H10N4O2/include.h"

/********************************************************************/
/*							プロトタイプ宣言						*/
/********************************************************************/

/*********************************************************************
			＠実行順について

	Initialization_Process_Before_Loop
			↓
	Debug_Mode_Initialization_Process_Before_Loop(デバック時のみ)
			↓	(ループ開始)
	SceneManager::edit
			↓
	Update_Process
			↓
	Debug_Mode_Update_Process(デバック時のみ)
			↓
	SceneManager::render
			↓
	Drawing_Process
			↓
	Debug_Mode_Drawing_Process(デバック時のみ)
			↓	(ループ終了)
	SceneManager::release
			↓
	End_Process_After_The_Loop_ends
			↓
	Debug_Mode_End_Process_After_The_Loop_ends(デバック時のみ)

*********************************************************************/

/*********************************************************************
			＠ゲームループ前に行う初期化処理
			シーンの追加などを記入してください。
*********************************************************************/
void Initialization_Process_Before_Loop(ID3D11Device* device)
{
}

/*********************************************************************
			＠更新処理
			シーンマネージャーの更新処理とは別に実行されます。
			常時更新したい処理はこちらに記入してください。
*********************************************************************/
void Update_Process(float elapsed_time)
{
}

/*********************************************************************
			＠描画処理
			シーンマネージャーの描画処理とは別に実行されます。
			常時描画したい処理はこちらに記入してください。
*********************************************************************/
void Drawing_Process(ID3D11DeviceContext* immediate_context, float elapsed_time)
{
}

/*********************************************************************
			＠ループ終了後の終了処理
			解放処理などをこちらに記入してください。
*********************************************************************/
void End_Process_After_The_Loop_ends()
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
void Debug_Mode_Initialization_Process_Before_Loop(ID3D11Device* device)
{
}

/*********************************************************************
			＠デバック用の更新処理
			シーンマネージャーの更新処理とは別に実行されます。
			常時更新したい処理はこちらに記入してください。
*********************************************************************/
void Debug_Mode_Update_Process(float elapsed_time)
{
}

/*********************************************************************
			＠デバック用の描画処理
			シーンマネージャーの描画処理とは別に実行されます。
			常時描画したい処理はこちらに記入してください。
*********************************************************************/
void Debug_Mode_Drawing_Process(ID3D11DeviceContext* immediate_context, float elapsed_time)
{
}

/*********************************************************************
			＠ループ終了後のデバック用の終了処理
			解放処理などをこちらに記入してください。
*********************************************************************/
void Debug_Mode_End_Process_After_The_Loop_ends()
{
}
#endif // _DEBUG