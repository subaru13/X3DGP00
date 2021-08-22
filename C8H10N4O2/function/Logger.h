#pragma once

/*
	@ Logger class
*/
class __Logger_ final
{
private:
	__Logger_() = delete;
	~__Logger_() = delete;
	static constexpr int __max_file_name = 256;
	static char __log_file[__max_file_name];
public:
	static void __timestamp_();
	static void __log_(const char*, ...);
	static void __warning_log_(const char*, int, const char*, ...);
	static void __trace_log_(long);
	static void __power_info_();
	static void __display_info_();
	static void __memory_info_();

	static void __set_log_name(const char*);
	static void __timestamp_f_();
	static void __log_f_(const char*, ...);
	static void __warning_log_f_(const char*, int, const char*, ...);
	static void __trace_log_f_(long);
	static void __power_info_f_();
	static void __display_info_f_();
	static void __memory_info_f_();

	static void __timestamp_c_();
	static void __log_c_(const char*, ...);
	static void __warning_log_c_(const char*, int, const char*, ...);
	static void __trace_log_c_(long);
	static void __power_info_c_();
	static void __display_info_c_();
	static void __memory_info_c_();
};

/*
	@ Logger macro
*/

#define TIMESTAMP			{__Logger_::__timestamp_();}
#define LOG(...)			{__Logger_::__log_(__VA_ARGS__);}
#define WARNING(...)		{__Logger_::__warning_log_(__FILE__,__LINE__,__VA_ARGS__);}
#define TRACE_LOG(err)		{__Logger_::__trace_log_(err);}
#define POWER_INFO			{__Logger_::__power_info_();}
#define DISPLAY_INFO		{__Logger_::__display_info_();}
#define MEMORY_INFO			{__Logger_::__memory_info_();}

#define SET_NAME(name)		{__Logger_::__set_log_name(name);}
#define TIMESTAMP_F			{__Logger_::__timestamp_f_();}
#define LOG_F(...)			{__Logger_::__log_f_(__VA_ARGS__);}
#define WARNING_F(...)		{__Logger_::__warning_log_f_(__FILE__,__LINE__,__VA_ARGS__);}
#define TRACE_LOG_F(err)	{__Logger_::__trace_log_f_(err);}
#define POWER_INFO_F		{__Logger_::__power_info_f_();}
#define DISPLAY_INFO_F		{__Logger_::__display_info_f_();}
#define MEMORY_INFO_F		{__Logger_::__memory_info_f_();}

#define TIMESTAMP_C			{__Logger_::__timestamp_c_();}
#define LOG_C(...)			{__Logger_::__log_c_(__VA_ARGS__);}
#define WARNING_C(...)		{__Logger_::__warning_log_c_(__FILE__,__LINE__,__VA_ARGS__);}
#define TRACE_LOG_C(err)	{__Logger_::__trace_log_c_(err);}
#define POWER_INFO_C		{__Logger_::__power_info_c_();}
#define DISPLAY_INFO_C		{__Logger_::__display_info_c_();}
#define MEMORY_INFO_C		{__Logger_::__memory_info_c_();}

#ifdef _DEBUG
#define TIMESTAMP_D			{TIMESTAMP}
#define LOG_D(...)			{LOG(__VA_ARGS__)}
#define WARNING_D(...)		{WARNING(__VA_ARGS__)}
#define TRACE_LOG_D(err)	{TRACE_LOG(err)}

#define TIMESTAMP_DC		{TIMESTAMP_C}
#define LOG_DC(...)			{LOG_C(__VA_ARGS__)}
#define WARNING_DC(...)		{WARNING_C(__VA_ARGS__)}
#define TRACE_LOG_DC(err)	{TRACE_LOG_C(err)}
#else
#define TIMESTAMP_D			{}
#define LOG_D(...)			{}
#define WARNING_D(...)		{}
#define TRACE_LOG_D(err)	{}

#define TIMESTAMP_DC		{}
#define LOG_DC(...)			{}
#define WARNING_DC(...)		{}
#define TRACE_LOG_DC(err)	{}
#endif

