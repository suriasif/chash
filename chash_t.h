
#define DBG_LOW      10
#define DBG_MEDIUM   50
#define DBG_HIGH     90

#define dbg_out(x,...) { if ( x <= Diag_Level ) My_Diagnostic(x,__VA_ARGS__); }

int Next64Pointer(int idx);//Some Machines throw exception if int* is not align 


#ifdef __cplusplus
extern "C" {
#endif

#if !defined(UI64)
  #define UI64 unsigned long long
#endif

	void My_Diagnostic(int dbg,const char* format,...);
  UI64 Compute_Hash(const char* name);

	void* chash_Init();
	const char* chash_Add(void* This,const char* prefix,const char* name,const char* value,int replaceComma);
	int   chash_Release(void* This);
	int   chash_DumpAll(void* This);
	int   chash_ULimit(void* This);
	int   chash_Empty(void* This);

	int   chash_key_Count(void* This,int key);
	int   chash_key_Find(void* This,const char* name);
	int   chash_value_First(void* This,int key);
	const char* chash_value_Name(void* This,int pos);
	const char* chash_value_Data(void* This,int pos);
	int   chash_value_Next(void* This,int pos);

  
#ifdef __cplusplus
}
#endif

