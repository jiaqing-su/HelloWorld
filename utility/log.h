#ifndef _LOG_H_
#define _LOG_H_

#define LOG_OPT_LEVEL     0x01
#define LOG_OPT_TIMESTAM  0x02
#define LOG_OPT_FILELINE  0x04
#define LOG_OPT_FUNCTION  0x08
#define LOG_OPT_SYSERROR  0x10

#define LOG_OUT_CONSOLE   0x01
#define LOG_OUT_FILE      0x02
#define LOG_OUT_VS_DEBUG  0x04

#define LOG_LEVEL_FATAL   0x01
#define LOG_LEVEL_ERROR   0x02
#define LOG_LEVEL_WARNING 0x04
#define LOG_LEVEL_INFO    0x08
#define LOG_LEVEL_DEBUG   0x10
#define LOG_LEVEL_TRACE   0x20

/* 日志等级
TRACE
	TRACE的主要使用目的是在线调试。
	该级别日志，默认情况下，既不打印到终端也不输出到文件。此时，对程序运行效率几乎不产生影响。
	在程序运行过程中，如果需要查看TRACE级别日志，需要通过elog命令开启TRACE或者将程序日志输出级别降至TRACE。
	.
DEBUG
	DEUG的主要使用目的是终端查看和在线调试。
	该级别日志，默认情况下会打印到终端输出，但是不会归档到日志文件。因此，一般用于开发者在程序当前启动窗口上，查看日志流水信息。
	在程序启动命令行中加入 “-nostdout” ,将关闭所有前端打印输出（主要针对DEBUG级别日志）。
	.
INFO
	INFO的主要使用目的报告程序进度和转态信息。一般这种信息都是一过性的，不会大量反复输出。例如：连接商用库成功后，可以打印一条连库成功的信息，便于跟踪程序进展信息。
	该级别日志，默认情况下会同时打印到终端和归档到日志文件（下同）。
	.
WARNING
	警告信息，表明程序处理中遇到非法数据或者某种可能的错误，该错误是一过性的、可恢复的，不会影响程序继续运行，程序仍处在正常状态。
	.
ERROR
	该错误发生后程序仍然可以运行，但是极有可能运行在某种非正常的状态下，导致无法完成全部既定的功能。
	.
FATAL
	致命的错误，表明程序遇到了致命的错误，必须马上终止运行。
*/

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

void set_log_opt_mask(unsigned int mask);/* LOG_OPT_* */
void set_log_level_mask(unsigned int mask);/* LOG_LEVEL_* */
void set_log_out_mask(unsigned int mask);/* LOG_OUT_* */
void set_log_file_path(const char* path);
void set_log_file_name(const char* name);

void log_print(int level, const char* file, int line, const char* func, const char* format, ...);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#define log_out_(level, format, ...)\
	log_print(level, __FILE__, __LINE__, __FUNCTION__, format,  ##__VA_ARGS__)

#ifndef DISABLE_LOG
#define log_fatal(format,...)\
	log_out_(LOG_LEVEL_FATAL, format, ##__VA_ARGS__);

#define log_error(format,...)\
	log_out_(LOG_LEVEL_ERROR, format, ##__VA_ARGS__);

#define log_warning(format,...)\
	log_out_(LOG_LEVEL_WARNING, format, ##__VA_ARGS__);

#define log_info(format,...)\
	log_out_(LOG_LEVEL_INFO, format, ##__VA_ARGS__);

#define log_debug(format,...)\
	log_out_(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__);

#define log_trace(format,...)\
	log_out_(LOG_LEVEL_TRACE, format, ##__VA_ARGS__);
#else
#define log_fatal(format,...)
#define log_error(format,...)
#define log_warning(format,...)
#define log_info(format,...)
#define log_debug(format,...)
#define log_trace(format,...)
#endif /*DISABLE_LOG*/

#endif/*_LOG_H_*/
