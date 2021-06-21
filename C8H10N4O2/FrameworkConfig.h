#pragma once

#ifdef _M_IX86
static_assert(false,"Not compatible with x86 version");
#endif // _M_IX86

//�h��Ԃ��F
static CONST FLOAT		FILL_COLOR[4]{ 0.2f, 0.2f, 0.6f, 1.0f };
//�E�B���h�E�̕�
static CONST LONG		SCREEN_WIDTH{ 1280 };
//�E�B���h�E�̍���
static CONST LONG		SCREEN_HEIGHT{ 720 };
//�t���X�N���[���t���O
static CONST BOOL		FULLSCREEN{ FALSE };
//���o���ɕ\�����镶����
static CONST LPWSTR		APPLICATION_NAME{ L"DirectX11" };
//�X���b�v�`�F�C���̃o�b�N�o�b�t�@�[�֐؂�ւ���^�C�~���O
static CONST UINT		SYNC_INTERVAL{ FALSE };
//�V�F�[�_�[��CSO�t�@�C�����u����Ă���f�B���N�g���[�̃p�X
static CONST CHAR*		CSO_FILE_DIRECTORY{"data\\shader\\"};
//ImGui���g�p���邱�Ƃ��R���p�C���[�ɓ`�B
#define USE_IMGUI		TRUE
//ImGui�̃t�H���g�T�C�Y
#define IMGUI_FONT_SIZE	14