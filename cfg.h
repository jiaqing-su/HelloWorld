
#ifndef _CFG_OP_H_
#define _CFG_OP_H_

typedef void* CFG_HANDLE;

CFG_HANDLE cfg_create(const char*);
const char* cfg_get_str(const char*, const char*, CFG_HANDLE);
void cfg_release(CFG_HANDLE);

/*
like
GetPrivateProfileString
*/
unsigned int get_cfg_str_file(const char*, const char*, const char*, char*, unsigned int, const char*);

#endif/*_CFG_OP_H_*/