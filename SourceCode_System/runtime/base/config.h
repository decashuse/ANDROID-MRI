
#ifndef __HOOKA_CONFIG__
#define __HOOKA_CONFIG__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <fcntl.h>
//#include <async_safe/log.h>
#include <android/log.h>



int check_target_method(const char* strMethodName);
bool check_target_search_item(const char* strMethodName);
int get_target_uid();
int get_trace_mode();
int hooka_art(const char* strClassName);
int LoadHOOKAConfig();
int is_display_stack_trace();
int is_display_code_dump();
int is_display_args();
int is_display_instruction();
bool is_target_uid();
bool is_search_item_exist();

#endif
