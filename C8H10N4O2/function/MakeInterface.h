#pragma once
#include "sprite.h"
#include "sprite_batch.h"
#include "OffScreen.h"
#include "skinned_mesh.h"
#include "SceneConstantBuffer.h"
#include "geometry_primitive.h"
#include <memory>

/// <summary>
///	�X�v���C�g���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="file_name">�e�N�X�`���̃A�h���X</param>
inline std::unique_ptr<Sprite> MakeSprite(ID3D11Device* device, const wchar_t* file_name = L"\0")
{
	return std::make_unique<Sprite>(device, file_name);
}

/// <summary>
///	�X�v���C�g���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="new_shader_resource_view">�V�F�[�_�[���\�[�X�r���[</param>
inline std::unique_ptr<Sprite> MakeSprite(ID3D11Device* device, ID3D11ShaderResourceView* new_shader_resource_view)
{
	return std::make_unique<Sprite>(device, new_shader_resource_view);
}

/// <summary>
/// �X�v���C�g�o�b�`���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="max_sprites">�\���ő喇��</param>
/// <param name="file_name">�e�N�X�`���̃A�h���X</param>
inline std::unique_ptr<SpriteBatch> MakeSpriteBatch(ID3D11Device* device, size_t max_sprites, const wchar_t* file_name = L"\0")
{
	return std::make_unique<SpriteBatch>(device, max_sprites, file_name);
}

/// <summary>
/// �X�v���C�g�o�b�`���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="max_sprites">�\���ő喇��</param>
/// <param name="new_shader_resource_view">�V�F�[�_�[���\�[�X�r���[</param>
inline std::unique_ptr<SpriteBatch> MakeSpriteBatch(ID3D11Device* device, size_t max_sprites, ID3D11ShaderResourceView* new_shader_resource_view)
{
	return std::make_unique<SpriteBatch>(device, max_sprites, new_shader_resource_view);
}

/// <summary>
/// �X�L���h���b�V�����쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="fbx_filename">���b�V���f�[�^�̃A�h���X(fbx,obj,json)</param>
/// <param name="triangulate">�O�p�����邩</param>
/// <param name="sampling_rate">�A�j���V�����̃T���v�����O���[�g</param>
inline std::unique_ptr<SkinnedMesh> MakeSkinnedMesh(ID3D11Device* device, const char* filename, bool triangulate = false, float sampling_rate = 0.0f)
{
	return std::make_unique<SkinnedMesh>(device, filename, triangulate, sampling_rate);
}

/// <summary>
/// �V�[���R���X�^���g���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
inline std::unique_ptr<SceneConstant> MakeSceneConstant(ID3D11Device* device)
{
	return std::make_unique<SceneConstant>(device);
}

/// <summary>
/// �W�I���g���v���~�e�B�u���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="config">�v���~�e�B�u�̍\��</param>
inline std::unique_ptr<GeometryPrimitive> MakeGeometryPrimitive(ID3D11Device* device, GP_CONFIG config)
{
	return std::make_unique<GeometryPrimitive>(device, config);
}

/// <summary>
///	�I�t�X�N���[�����쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="link_destination">�V�F�[�_�[���\�[�X�r���[�̃����N��</param>
/// <param name="w">�r���[�̉���</param>
/// <param name="h">�r���[�̏c��</param>
/// <param name="format">�V�F�[�_�[���\�[�X�r���[�̃t�H�[�}�b�g</param>
/// <param name="need_renderer">�����_���[���K�v���B</param>
inline std::unique_ptr<OffScreen> MakeOffScreen(ID3D11Device* device,
	OffScreen::LINK_DESTINATION link_destination,
	UINT w, UINT h,
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
	bool need_renderer = true)
{
	return std::make_unique<OffScreen>(device, link_destination, w, h, format, need_renderer);
}