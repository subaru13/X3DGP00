#pragma once
#include "SimpleSound.h"
#include "Sprite.h"
#include "SpriteBatch.h"
#include "OffScreen.h"
#include "SkinnedMesh.h"
#include "SceneConstantBuffer.h"
#include "GeometryPrimitive.h"
#include "CreateComObjectHelpar.h"
#include "ConstantBuffer.h"
#include "Bloom.h"
#include "Particle.h"
#include <memory>


/// <summary>
/// �T�E���h���쐬���܂��B
/// </summary>
/// <param name="filename">�T�E���h�̃A�h���X</param>
/// <param name="volume">�{�����[��</param>
inline std::unique_ptr<CXAudio> makeSound(const wchar_t* filename, float volume = 1.0f)
{
	return std::make_unique<CXAudio>(filename, volume);
}

/// <summary>
///	�X�v���C�g���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="file_name">�e�N�X�`���̃A�h���X</param>
inline std::unique_ptr<Sprite> makeSprite(ID3D11Device* device, const wchar_t* file_name = L"\0")
{
	return std::make_unique<Sprite>(device, file_name);
}

/// <summary>
///	�X�v���C�g���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="new_shader_resource_view">�V�F�[�_�[���\�[�X�r���[</param>
inline std::unique_ptr<Sprite> makeSprite(ID3D11Device* device, ID3D11ShaderResourceView* new_shader_resource_view)
{
	return std::make_unique<Sprite>(device, new_shader_resource_view);
}

/// <summary>
/// �X�v���C�g�o�b�`���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="max_sprites">�\���ő喇��</param>
/// <param name="file_name">�e�N�X�`���̃A�h���X</param>
inline std::unique_ptr<SpriteBatch> makeSpriteBatch(ID3D11Device* device, size_t max_sprites, const wchar_t* file_name = L"\0")
{
	return std::make_unique<SpriteBatch>(device, max_sprites, file_name);
}

/// <summary>
/// �X�v���C�g�o�b�`���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="max_sprites">�\���ő喇��</param>
/// <param name="new_shader_resource_view">�V�F�[�_�[���\�[�X�r���[</param>
inline std::unique_ptr<SpriteBatch> makeSpriteBatch(ID3D11Device* device, size_t max_sprites, ID3D11ShaderResourceView* new_shader_resource_view)
{
	return std::make_unique<SpriteBatch>(device, max_sprites, new_shader_resource_view);
}

/// <summary>
/// �X�L���h���b�V�����쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="fbx_filename">���b�V���f�[�^�̃A�h���X(fbx,obj,skn)</param>
/// <param name="triangulate">�O�p�����邩</param>
/// <param name="sampling_rate">�A�j���V�����̃T���v�����O���[�g</param>
inline std::unique_ptr<SkinnedMesh> makeSkinnedMesh(ID3D11Device* device, const char* filename, bool triangulate = false, float sampling_rate = 0.0f)
{
	return std::make_unique<SkinnedMesh>(device, filename, triangulate, sampling_rate);
}

/// <summary>
/// �L�[�t���[���Ǘ��N���X���쐬���܂��B
/// </summary>
/// <param name="mesh">�X�L���h���b�V���̃V�F�A�[�h�|�C���^</param>
inline std::unique_ptr<Keyframe> makeKeyframe(std::shared_ptr<SkinnedMesh> mesh)
{
	return std::make_unique<Keyframe>(mesh);
}

/// <summary>
/// �R���X�^���g�o�b�t�@���쐬���܂��B
/// </summary>
/// <typeparam name="T">�R���X�^���g�o�b�t�@�ɓn���^</typeparam>
/// <typeparam name="...Args"></typeparam>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="...args">�R���X�^���g�o�b�t�@���ŊǗ�����f�[�^�̃R���X�g���N�^�ɓn���f�[�^</param>
template <class T,class... Args>
inline std::unique_ptr<ConstantBuffer<T>> makeConstantBuffer(ID3D11Device* device, Args... args)
{
	return std::make_unique<ConstantBuffer<T>>(device, args...);
}

/// <summary>
/// �V�[���R���X�^���g���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
inline std::unique_ptr<SceneConstant> makeSceneConstant(ID3D11Device* device)
{
	return std::make_unique<SceneConstant>(device);
}

/// <summary>
/// �W�I���g���v���~�e�B�u���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="config">�v���~�e�B�u�̍\��</param>
inline std::unique_ptr<GeometryPrimitive> makeGeometryPrimitive(ID3D11Device* device, GP_CONFIG config)
{
	return std::make_unique<GeometryPrimitive>(device, config);
}

/// <summary>
/// �t���[���o�b�t�@���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="config">�t���[���o�b�t�@�̍\��</param>
inline std::unique_ptr<FrameBuffer> makeFrameBuffer(ID3D11Device* device,FB_CONFIG config)
{
	return std::make_unique<FrameBuffer>(device, config);
}

/// <summary>
/// �W�I���g���o�b�t�@���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="config">�W�I���g���o�b�t�@�̍\��</param>
inline std::unique_ptr<GeometryBuffer> makeGeometryBuffer(ID3D11Device* device, GB_CONFIG config)
{
	return std::make_unique<GeometryBuffer>(device, config);
}

