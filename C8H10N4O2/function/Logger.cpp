#define	_CRT_SECURE_NO_WARNINGS
#include "Logger.h"
#include <fstream>
#include <Windows.h>
#include <memory>
#include <time.h>

using __ofs_a_ = std::ofstream;
using std::ios_base;
using std::string;
using std::endl;

#define __LOG_FILE_NAME_A_	"log.txt"
#define __FORMAT_STRING(x)	char x##[1024]
#define __CFORMAT_STRING(x)	const char x##[1024]
#define __SEND_STRING(x)	char x##[1025]
#define __DATE_STRING(x)	char x##[64]
#define __ERROR_STRING(x)	LPSTR x
#define __OFS_(x)			__ofs_a_ x ## {__Logger_::__log_file,ios_base::app}
#define __EREER_(x)			(x <= 0)
#define __CAN_NOT_USE_(x)	(x == nullptr)

#define __FORMAT_A_(_format,_string)\
{\
	va_list args;\
	va_start(args, _string);\
	if (__EREER_(vsnprintf_s(_format, sizeof(_format), _string, args)))return;\
	va_end(args);\
}

#define __STRFTIMER_A_(date)\
{\
	time_t t = time(NULL);\
	strftime(date, sizeof(date), "%Y/%m/%d %a %H:%M:%S\n", localtime(&t));\
}

#define __ERROR_MSG_A_(msg,_error_code)\
{\
	FormatMessageA(\
	FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,\
	NULL, _error_code,\
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),\
	reinterpret_cast<LPSTR>(&msg), NULL, NULL);\
}

inline char* __file_name_(const char* __file)
{
	string path{ __file };
	size_t path_i = path.find_last_of("\\") + 1;
	__FORMAT_STRING(__out_str) = { "\0" };
	return strcpy(__out_str, path.substr(path_i).c_str());
}

#define __WARNING_FORMAT_A_(__file,__line) sprintf_s(format, sizeof(format), "warning!!\t%s\t%d\t", __file_name_(__file), __line)

char __Logger_::__log_file[__Logger_::__max_file_name] = { __LOG_FILE_NAME_A_ };


void __Logger_::__timestamp_()
{
	__DATE_STRING(date);
	__STRFTIMER_A_(date);
	OutputDebugStringA(date);
}

void __Logger_::__log_(const char* string, ...)
{
	if (__CAN_NOT_USE_(string))return;
	__FORMAT_STRING(format) = { "\0" };
	__FORMAT_A_(format, string);
	OutputDebugStringA(format);
}

void __Logger_::__warning_log_(const char* __file, int __line, const char* string, ...)
{
	__FORMAT_STRING(format) = { "\0" };
	__FORMAT_STRING(name) = { "\0" };
	if (__EREER_(__WARNING_FORMAT_A_(__file, __line)))return;
	OutputDebugStringA(format);
	if (__CAN_NOT_USE_(string))
	{
		OutputDebugStringA("\n");
		return;
	}
	__FORMAT_A_(format, string);
	OutputDebugStringA(format);
}

void __Logger_::__trace_log_(long error_code)
{
	__ERROR_STRING(format);
	__ERROR_MSG_A_(format, error_code);
	OutputDebugStringA(format);
}

void __Logger_::__set_log_name(const char* new_name)
{
	if (!__CAN_NOT_USE_(new_name))strcpy(__Logger_::__log_file, new_name);
}

void __Logger_::__timestamp_f_()
{
	__OFS_(_ofs);
	if (_ofs)
	{
		__DATE_STRING(date);
		__STRFTIMER_A_(date);
		_ofs << date ;
		_ofs.close();
	}
}


void __Logger_::__log_f_(const char* string, ...)
{
	if (__CAN_NOT_USE_(string))return;
	__OFS_(_ofs);
	if (_ofs)
	{
		__FORMAT_STRING(format) = { "\0" };
		__FORMAT_A_(format, string);
		_ofs << format << endl;
		_ofs.close();
	}
}



void __Logger_::__warning_log_f_(const char* __file, int __line, const char* string, ...)
{
	__OFS_(_ofs);
	if (_ofs)
	{
		__FORMAT_STRING(format) = { "\0" };
		__FORMAT_STRING(name) = { "\0" };
		if (__EREER_(__WARNING_FORMAT_A_(__file, __line)))
		{
			_ofs.close();
			return;
		}
		_ofs << format;
		if (__CAN_NOT_USE_(string))
		{
			_ofs << endl;
			_ofs.close();
			return;
		}
		__FORMAT_A_(format, string);
		_ofs << format << endl;
		_ofs.close();
	}
}

void __Logger_::__trace_log_f_(long error_code)
{
	__OFS_(_ofs);
	if (_ofs)
	{
		__ERROR_STRING(format);
		__ERROR_MSG_A_(format, error_code);
		_ofs << format;
		_ofs.close();
	}
}


class __console_
{
public:
	~__console_()
	{
		if (fp)fclose(fp);
		if (hwnd != NULL)
		{
			HMENU hmenu = GetSystemMenu(hwnd, TRUE);
			RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
			ShowWindow(hwnd, SW_HIDE);
		}
		FreeConsole();
	}

	static std::unique_ptr<__console_> _make_console()
	{
		return std::unique_ptr<__console_>(new __console_);
	}

	bool _is()const { return fp != NULL; }

private:
	__console_() :fp(NULL), hwnd(NULL)
	{
		if (!AllocConsole())return;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONIN$", "r", stdin);
		hwnd = GetConsoleWindow();
		if (hwnd != NULL)
		{
			HMENU hmenu = GetSystemMenu(hwnd, FALSE);
			RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
		}
	}
	FILE*	fp;
	HWND	hwnd;
};

static std::unique_ptr<__console_> _console_ = nullptr;

#define __MAKE_CONSOLE_ \
if (_console_ == nullptr&&GetConsoleWindow()==NULL)\
{\
	_console_ = std::move(__console_::_make_console());\
	if (!_console_->_is()) { _console_.reset(); return; }\
}


void __Logger_::__timestamp_c_()
{
	__MAKE_CONSOLE_;
	__DATE_STRING(date);
	__STRFTIMER_A_(date);
	printf(date);
}

void __Logger_::__log_c_(const char* string, ...)
{
	if (__CAN_NOT_USE_(string))return;
	__MAKE_CONSOLE_;
	__FORMAT_STRING(format) = { "\0" };
	__FORMAT_A_(format, string);
	printf(format);
}

void __Logger_::__warning_log_c_(const char* __file, int __line, const char* string, ...)
{
	__MAKE_CONSOLE_;
	__FORMAT_STRING(format) = { "\0" };
	__FORMAT_STRING(name) = { "\0" };
	if (__EREER_(__WARNING_FORMAT_A_(__file, __line)))return;
	printf(format);
	if (__CAN_NOT_USE_(string)) { printf("\n"); return; }
	__FORMAT_A_(format, string);
	printf(format);
}

void __Logger_::__trace_log_c_(long error_code)
{
	__MAKE_CONSOLE_;
	__ERROR_STRING(format);
	__ERROR_MSG_A_(format, error_code);
	printf(format);
}