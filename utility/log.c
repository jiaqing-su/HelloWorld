
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdarg.h>
#include <errno.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <pthread.h>

#define CRITICAL_SECTION             pthread_mutex_t
#define InitializeCriticalSection(X) pthread_mutex_init(X, NULL)
#define EnterCriticalSection(X)      pthread_mutex_lock(X)
#define LeaveCriticalSection(X)      pthread_mutex_unlock(X)

#define FOREGROUND_RED    0x0001
#define FOREGROUND_GREEN  0x0002
#define FOREGROUND_BLUE   0x0004
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

static unsigned int opt_mask = LOG_OPT_LEVEL|LOG_OPT_TIMESTAM|LOG_OPT_FUNCTION|LOG_OPT_SYSERROR;
static unsigned int lvl_mask = 0xFFFF;
static unsigned int out_mask = 0xFFFF;
static const char lv_name[][16] = {"[FATAL] ","[ERROR] ","[WARN ] ","[INFO ] ","[DEBUG] ","[TRACE] "};
static char log_file_path[MAX_PATH] = "";
static char log_file_name[MAX_PATH] = "";

static int mutex_init = 0;
static CRITICAL_SECTION mtx;

void set_log_opt_mask(unsigned int mask)
{
    opt_mask = mask;
}

void set_log_level_mask(unsigned int mask)
{
    lvl_mask = mask;
}

void set_log_out_mask(unsigned int mask)
{
    out_mask = mask;
}

void set_log_file_path(const char* path)
{
	if(path != NULL && strlen(log_file_path) == 0)
		strcpy(log_file_path, path);
}

void set_log_file_name(const char* name)
{
	if(name != NULL && strlen(log_file_name) == 0)
		strcpy(log_file_name, name);
}

static void log_out_to_console(const char* log_buf, int level)
{
    int highlight = 0;
    unsigned short wColor = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE;
    switch(level)
    {
    case LOG_LEVEL_FATAL:
        wColor = FOREGROUND_RED;
        highlight = 1;
        break;
    case LOG_LEVEL_ERROR:
        wColor = FOREGROUND_RED;
        break;
    case LOG_LEVEL_WARNING:
        wColor = FOREGROUND_RED|FOREGROUND_GREEN;
        break;
    case LOG_LEVEL_INFO:
        break;
    case LOG_LEVEL_DEBUG:
        wColor = FOREGROUND_RED|FOREGROUND_BLUE;
        break;
    case LOG_LEVEL_TRACE:
        wColor = FOREGROUND_BLUE;
        break;
    default:
        break;
    }
#ifdef WIN32
    if(highlight)
        wColor |= FOREGROUND_INTENSITY;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
    printf("%s", log_buf);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
#else
    wColor += 30;
    printf("\x1b[%d;%dm%s\x1b[%dm", 40, wColor, log_buf, highlight);
    printf("\x1b[%dm", 0);
#endif
}

static void log_out_to_file(const char* log_buf, time_t tt)
{
	struct tm* tm_time = localtime(&tt);
	char* p1 = NULL;
	char* p2 = NULL;
	char* tail = NULL;
	char exe_file_path[MAX_PATH] = "";
#ifdef WIN32
	GetModuleFileNameA(NULL, exe_file_path, MAX_PATH);
#else
	readlink( "/proc/self/exe", exe_file_path,  MAX_PATH);
#endif
	
	p1 = strrchr(exe_file_path, '\\');
	p2 = strrchr(exe_file_path, '/');
	tail = max(p1, p2);

	if(strlen(log_file_path) == 0)
	{
		if(tail != NULL)
			strncpy(log_file_path, exe_file_path, tail - exe_file_path);
	}

	if(strlen(log_file_name) == 0)
	{
		if(tail != NULL)
		{
			char* dot = strchr(tail, '.');
			if(dot != NULL)
				*dot = 0;
			strcpy(log_file_name, tail+1);
		}
		else
		{
			strcpy(log_file_name, exe_file_path);
		}
	}

    sprintf(exe_file_path, "%s/log/%s_%04d%02d%02d.log", 
		log_file_path, 
		log_file_name, 
		tm_time->tm_year+1900, 
		tm_time->tm_mon+1, 
		tm_time->tm_mday);

    if(strlen(exe_file_path) > 0)
    {
        FILE* fp = fopen(exe_file_path, "a");
        if(fp)
        {
            fwrite(log_buf, strlen(log_buf), 1, fp);
            fclose(fp);
        }
    }
}

void log_print(int level, const char* file, int line, const char* func, const char* format, ...)
{
	char log_buf[2048] = {0};
	struct timeb tb;

    if(!(level&lvl_mask))
        return;

    if(opt_mask&LOG_OPT_LEVEL)
    {
        int i = 0;
        for (; i < 6; i++)
        {
            if(level&(1<<i))
            {
                strcat(log_buf, lv_name[i]);
                break;
            }
        }
    }

    if(opt_mask&LOG_OPT_TIMESTAM)
    {
		char* time_buf = log_buf + strlen(log_buf);
		struct tm* ptm = NULL;
		ftime(&tb);
        ptm = localtime(&tb.time);
        sprintf(time_buf, "%04d-%02d-%02d %02d:%02d:%02d:%04d ",
                ptm->tm_year+1900,
                ptm->tm_mon+1,
                ptm->tm_mday,
                ptm->tm_hour,
                ptm->tm_min,
                ptm->tm_sec,
				tb.millitm);
    }

    if(opt_mask&LOG_OPT_FILELINE && (level&(LOG_LEVEL_FATAL|LOG_LEVEL_ERROR|LOG_LEVEL_DEBUG|LOG_LEVEL_TRACE)))
    {
        char* line_buf = log_buf + strlen(log_buf);
        const char* p1 = strrchr(file, '\\');
        const char* p2 = strrchr(file, '/');
        const char* p  = max(p1, p2) + 1;
		if(p == NULL)
			p = file;
        sprintf(line_buf, "%s[%d]|", p, line);
    }

    if(opt_mask&LOG_OPT_FUNCTION && (level&(LOG_LEVEL_FATAL|LOG_LEVEL_ERROR|LOG_LEVEL_DEBUG|LOG_LEVEL_TRACE)))
    {
        char* func_buf = log_buf + strlen(log_buf);
        sprintf(func_buf, "%s|", func);
    }

    /*user msg*/
    {
		char* user_msg = log_buf + strlen(log_buf);
        va_list args;
        va_start( args, format );
        vsprintf(user_msg, format, args);
        va_end(args);
        strcat(log_buf, ".");
    }

    if(opt_mask&LOG_OPT_SYSERROR && level&(LOG_LEVEL_FATAL|LOG_LEVEL_ERROR))
    {
#ifdef WIN32
        LPVOID lpMsgBuf;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR) &lpMsgBuf,
            0, NULL );
        strcat(log_buf,(LPSTR)lpMsgBuf);
        LocalFree(lpMsgBuf);
		if(out_mask&LOG_OUT_VS_DEBUG)
			OutputDebugStringA(log_buf);
#else
        char * msg = strerror(errno);
        strcat(log_buf, msg);
        strcat(log_buf, "\r\n");
#endif
    }
    else
    {
        strcat(log_buf, "\r\n");
    }

	if (!mutex_init)
		InitializeCriticalSection(&mtx);

	EnterCriticalSection(&mtx);
    if(out_mask&LOG_OUT_CONSOLE)
    {
        log_out_to_console(log_buf, level);
    }

    if(out_mask&LOG_OUT_FILE)
    {
        log_out_to_file(log_buf, tb.time);
    }
	LeaveCriticalSection(&mtx);
}

