#pragma once
#include "Arithmetic.h"
#include "ConstantBuffer.h"
class SceneConstant
{
private:
	struct data
	{
		FLOAT4X4	view_projection;
		FLOAT4		light_direction;
		FLOAT4		camera_position;
	};

	ConstantBuffer<data> constant_buffer;
	FLOAT4X4	view;
	FLOAT4X4	projection;
public:
	SceneConstant(ID3D11Device* device);
	virtual ~SceneConstant() = default;
	/// <summary>
	/// ���C�g�̌�����ݒ肵�܂��B
	/// </summary>
	/// <param name="light_direction">���C�g�̌���</param>
	void setLightDirection(FLOAT4 light_direction) { constant_buffer.data.light_direction = light_direction; }
	/// <summary>
	/// �r���[�s���ݒ肵�܂��B
	/// </summary>
	/// <param name="view">�r���[�s��</param>
	void setView(FLOAT4X4 view) { this->view = view; }
	/// <summary>
	/// �v���W�F�N�V�����s���ݒ肵�܂��B
	/// </summary>
	/// <param name="projection">�v���W�F�N�V�����s��</param>
	void setProjection(FLOAT4X4 projection) { this->projection = projection; }
	/// <summary>
	/// �J�����̍��W��ݒ肵�܂��B
	/// </summary>
	/// <param name="camera_position">�J�����̍��W</param>
	void setCameraPosition(FLOAT4 camera_position) { constant_buffer.data.camera_position = camera_position; }

	/// <summary>
	/// ���C�g�̌������擾���܂��B
	/// </summary>
	const FLOAT4& getLightDirection()const { return constant_buffer.data.light_direction; }
	/// <summary>
	/// �r���[�s����擾���܂��B
	/// </summary>
	const FLOAT4X4& getView()const { return view; }

	/// <summary>
	/// �v���W�F�N�V�����s����擾���܂��B
	/// </summary>
	const FLOAT4X4& getProjection()const { return projection; }
	/// <summary>
	/// �J�����̍��W���擾���܂��B
	/// </summary>
	/// <returns></returns>
	const FLOAT4& setCameraPosition()const { return constant_buffer.data.camera_position; }


	/// <summary>
	/// �V�[���f�[�^���V�F�[�_�[�ɑ��M���܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	void send(ID3D11DeviceContext* immediate_context);

};



