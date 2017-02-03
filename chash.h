
#if !defined(UI64)
  #define UI64 unsigned long long
#endif

#if !defined(NULL)
 #define NULL 0L
#endif

extern "C" {
  UI64 Compute_Hash(const char* name);
}

int Next64Pointer(int idx);//Some Machines throw exception if int* is not align

typedef struct HashItem_t
{
  UI64  key_hash;
  int   key_idx;  //absolute byte index in the KeyValue data Pool (buffer) 
  int   key_count;
} HashItem;

typedef struct KeyValue_t
{
  int   next_idx;  //absolute byte index in KeyValue data Pool (buffer)
  int   key_len;
  int   val_len;
  //next is null terminated key
  //next is null terminated value
} KeyValue;

typedef struct KeyValue_Wrap_t
{
  KeyValue info;
  char     data[1];  //key 
  //next is null terminated value
} KeyValue_Wrap;

class KeyValuePool
{
public:
	KeyValuePool()  { Init();	 }
	~KeyValuePool() { Release(); }
	void  Empty()   { index = sizeof(KeyValue_Wrap) + 4; }//0 index is sentenal value

	KeyValue_Wrap*  GetItem(int idx)
	{ if( idx >= index ) return NULL; 	return (KeyValue_Wrap*)(&pBuffer[idx]); 	}

	int GetItem_KeyNext(int idx)
	{ if( idx >= index ) return 0; return ((KeyValue*)(&pBuffer[idx]))->next_idx; }

	const char* GetItem_Value(int idx)
	{ if( idx >= index ) return NULL; 	
	  KeyValue_Wrap* key = (KeyValue_Wrap*)(&pBuffer[idx]); 	
	  return &key->data[key->info.key_len];
	}

	const char* GetItem_Name(int idx)
	{ if( idx >= index ) return NULL; 	
	  KeyValue_Wrap* key = (KeyValue_Wrap*)(&pBuffer[idx]); 	
	  return key->data;
	}
	//
	int  AddItem(const char* prefix,const char* name,const char* value,int replaceComma);
	int  SetNext(int idx,int next);
private:
  void dump_Next64Pointer(int o,int n);

	void  Init()
	{
		nBuffer = 8180;
		pBuffer = new unsigned char[nBuffer+12];
		index = Next64Pointer(sizeof(KeyValue_Wrap) + 4);//0 index is sentenal value
		memset(pBuffer,0,index);
	}
	void Release()
	{
		if( pBuffer != NULL ) 
			delete []pBuffer; 
		pBuffer = NULL;
	}
	void  CheckAndGrow(int kvLen);
	//
	unsigned char  *pBuffer;
	int             nBuffer;
	int				index;
};

class HashPool
{
public:
	HashPool()    { Init();    }
	~HashPool()   { Release(); }
	void  Empty() { index = 1; }//0 index is sentenal value
	int   FindNext(UI64 hash,int idx);
	int   AddItem(UI64 hash,int kv_idx);
	int   ULimit() { return index; }

	HashItem* GetItem(int idx) 
	{ 	if( idx < index ) return &pItems[idx]; return NULL;	}

	int GetItem_KeyFirst(int idx)
	{ 	if( idx < index ) return pItems[idx].key_idx; return 0;	}

	int GetItem_KeyCount(int idx) 
	{ 	if( idx < index ) return pItems[idx].key_count; return 0; }

private:
	void  Init()
	{
		nItems = 800;
		pItems = new HashItem[nItems+4];
		index  = 1;	//0 index is sentenal value
		pItems[0].key_count = 0;
		pItems[0].key_hash = 0;
		pItems[0].key_idx = 0;
	}
	void Release()
	{
		if( pItems != NULL ) 
			delete []pItems; 
		pItems = NULL;
	}
	void  CheckAndGrow();
	//
	HashItem*	pItems;
	int			nItems;
	int			index;
};

#define CHASH_TAG    0x1BA2D4C3
class CHash
{
public:
	CHash()
	{
		Magic  = CHASH_TAG;
	}
	~CHash()
	{
		Magic  = 0x11223344;
	}
	const char* Add(const char* prefix,const char* name,const char* value,int replaceComma);
	int DumpAll();
	//
	int   ULimit()                     { return hashPool.ULimit(); }
	//
	int   key_Find(const char* name)   { return Find(Compute_Hash(name),name);  }
	int   key_Count(int key)           { return hashPool.GetItem_KeyCount(key); }
	//
	int   value_First(int key)         { return hashPool.GetItem_KeyFirst(key); }
	int   value_Next(int pos)          { return dataPool.GetItem_KeyNext(pos);  }
	const char* value_Data(int pos)    { return dataPool.GetItem_Value(pos);    }
	const char* value_Name(int pos)    { return dataPool.GetItem_Name(pos);     }
	//
	int  Empty()                       { hashPool.Empty(); dataPool.Empty(); return 0;}
	//
	unsigned int   GetMagic()          { return Magic; }
private:	
	int   Find(UI64 hash,const char* name);
	//
    unsigned int   Magic;
	HashPool	 hashPool;
	KeyValuePool dataPool;
};

