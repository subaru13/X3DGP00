#pragma once
#include <crtdbg.h>
#include <string>
#include <stdio.h>

/// <summary>
/// �t�@�C�������݂��邩���肵�܂��B
/// </summary>
/// <param name="file_name">�t�@�C���p�X</param>
/// <returns>���݂����true</returns>
inline bool isExistFileW(const std::wstring& file_name)
{
	if (file_name.empty())return false;
	FILE* fp = nullptr;
	_wfopen_s(&fp, file_name.c_str(), L"rb");
	bool result = (fp != nullptr);
	if (fp)fclose(fp);
	return result;
}

/// <summary>
/// �t�@�C�������݂��邩���肵�܂��B
/// </summary>
/// <param name="file_name">�t�@�C���p�X</param>
/// <returns>���݂����true</returns>
inline bool isExistFileA(const std::string& file_name)
{
	if (file_name.empty())return false;
	FILE* fp = nullptr;
	fopen_s(&fp, file_name.c_str(), "rb");
	bool result = (fp != nullptr);
	if (fp)fclose(fp);
	return result;
}

/// <summary>
/// �t�@�C���l�[���ƃf�B���N�g���[���������܂��B
/// </summary>
/// <param name="directory">�f�B���N�g���[</param>
/// <param name="name">�t�@�C���l�[��</param>
/// <returns>���������t�@�C���p�X</returns>
inline std::wstring combinePathsW(const std::wstring& directory, const std::wstring& name)
{
	std::wstring result = directory;
	if (!result.empty())
	{
		wchar_t back = result.back();
		if (back != '\\' && back != '/')
		{
			result += '\\';
		}
	}
	return result + name;
}

/// <summary>
/// �t�@�C���l�[���ƃf�B���N�g���[���������܂��B
/// </summary>
/// <param name="directory">�f�B���N�g���[</param>
/// <param name="name">�t�@�C���l�[��</param>
/// <returns>���������t�@�C���p�X</returns>
inline std::string combinePathsA(const std::string& directory, const std::string& name)
{
	std::string result = directory;
	if (!result.empty())
	{
		char back = result.back();
		if (back != '\\' && back != '/')
		{
			result += '\\';
		}
	}
	return result + name;
}

inline void rotate(float& x, float& y, float cx, float cy, float angle)
{
	x -= cx;
	y -= cy;

	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	float tx{ x }, ty{ y };
	x = cos * tx + -sin * ty;
	y = sin * tx + cos * ty;

	x += cx;
	y += cy;
};

#ifdef _DEBUG
#define NEW							new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else //!_DEBUG
#define NEW							new
#endif //_DEBUG

#define SAFE_DELETE(pointer)		if(pointer!=NULL){ delete pointer;pointer = NULL; }

#define SAFE_DELETE_ARRAY(pointer)	if(pointer!=NULL){ delete[] pointer;pointer = NULL; }

template<typename T>
class Singleton
{
public:
	static T* getInstance()
	{
		static T instance{};
		return &instance;
	}
};

#define SINGLETON_CLASS(type)		class type : public Singleton<type>

#define GET_INSTANCE(type)			type::getInstance()

#define SWAP_VARIABLE(a,b)			{auto __storage = a; a = b; b = __storage;}