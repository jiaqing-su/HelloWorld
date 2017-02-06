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

/* ��־�ȼ�
TRACE
	TRACE����Ҫʹ��Ŀ�������ߵ��ԡ�
	�ü�����־��Ĭ������£��Ȳ���ӡ���ն�Ҳ��������ļ�����ʱ���Գ�������Ч�ʼ���������Ӱ�졣
	�ڳ������й����У������Ҫ�鿴TRACE������־����Ҫͨ��elog�����TRACE���߽�������־���������TRACE��
	.
DEBUG
	DEUG����Ҫʹ��Ŀ�����ն˲鿴�����ߵ��ԡ�
	�ü�����־��Ĭ������»��ӡ���ն���������ǲ���鵵����־�ļ�����ˣ�һ�����ڿ������ڳ���ǰ���������ϣ��鿴��־��ˮ��Ϣ��
	�ڳ��������������м��� ��-nostdout�� ,���ر�����ǰ�˴�ӡ�������Ҫ���DEBUG������־����
	.
INFO
	INFO����Ҫʹ��Ŀ�ı��������Ⱥ�ת̬��Ϣ��һ��������Ϣ����һ���Եģ��������������������磺�������ÿ�ɹ��󣬿��Դ�ӡһ������ɹ�����Ϣ�����ڸ��ٳ����չ��Ϣ��
	�ü�����־��Ĭ������»�ͬʱ��ӡ���ն˺͹鵵����־�ļ�����ͬ����
	.
WARNING
	������Ϣ�������������������Ƿ����ݻ���ĳ�ֿ��ܵĴ��󣬸ô�����һ���Եġ��ɻָ��ģ�����Ӱ�����������У������Դ�������״̬��
	.
ERROR
	�ô������������Ȼ�������У����Ǽ��п���������ĳ�ַ�������״̬�£������޷����ȫ���ȶ��Ĺ��ܡ�
	.
FATAL
	�����Ĵ��󣬱������������������Ĵ��󣬱���������ֹ���С�
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
