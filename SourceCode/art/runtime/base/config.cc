

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <fcntl.h>
//#include <async_safe/log.h>
#include <android/log.h>
#include <vector>
#include <string>
#include "config.h"




#define HOOKA_LOG_STRING       "HOOKA"
#define HOOKA_CONFIG_FILE_NAME "/data/local/tmp/ART.config"
#define BYPASS_STRING_MAX_SIZE 512 

#define TARGET_APP_UID_PREFIX      "[TARGET_APP_UID]"
#define DISPLAY_STACK_TRACE_PREFIX "[DISPLAY_STACK_TRACE]"
#define DISPLAY_CODE_DUMP_PREFIX   "[DISPLAY_CODE_DUMP]"
#define DISPLAY_ARGS_PREFIX        "[DISPLAY_ARGS_INFO]"
#define DISPLAY_INSTRUCTION_PREFIX "[DISPLAY_INSTRUCTION_INFO]"
#define TRACE_MODE_PREFIX          "[TRACE_MODE]"

#define TRACE_MODE_DUMP_INSTRUCTION_FULL_PREFIX          "[TRACE_MODE_DISP_INSTRUCTION_FULL]"
#define TRACE_MODE_DUMP_INSTRUCTION_ITEM_PREFIX          "[TRACE_MODE_DISP_INSTRUCTION_ITEM]"
#define TRACE_MODE_DUMP_INSTRUCTION_GRAPH_PREFIX         "[TRACE_MODE_DUMP_INSTRUCTION_GRAPH]"
#define TRACE_MODE_DUMP_METHOD_ALL_IN_ONE_PREFIX         "[TRACE_MODE_DISP_METHOD_ALL_IN_ONE]"


 static int  g_Target_App_Uid  = -1;
 static int  g_Current_App_Uid = -1;
 static bool g_Is_Target_App   = false;

 static int g_Display_Stack_Trace = -1;
 static int g_Display_Code_Dump   = -1;
 static int g_Display_Args        = -1;
 static int g_Display_Instruction = -1;
 static int g_Trace_Mode          = -1;
 static bool g_Include_Rule_Exist =  false; 
 static bool g_Search_Item_Exist  =  false; 

 static std::vector<std::string> g_IncludeMethodName;
 static std::vector<std::string> g_ExcludeMethodName;
 static std::vector<std::string> g_SearchItem;


int LoadHOOKAConfig(void)
{
	char* pLine = NULL;
	char strLine[BYPASS_STRING_MAX_SIZE];
	memset(strLine, 0x0, BYPASS_STRING_MAX_SIZE);
	g_IncludeMethodName.clear();
	g_ExcludeMethodName.clear();
	g_SearchItem.clear();

	FILE *fp = fopen(HOOKA_CONFIG_FILE_NAME, "r");
	if(fp == NULL)
		return 0;

	while( fgets( strLine, BYPASS_STRING_MAX_SIZE, fp) != NULL )
	{
		if( ( pLine = strchr(strLine, '\n') ) != NULL)
			*pLine ='\0';

		if( ( pLine = strchr(strLine, '\r') ) != NULL)
			*pLine ='\0';

		// TARGET APP UID
		if( strstr(strLine, TARGET_APP_UID_PREFIX) != NULL && strncmp(strLine, "###", 3) != 0 )
		{
			char strTargetAppUid[10] = {0,};
			strncpy(strTargetAppUid, strLine+18, strlen(strLine)-19);
			g_Target_App_Uid  = atoi(strTargetAppUid);
			g_Current_App_Uid = getuid();
			if( g_Target_App_Uid == g_Current_App_Uid ) g_Is_Target_App = true;
		}

		// support Stack Trace
		// [DISPLAY_STACK_TRACE]=[Y]
		else if( strstr(strLine, DISPLAY_STACK_TRACE_PREFIX) != NULL && strncmp(strLine, "###", 3) != 0 )
		{
			if( strLine[23] == 'Y' )
				g_Display_Stack_Trace = 1; 
			else
				g_Display_Stack_Trace = 0; 
		}

		// [DISPLAY_CODE_DUMP]=[Y]
		else if( strstr(strLine, DISPLAY_CODE_DUMP_PREFIX) != NULL && strncmp(strLine, "###", 3) != 0 )
		{
			if( strLine[21] == 'Y' )
				g_Display_Code_Dump = 1; 
			else
				g_Display_Code_Dump = 0; 
		}

		// [DISPLAY_ARGS_INFO]=[Y]
		if( strstr(strLine, DISPLAY_ARGS_PREFIX) != NULL && strncmp(strLine, "###", 3) != 0 )
		{
			if( strLine[21] == 'Y' )
				g_Display_Args = 1; 
			else
				g_Display_Args = 0; 
				
		}

		// [DISPLAY_INSTRUCTION_INFO]=[Y]
		if( strstr(strLine, DISPLAY_INSTRUCTION_PREFIX) != NULL && strncmp(strLine, "###", 3) != 0 )
		{
			if( strLine[28] == 'Y' )
				g_Display_Instruction = 1; 
			else
				g_Display_Instruction = 0; 
				
		}
		
		// [TRACE_MODE]=[1] //1 or 2 or 3
		if( strstr(strLine, TRACE_MODE_PREFIX) != NULL && strncmp(strLine, "###", 3) != 0 )
		{
			g_Trace_Mode = (int)strLine[14] - 48;
			/*
			if( strLine[14] == '1' )
				g_Trace_Mode = 1; 
			else if( strLine[14] == '2' )
				g_Trace_Mode = 2; 
			else if( strLine[14] == '3' )
				g_Trace_Mode = 3; 
			else 
				g_Trace_Mode = 4; 
			*/
				
		}



		// [INCLUDE]=[void android.system.StructTimespec.<init>(long, long)]
		else if( strncmp(strLine, "###", 3) != 0 && strstr(strLine, "[INCLUDE]=[") != NULL )   
		{
			char strTemp[BYPASS_STRING_MAX_SIZE];
			memset(strTemp, 0x0, BYPASS_STRING_MAX_SIZE);

			strncpy(strTemp, strLine+11, strlen(strLine)-11-1);
			g_IncludeMethodName.push_back( strTemp );
 			g_Include_Rule_Exist = true;
		}

		// [EXCLUDE]=[void android.system.StructTimespec.<init>(long, long)]
		else if( strncmp(strLine, "###", 3) != 0 && strstr(strLine, "[EXCLUDE]=[") != NULL )   
		{
			char strTemp[BYPASS_STRING_MAX_SIZE];
			memset(strTemp, 0x0, BYPASS_STRING_MAX_SIZE);

			strncpy(strTemp, strLine+11, strlen(strLine)-11-1);
			g_ExcludeMethodName.push_back( strTemp );
		}

		// [SEARCH_ITEM]=[820115]
		else if( strncmp(strLine, "###", 3) != 0 && strstr(strLine, "[SEARCH_ITEM]=[") != NULL )   
		{
			char strTemp[BYPASS_STRING_MAX_SIZE];
			memset(strTemp, 0x0, BYPASS_STRING_MAX_SIZE);

			strncpy(strTemp, strLine+15, strlen(strLine)-15-1);
			g_SearchItem.push_back( strTemp );
			g_Search_Item_Exist = true;
		}



	}


	fclose(fp);
	return g_Target_App_Uid;
}

