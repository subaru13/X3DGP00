#pragma once

#include "SpriteBatch.h"
#include <vector>
#include <string>

class ASCII
{
public:
	typedef int Handle;
	struct SprData
	{
		std::shared_ptr<SpriteBatch>	spr;
		std::wstring					file_name;
		FLOAT2							size_per_char;
		SprData() :spr(nullptr), file_name(), size_per_char() {}
	};
private:
	friend std::unique_ptr<ASCII> std::make_unique<ASCII>();
	ASCII() = default;
	std::vector<SprData>		datas;
	static std::unique_ptr<ASCII> instance;
public:
	/// <summary>
	/// フォントデータを追加します。
	/// </summary>
	/// <param name="device">有効なデバイス</param>
	/// <param name="file_name">フォントテクスチャのファイルアドレス</param>
	/// <param name="size_per_char">1文字当たりの切り取りサイズ</param>
	/// <returns>登録したハンドル(登録に失敗した場合-1)</returns>
	Handle addFont(ID3D11Device* device, const wchar_t* file_name, FLOAT2 size_per_char = { 16.0f,16.0f });

	/// <summary>
	/// 文字列を描画します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="handle">フォントのハンドル</param>
	/// <param name="str">描画する文字列</param>
	/// <param name="pos">表示位置</param>
	/// <param name="scale">スケール</param>
	/// <param name="color">色</param>
	/// <returns>表示終了時の右下の座標</returns>
	FLOAT2 print(ID3D11DeviceContext* immediate_context,
		Handle handle,
		std::string str,
		FLOAT2 pos,
		FLOAT2 scale = { 1.0f,1.0f },
		FLOAT4 color = { 1,1,1,1 });

	//インスタンスを作成します
	//【返り値】インスタンスの作成に成功した場合trueを返します。
	static bool createInstance();

	//インスタンスを取得します。
	static ASCII* getInstance()
	{
		if(instance == nullptr)createInstance();
		return instance.get();
	}

	//インスタンスを破棄します。
	//【返り値】インスタンスの破棄に成功した場合trueを返します。
	static bool destroyInstance();
	~ASCII();
};

using ASCII_HANDLE = ASCII::Handle;