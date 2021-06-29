#pragma once
#include "sprite.h"
#include "sprite_batch.h"
#include "OffScreen.h"
#include "skinned_mesh.h"
#include "SceneConstantBuffer.h"
#include "geometry_primitive.h"
#include <memory>


/// <summary>
///	スプライトを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="file_name">テクスチャのアドレス</param>
inline std::unique_ptr<Sprite> MakeSprite(ID3D11Device* device, const wchar_t* file_name = L"\0")
{
	return std::make_unique<Sprite>(device, file_name);
}

/// <summary>
///	スプライトを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="new_shader_resource_view">シェーダーリソースビュー</param>
inline std::unique_ptr<Sprite> MakeSprite(ID3D11Device* device, ID3D11ShaderResourceView* new_shader_resource_view)
{
	return std::make_unique<Sprite>(device, new_shader_resource_view);
}

/// <summary>
/// スプライトバッチを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="max_sprites">表示最大枚数</param>
/// <param name="file_name">テクスチャのアドレス</param>
inline std::unique_ptr<SpriteBatch> MakeSpriteBatch(ID3D11Device* device, size_t max_sprites, const wchar_t* file_name = L"\0")
{
	return std::make_unique<SpriteBatch>(device, max_sprites, file_name);
}

/// <summary>
/// スプライトバッチを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="max_sprites">表示最大枚数</param>
/// <param name="new_shader_resource_view">シェーダーリソースビュー</param>
inline std::unique_ptr<SpriteBatch> MakeSpriteBatch(ID3D11Device* device, size_t max_sprites, ID3D11ShaderResourceView* new_shader_resource_view)
{
	return std::make_unique<SpriteBatch>(device, max_sprites, new_shader_resource_view);
}

/// <summary>
/// スキンドメッシュを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="fbx_filename">メッシュデータのアドレス(fbx,obj,json)</param>
/// <param name="triangulate">三角化するか</param>
/// <param name="sampling_rate">アニメションのサンプリングレート</param>
inline std::unique_ptr<SkinnedMesh> MakeSkinnedMesh(ID3D11Device* device, const char* filename, bool triangulate = false, float sampling_rate = 0.0f)
{
	return std::make_unique<SkinnedMesh>(device, filename, triangulate, sampling_rate);
}

/// <summary>
/// シーンコンスタントを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
inline std::unique_ptr<SceneConstant> MakeSceneConstant(ID3D11Device* device)
{
	return std::make_unique<SceneConstant>(device);
}

/// <summary>
/// ジオメトリプリミティブを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="config">プリミティブの構成</param>
inline std::unique_ptr<GeometryPrimitive> MakeGeometryPrimitive(ID3D11Device* device, GP_CONFIG config)
{
	return std::make_unique<GeometryPrimitive>(device, config);
}

/// <summary>
///	オフスクリーンを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="link_destination">シェーダーリソースビューのリンク先</param>
/// <param name="w">ビューの横幅</param>
/// <param name="h">ビューの縦幅</param>
/// <param name="format">シェーダーリソースビューのフォーマット</param>
/// <param name="need_renderer">レンダラーが必要か。</param>
inline std::unique_ptr<OffScreen> MakeOffScreen(ID3D11Device* device,
	OffScreen::LINK_DESTINATION link_destination,
	UINT w, UINT h,
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
	bool need_renderer = true)
{
	return std::make_unique<OffScreen>(device, link_destination,w,h,format, need_renderer);
}