int check_target_method(const char* strMethodName)
{
	// CHECK Target Method Name
	std::vector<std::string>::iterator i;

	if( g_Include_Rule_Exist )
	{
		for(i = g_IncludeMethodName.begin(); i != g_IncludeMethodName.end(); i++)
		{
			// INCLUDE match success
			if( strstr( strMethodName, (*i).c_str() ) != nullptr )
			{
				// Check Exclude method name
				for(i = g_ExcludeMethodName.begin(); i != g_ExcludeMethodName.end(); i++)
				{
						// Exclude match success
						if( strstr( strMethodName, (*i).c_str() ) != nullptr ) 
							return 0;
				}
				return 1;
				////////////////////////////////////
			}
		}
		return 0;
	}

	else
	{
		// Check Exclude method name
		for(i = g_ExcludeMethodName.begin(); i != g_ExcludeMethodName.end(); i++)
		{
			if( strstr( strMethodName, (*i).c_str() ) != nullptr )
				return 0;
		}

		return 1;
	}
}

bool check_target_search_item(const char* strSearchItem)
{
	// CHECK Target Method Name
	std::vector<std::string>::iterator i;

	for(i = g_SearchItem.begin(); i != g_SearchItem.end(); i++)
	{
					if( strstr( strSearchItem, (*i).c_str() ) != nullptr )
					{
									return true;
					}
	}
	return false;

}
int get_trace_mode()
{
	return g_Trace_Mode;
}

int is_display_stack_trace()
{
	return g_Display_Stack_Trace;	
}

int is_display_code_dump()
{
	return g_Display_Code_Dump;
}

int is_display_args()
{
	return g_Display_Args;
}

int is_display_instruction()
{
	return g_Display_Instruction;
}


bool is_target_uid()
{
	return g_Is_Target_App;
}


int get_target_uid()
{
	return g_Target_App_Uid; 
}



int hooka_art(const char* strClassName)
{		
	if( check_target_method(strClassName) )
	{
		//async_safe_format_log(ANDROID_LOG_INFO, HOOKA_LOG_STRING, "[APP.DBI] [%s]", strClassName);
		//__android_log_print(ANDROID_LOG_INFO, HOOKA_LOG_STRING, "[ART] [%s]", strClassName);
	}
	return 0;
}

bool is_search_item_exist()
{
	return g_Search_Item_Exist;
}

		//__android_log_print(ANDROID_LOG_INFO, HOOKA_LOG_STRING, "[LOAD_INFO] [%s]", strTargetAppUid);
