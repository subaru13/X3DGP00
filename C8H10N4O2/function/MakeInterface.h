#pragma once
#include "Sprite.h"
#include "SpriteBatch.h"
#include "OffScreen.h"
#include "SkinnedMesh.h"
#include "SceneConstantBuffer.h"
#include "GeometryPrimitive.h"
#include "CreateComObjectHelpar.h"
#include "ConstantBuffer.h"
#include "Bloom.h"
#include <memory>

/// <summary>
///	スプライトを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="file_name">テクスチャのアドレス</param>
inline std::unique_ptr<Sprite> makeSprite(ID3D11Device* device, const wchar_t* file_name = L"\0")
{
	return std::make_unique<Sprite>(device, file_name);
}

/// <summary>
///	スプライトを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="new_shader_resource_view">シェーダーリソースビュー</param>
inline std::unique_ptr<Sprite> makeSprite(ID3D11Device* device, ID3D11ShaderResourceView* new_shader_resource_view)
{
	return std::make_unique<Sprite>(device, new_shader_resource_view);
}

/// <summary>
/// スプライトバッチを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="max_sprites">表示最大枚数</param>
/// <param name="file_name">テクスチャのアドレス</param>
inline std::unique_ptr<SpriteBatch> makeSpriteBatch(ID3D11Device* device, size_t max_sprites, const wchar_t* file_name = L"\0")
{
	return std::make_unique<SpriteBatch>(device, max_sprites, file_name);
}

/// <summary>
/// スプライトバッチを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="max_sprites">表示最大枚数</param>
/// <param name="new_shader_resource_view">シェーダーリソースビュー</param>
inline std::unique_ptr<SpriteBatch> makeSpriteBatch(ID3D11Device* device, size_t max_sprites, ID3D11ShaderResourceView* new_shader_resource_view)
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
inline std::unique_ptr<SkinnedMesh> makeSkinnedMesh(ID3D11Device* device, const char* filename, bool triangulate = false, float sampling_rate = 0.0f)
{
	return std::make_unique<SkinnedMesh>(device, filename, triangulate, sampling_rate);
}

/// <summary>
/// キーフレーム管理クラスを作成します。
/// </summary>
/// <param name="mesh">スキンドメッシュのシェアードポインタ</param>
inline std::unique_ptr<Keyframe> makeKeyframe(std::shared_ptr<SkinnedMesh> mesh)
{
	return std::make_unique<Keyframe>(mesh);
}

/// <summary>
/// コンスタントバッファを作成します。
/// </summary>
/// <typeparam name="T">コンスタントバッファに渡す型</typeparam>
/// <typeparam name="...Args"></typeparam>
/// <param name="device">有効なデバイス</param>
/// <param name="...args">コンスタントバッファ内で管理するデータのコンストラクタに渡すデータ</param>
template <class T,class... Args>
inline std::unique_ptr<ConstantBuffer<T>> makeConstantBuffer(ID3D11Device* device, Args... args)
{
	return std::make_unique<ConstantBuffer<T>>(device, args...);
}

/// <summary>
/// シーンコンスタントを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
inline std::unique_ptr<SceneConstant> makeSceneConstant(ID3D11Device* device)
{
	return std::make_unique<SceneConstant>(device);
}

/// <summary>
/// ジオメトリプリミティブを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="config">プリミティブの構成</param>
inline std::unique_ptr<GeometryPrimitive> makeGeometryPrimitive(ID3D11Device* device, GP_CONFIG config)
{
	return std::make_unique<GeometryPrimitive>(device, config);
}

/// <summary>
///	オフスクリーンを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="config">オフスクリーンの構成</param>
inline std::unique_ptr<OffScreen> makeOffScreen(ID3D11Device* device,OFFSCREEN_CONFIG config)
{
	return std::make_unique<OffScreen>(device, config);
}

/// <summary>
/// フルスクリーンクワッドを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="ps_filename">ピクセルシェーダー(cso)のアドレス</param>
inline std::unique_ptr<FullScreenQuad> makeFullScreenQuad(ID3D11Device* device, const char* ps_filename = NULL)
{
	return std::make_unique<FullScreenQuad>(device, ps_filename);
}

/// <summary>
/// ガウシアンフィルターを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="w">ビューの横幅</param>
/// <param name="h">ビューの縦幅</param>
/// <param name="format">シェーダーリソースビューのフォーマット</param>
inline std::unique_ptr<GaussianFilter> makeGaussianFilter(ID3D11Device* device,
	UINT w, UINT h,
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
{
	return std::make_unique<GaussianFilter>(device, w, h, format);
}

/// <summary>
/// ブルームレンダラーを作成します。
/// </summary>
/// <param name="device">有効なデバイス</param>
/// <param name="w">ビューの横幅</param>
/// <param name="h">ビューの縦幅</param>
/// <param name="format">シェーダーリソースビューのフォーマット</param>
inline std::unique_ptr<BloomRenderer> makeBloomRenderer(ID3D11Device* device,
	UINT w, UINT h,
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
{
	return std::make_unique<BloomRenderer>(device, w, h, format);
}

template <class T>
using MyInterface			= std::shared_ptr <T>;

using ISprite				= MyInterface<Sprite>;
using ISpriteBatch			= MyInterface<SpriteBatch>;
using ISkinnedMesh			= MyInterface<SkinnedMesh>;
using IKeyframe				= MyInterface<Keyframe>;
template <class T>
using IConstantBuffer		= MyInterface<ConstantBuffer<T>>;
using ISceneConstant		= MyInterface<SceneConstant>;
using IGeometryPrimitive	= MyInterface<GeometryPrimitive>;
using IOffScreen			= MyInterface<OffScreen>;
using IFullScreenQuad		= MyInterface<FullScreenQuad>;
using IGaussianFilter		= MyInterface<GaussianFilter>;
using IBloomRenderer		= MyInterface<BloomRenderer>;
