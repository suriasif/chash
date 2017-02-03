#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "chash_t.h"
#include "cmap_t.h"

extern int Diag_Level;
int Diag_High = 0;
int Diag_Medium = 0;
int Diag_Low = 0;

void* pFldHash = NULL;  //Global HashMap for All out Record Fields
void* pIdsHash = NULL;  //Global HashMap for Acc, dedAcc etc Ids maps

long long String2Number(const char* value,int base); 
const char* String2String(const char* value,int base); 
const char* FormatString(const char* format,...);

void My_Diagnostic(int dbg,const char* format,...)
{
  if( dbg >  Diag_Level  ) return;
  
  char buffer[4096];
  
  va_list args;
  va_start (args, format);
  
  vsnprintf (buffer,4000,format, args);
  xyz_diagnostic("XYZ", dbg, buffer);
  
  va_end (args);  
}

void set_diagnostic_level()
{
    if( Diag_Level >= DBG_HIGH )    Diag_High = 1;
    if( Diag_Level >= DBG_MEDIUM )  Diag_Medium = 1;
    if( Diag_Level >= DBG_LOW )     Diag_Low = 1;
    dbg_out(DBG_LOW,"set_diagnostic_level(): [%d]",Diag_Level)
}

void DumpKeys(void* phash)
{
	int pos,i;
	for(i=1; i < chash_ULimit(phash); i++)
	{
		pos = chash_value_First(phash,i);
		dbg_out(DBG_HIGH,"(CHash) +--Key %s(%d): ",chash_value_Name(phash,pos), chash_key_Count(phash,i) )
		while(pos > 0 )
		{
		  dbg_out(DBG_HIGH,"(CHash)    +--Value %s",chash_value_Data(phash,pos) )
			pos = chash_value_Next(phash,pos);
		}
	}
}

void testLib_end() 
{
  if( pFldHash != NULL ) chash_Release(pFldHash);  //Delete the Instance of Chash Map
  pFldHash = NULL;
  
  if( pIdsHash != NULL ) chash_Release(pIdsHash);  //Delete the Instance of Chash Map
  pIdsHash = NULL;
}

void testLib_init() 
{
	  pFldHash    = chash_Init(); // Create an Instance of Chash Map
	  pIdsHash    = chash_Init(); // Create an Instance of Chash Map
}

void testLib_process(void) 
{
    chash_Empty(pFldHash); //Make it Empty before use
    chash_Empty(pIdsHash); //Make it Empty before use
    
    //processing
    
    chash_Empty(pFldHash); //Make it empty after use
    chash_Empty(pIdsHash); //Make it empty after use
}

// int snprintf(char *str, size_t size, const char *format, ...);
///////////////////////////////////////////////////////////////////
///////////      Aggregation Functions              ///////////////
///////////////////////////////////////////////////////////////////
const char* GetSum_OverOut(const char* name)
{
	long long Num;
  long long Sum = 0;
  const char* val;
  
  int key = chash_key_Find(pFldHash,name);
  if( key == 0 ) return NULL;
  
	int pos = chash_value_First(pFldHash,key);
	while(pos > 0 )
	{
	  val = chash_value_Data(pFldHash,pos);
	  if( val != NULL )
	  {
      Num = 0;
      sscanf(val,"%lld",&Num);
  	  Sum = Sum + Num;
  		pos = chash_value_Next(pFldHash,pos);
    }
	}
	return FormatString("%lld",Sum);
}
const char* GetMax_OverOut(const char* name)
{
	long long Num;
  long long Max = 0;
  const char* val;
  
  int key = chash_key_Find(pFldHash,name);
  if( key == 0 ) return NULL;
  
	int pos = chash_value_First(pFldHash,key);
	while(pos > 0 )
	{
	  val = chash_value_Data(pFldHash,pos);
	  if( val != NULL )
	  {
      Num = 0;
      sscanf(val,"%lld",&Num);
      if( Num > Max) Max = Num;
  		pos = chash_value_Next(pFldHash,pos);
    }
	}
	return FormatString("%lld",Max);
}
const char* GetCount_OverOut(const char* name)
{
	long long Num;
  long long Max = 0;
  const char* val;
  const char* result = NULL;
  
  int key = chash_key_Find(pFldHash,name);
  if( key == 0 ) return NULL;
  
	int pos = chash_value_First(pFldHash,key);
  Max = 1;
	while(pos > 0 )
	{
	  val = chash_value_Data(pFldHash,pos);
	  if( val != NULL )
	  {
	    Max = Max + 1;
  		pos = chash_value_Next(pFldHash,pos);
    }
	}
	return FormatString("%lld",Max);
}
const char* GetLast_OverOut(const char* name)
{
	long long Num;
  long long Max = 0;
  const char* val;
  const char* result = NULL;
  
  int key = chash_key_Find(pFldHash,name);
  if( key == 0 ) return NULL;
  
	int pos = chash_value_First(pFldHash,key);
	while(pos > 0 )
	{
	  val = chash_value_Data(pFldHash,pos);
	  if( val != NULL )
	  {
	    result = val;
  		pos = chash_value_Next(pFldHash,pos);
    }
	}
	return result;
}
const char* GetFirst_OverOut(const char* name)
{
	long long Num;
  long long Max = 0;
  
  int key = chash_key_Find(pFldHash,name);
  if( key == 0 ) return NULL;
  
	int pos = chash_value_First(pFldHash,key);
	if( pos == 0 ) return NULL;
	
  return chash_value_Data(pFldHash,pos);
}
///////////////////////////////////////////////////////////////////
///////////      Common / Generic Txd Funcction     ///////////////
///////////////////////////////////////////////////////////////////
char Last_Falt_Value[512];
int Add_Flat_Field(const char* prefix,const char* name,const char* value)
{
   memset(Last_Falt_Value,0,510);
   if( strncmp(name,"undec_",6)== 0 )
   {
     const char* tmp = String2String(value,16);
     chash_Add(pFldHash,prefix,&name[6],tmp,0);
     
     strncpy(Last_Falt_Value,tmp,500);
     dbg_out(DBG_HIGH,"(####)   Adding FaltField [%s]+[%s] = [%s]",prefix,&name[6],tmp)
   } 
   else
   {
      chash_Add(pFldHash,prefix,name,value,1);
      strncpy(Last_Falt_Value,value,500);
      dbg_out(DBG_HIGH,"(####)   Adding FaltField [%s]+[%s] = [%s]",prefix,name,value)
   }
   return 0;
}
/////////////////////        
const char* FormatString(const char* format,...)
{
  static char  cache_buff[65536];
  static int   cache_pos = 0;
  
  char *cache = &cache_buff[cache_pos * 1024];
  cache_pos = cache_pos + 1;
  if( cache_pos >= 64 ) cache_pos = 0;

  va_list args;
  va_start (args, format);
  vsnprintf (cache,1020,format, args);
  va_end (args);
  return cache;  
}
long long String2Number(const char* value,int base) 
{ 
   long long tmp = 0;
   if( base == 16 )
      sscanf(value,"%llX",&tmp);
   else
      sscanf(value,"%lld",&tmp);
   return tmp;
}
const char* String2String(const char* value,int base) 
{ 
   long long tmp = 0;
   if( base == 16 )
      sscanf(value,"%llX",&tmp);
   else
      sscanf(value,"%lld",&tmp);
   return FormatString("%lld",tmp);
}
///////////////////////////////////
