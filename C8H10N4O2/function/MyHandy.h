#pragma once
#include <string>
#include <stdio.h>

/// <summary>
/// ファイルが存在するか判定します。
/// </summary>
/// <param name="file_name">ファイルパス</param>
/// <returns>存在すればtrue</returns>
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
/// ファイルが存在するか判定します。
/// </summary>
/// <param name="file_name">ファイルパス</param>
/// <returns>存在すればtrue</returns>
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
/// ファイルネームとディレクトリーを結合します。
/// </summary>
/// <param name="directory">ディレクトリー</param>
/// <param name="name">ファイルネーム</param>
/// <returns>結合したファイルパス</returns>
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
/// ファイルネームとディレクトリーを結合します。
/// </summary>
/// <param name="directory">ディレクトリー</param>
/// <param name="name">ファイルネーム</param>
/// <returns>結合したファイルパス</returns>
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