/// <summary>
/// �y�C���^�[���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="ps_filename">�s�N�Z���V�F�[�_�[(cso)�̃A�h���X</param>
inline std::unique_ptr<Peinter> makePeinter(ID3D11Device* device, const char* ps_filename = NULL)
{
	return std::make_unique<Peinter>(device, ps_filename);
}

/// <summary>
/// �K�E�V�A���t�B���^�[���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="w">�r���[�̉���</param>
/// <param name="h">�r���[�̏c��</param>
/// <param name="format">�V�F�[�_�[���\�[�X�r���[�̃t�H�[�}�b�g</param>
inline std::unique_ptr<GaussianFilter> makeGaussianFilter(ID3D11Device* device,
	UINT w, UINT h,
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
{
	return std::make_unique<GaussianFilter>(device, w, h, format);
}

/// <summary>
/// �u���[�������_���[���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="w">�r���[�̉���</param>
/// <param name="h">�r���[�̏c��</param>
/// <param name="shrink">�u���[�o�b�t�@�[�̏k���W��</param>
/// <param name="format">�V�F�[�_�[���\�[�X�r���[�̃t�H�[�}�b�g</param>
inline std::unique_ptr<BloomRenderer> makeBloomRenderer(ID3D11Device* device,
	UINT w, UINT h,
	UINT shrink = 8,
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
{
	if (shrink == 0)shrink = 1;
	return std::make_unique<BloomRenderer>(device, w, h, shrink, format);
}

/// <summary>
///
/// </summary>
/// <param name="device"></param>
/// <param name="max_sprites"></param>
/// <param name="file_name"></param>
/// <returns></returns>
inline std::unique_ptr<Particle> makeParticle(ID3D11Device* device,
	size_t max_sprites, const wchar_t* file_name = L"\0")
{
	return std::make_unique<Particle>(device, file_name, max_sprites);
}

/// <summary>
/// �s�N�Z���V�F�[�_�[���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="cso_filename">�s�N�Z���V�F�[�_�[(cso)�̃A�h���X</param>
inline Microsoft::WRL::ComPtr<ID3D11PixelShader> makePixelShader(ID3D11Device* device, std::string cso_filename)
{
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	loadPixelShader(device, cso_filename, pixel_shader.ReleaseAndGetAddressOf());
	return pixel_shader;
}

/// <summary>
/// �W�I���g���V�F�[�_�[���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="cso_filename">�W�I���g���V�F�[�_�[(cso)�̃A�h���X</param>
inline Microsoft::WRL::ComPtr<ID3D11GeometryShader> makeGeometryShader(ID3D11Device* device, std::string cso_filename)
{
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	loadGeometryShader(device, cso_filename, geometry_shader.ReleaseAndGetAddressOf());
	return geometry_shader;
}

/// <summary>
/// �h���C���V�F�[�_�[���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="cso_filename">�h���C���V�F�[�_�[(cso)�̃A�h���X</param>
inline Microsoft::WRL::ComPtr<ID3D11DomainShader> makeDomainShader(ID3D11Device* device, std::string cso_filename)
{
	Microsoft::WRL::ComPtr<ID3D11DomainShader> domain_shader;
	loadDomainShader(device, cso_filename, domain_shader.ReleaseAndGetAddressOf());
	return domain_shader;
}

/// <summary>
/// �n���V�F�[�_�[���쐬���܂��B
/// </summary>
/// <param name="device">�L���ȃf�o�C�X</param>
/// <param name="cso_filename">�n���V�F�[�_�[(cso)�̃A�h���X</param>
inline Microsoft::WRL::ComPtr<ID3D11HullShader> makeHullShader(ID3D11Device* device, std::string cso_filename)
{
	Microsoft::WRL::ComPtr<ID3D11HullShader> hull_shader;
	loadHullShader(device, cso_filename, hull_shader.ReleaseAndGetAddressOf());
	return hull_shader;
}

template <class T>
using MyInterface			= std::shared_ptr <T>;
template <class T>
using MyComInterface		= Microsoft::WRL::ComPtr<T>;

using ISound				= MyInterface<CXAudio>;
using ISprite				= MyInterface<Sprite>;
using ISpriteBatch			= MyInterface<SpriteBatch>;
using ISkinnedMesh			= MyInterface<SkinnedMesh>;
using IKeyframe				= MyInterface<Keyframe>;
template <class T>
using IConstantBuffer		= MyInterface<ConstantBuffer<T>>;
using ISceneConstant		= MyInterface<SceneConstant>;
using IGeometryPrimitive	= MyInterface<GeometryPrimitive>;
using IFrameBuffer			= MyInterface<FrameBuffer>;
using IGeometryBuffer		= MyInterface<GeometryBuffer>;
using IPeinter				= MyInterface<Peinter>;
using IGaussianFilter		= MyInterface<GaussianFilter>;
using IBloomRenderer		= MyInterface<BloomRenderer>;
using IParticle				= MyInterface<Particle>;
using IShaderResourceView	= MyComInterface<ID3D11ShaderResourceView>;
using IPixelShader			= MyComInterface<ID3D11PixelShader>;
using IGeometryShader		= MyComInterface<ID3D11GeometryShader>;
using IDomainShader			= MyComInterface<ID3D11DomainShader>;
using IHullShader			= MyComInterface<ID3D11HullShader>;