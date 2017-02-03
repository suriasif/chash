//#include "stdafx.h"
#include <memory.h>
#include <string.h>

#include "chash.h"
#include "chash_t.h"

int Diag_Level = DBG_HIGH;

int Next64Pointer(int idx)//Some Machines throw exception if int* is not align 
{
  int a = idx % 8; //64bit pointer
  int idx_align = idx;
  if( a != 0) idx_align = idx + (8-a);
  return idx_align; 
}

///////////////////////////////////////////////////////////////////
/////////////////////    KeyValuePool Methods     /////////////////
///////////////////////////////////////////////////////////////////
void KeyValuePool::CheckAndGrow(int kvLen)
{
	if( nBuffer == 0 || pBuffer  == NULL ) { Init(); }
    int n = Next64Pointer(index + sizeof(KeyValue) + kvLen + 32);  
    if( n >= nBuffer ) //we will over run the buffer 
    {
        n = nBuffer+8192;
        unsigned char *tmp = new unsigned char[n+12];
        memcpy(tmp,pBuffer,nBuffer);
        delete []pBuffer;
        pBuffer = tmp;
        nBuffer = n; 
    }  
}

int  KeyValuePool::AddItem(const char* prefix,const char* name,const char* value,int replaceComma)
{
  static char empty_prefix[8] = {0,0,0,0,0,0,0,0};
  empty_prefix[0]=0;
  const char* prfx = prefix;
  if( prefix == NULL ) prfx = empty_prefix;  
  
  int nPrx = strlen(prfx) + 1;
  int nKey = strlen(name) + 1;
  int nVal = strlen(value) + 1; //+1 is null termination char length
	CheckAndGrow(nPrx+nKey+nVal);

	KeyValue_Wrap* pKey = (KeyValue_Wrap*)(&pBuffer[index]); // prepare new KeyValue
	pKey->info.key_len = nKey + nPrx;
	pKey->info.val_len = nVal;
	pKey->info.next_idx = 0;
	
	strcpy(pKey->data,prfx);
	strcpy(&pKey->data[nPrx-1],name);
	strcpy(&pKey->data[nKey+nPrx],value);
	
	char* hval = &pKey->data[nKey+nPrx];
  if( replaceComma == 1)
  { 
    for(int j=0;hval[j]!=0;j++) 
    { 
      if( hval[j] == ',' ) hval[j] = '_';
    }
  }

	int old = index;
	index = index + sizeof(KeyValue_Wrap) + nPrx + nKey + nVal;
	index = Next64Pointer(index);
	return old;
}
       


int  KeyValuePool::SetNext(int idx,int next)
{
	if( idx >= index ) return 0;
	KeyValue* pKey = (KeyValue*)(&pBuffer[idx]);
	while( pKey->next_idx != 0 )
	{
		idx = pKey->next_idx;
		pKey = (KeyValue*)(&pBuffer[ pKey->next_idx ]);
	}
	pKey->next_idx = next;
	return idx;
}
///////////////////////////////////////////////////////////////////
/////////////////////     HashPool Methods     ////////////////////
///////////////////////////////////////////////////////////////////
void HashPool::CheckAndGrow()
{
	if( nItems == 0 || pItems  == NULL ) { Init(); }
    int n = index + 2;  
    if( n >= nItems ) //we will over run the Array
    {
        n = nItems+100;
        HashItem* tmp = new HashItem[n+4];
        memcpy(tmp,pItems,nItems * sizeof(HashItem));
		delete []pItems;
        pItems = tmp;
		nItems = n;
    }  
}

int HashPool::AddItem(UI64 hash,int kv_idx)
{
	CheckAndGrow();
	pItems[index].key_count = 1;
	pItems[index].key_hash = hash;
	pItems[index].key_idx = kv_idx;
	int old = index;
	index++;
	return old;
}

int HashPool::FindNext(UI64 hash,int idx)
{
	for(int i=idx; i < index ; i ++ )
	{
		if( pItems[i].key_count > 0 && pItems[i].key_hash == hash ) // we found the entry but nee to confirm
			return i;
	}
	return 0;
}
///////////////////////////////////////////////////////////////////
/////////////////////       CHash Methods      ////////////////////
///////////////////////////////////////////////////////////////////
int CHash::Find(UI64 hash,const char* name)
{
	KeyValue_Wrap* pKey;
	HashItem*      pHash;
	int idx = 1;
	while( idx > 0 )
	{
		idx = hashPool.FindNext(hash,idx);
		pHash = hashPool.GetItem(idx);
		if( pHash != NULL ) 
			pKey = dataPool.GetItem(pHash->key_idx); 
		else
			pKey = NULL;
		if( idx > 0  && pKey != NULL )
		{
			if( pKey->info.key_len > 0 && pKey->info.key_len < 512 && strcmp(pKey->data,name) == 0)	
				return idx;
		}
		if( idx > 0 ) idx++;
	}
	return 0;
}
const char* CHash::Add(const char* prefix,const char* name,const char* value,int replaceComma)
{
	// Adding new KeyValue to KeyValue Pool
	int kv_index = dataPool.AddItem(prefix,name,value,replaceComma);
	const char* full_name = dataPool.GetItem_Name(kv_index);

	UI64 hash = Compute_Hash(full_name);
	int idx = Find(hash,full_name);
	if( idx == 0 ) // Does not exist
	{ 
		hashPool.AddItem(hash,kv_index); 	
	}
	else //when entry already exist, append the kv 
	{
		HashItem* ph =  hashPool.GetItem(idx); 
		if( ph != NULL )
		{
			ph->key_count++;
			dataPool.SetNext(ph->key_idx,kv_index);
		}
	}
	return full_name;
}

int CHash::DumpAll()
{
	int idx;
	HashItem* ph;
	KeyValue_Wrap* pKey;
	char* pVal;
	for(int i=1;i<hashPool.ULimit();i++)
	{
		ph = hashPool.GetItem(i);
		if( ph != NULL && ph->key_count > 0 )
		{
			idx = ph->key_idx;
			while(idx != 0 )
			{
				pKey = dataPool.GetItem(idx);
				pVal = &pKey->data[pKey->info.key_len];
				dbg_out(DBG_HIGH,"Hash: Key=%s and Value=%s",pKey->data,pVal)
				//printf("%s %s\n",pKey->data,pVal);

				idx = pKey->info.next_idx;
			}
		}
	}
	return 0;
}
///////////////////////////////////////////////////////////////////
/////////////////////       CHash Wrapper      ////////////////////
///////////////////////////////////////////////////////////////////
#define invoke_chash_int(ptr,func,...) { if ( ptr == NULL ) {dbg_out(DBG_HIGH,"CHash pointer cannot be NULL") return 1;} \
	CHash* pHash = (CHash*)ptr; if( pHash->GetMagic() != CHASH_TAG ) {dbg_out(DBG_HIGH,"CHash not initialized") return 1;} \
	return pHash->func(__VA_ARGS__); }

#define invoke_chash_char(ptr,func,...) { if ( ptr == NULL ) {dbg_out(DBG_HIGH,"CHash pointer cannot be NULL") return NULL;} \
	CHash* pHash = (CHash*)ptr; if( pHash->GetMagic() != CHASH_TAG ) {dbg_out(DBG_HIGH,"CHash not initialized") return NULL;} \
	return pHash->func(__VA_ARGS__); }

extern "C"
{
  UI64 Compute_Hash(const char* name)
  {
     if( name == NULL ) return 1;
     if( name[0] == 0 ) return 2;
     UI64 hash = 0;
     UI64 ch = 0;
     for(int i = 0; name[i] != 0; i++)
     {
        ch = name[i];
        hash = ch + (hash << 6) + (hash << 16) - hash;
     }
     return hash;
  }

	void* chash_Init()
	{
		CHash* ptr = new CHash();
		return ptr;
	}

	int   chash_Release(void* This)
	{
		if( This == NULL )
		{
		  dbg_out(DBG_HIGH,"CHash pointer cannot be NULL")
		  return 1;  
		}
		CHash* pHash = (CHash*)This;
		delete pHash;
		return 0;
	}

	const char* chash_Add(void* This,const char* prefix,const char* name,const char* value,int replaceComma)
	{	invoke_chash_char(This,Add,prefix,name,value,replaceComma) 	}

	int  chash_DumpAll(void* This)
	{	invoke_chash_int(This,DumpAll) 	}

	int   chash_ULimit(void* This)
	{	invoke_chash_int(This,ULimit) 	}

	int   chash_Empty(void* This)
	{	invoke_chash_int(This,Empty) 	}

	int   chash_key_Count(void* This,int key)
	{	invoke_chash_int(This,key_Count,key) 	}

	int   chash_key_Find(void* This,const char* name)
	{	invoke_chash_int(This,key_Find,name) 	}

	int   chash_value_First(void* This,int key)
	{	invoke_chash_int(This,value_First,key) 	}

	const char*  chash_value_Name(void* This,int pos)
	{	invoke_chash_char(This,value_Name,pos) 	}

	const char*  chash_value_Data(void* This,int pos)
	{	invoke_chash_char(This,value_Data,pos) 	}

	int   chash_value_Next(void* This,int pos)
	{	invoke_chash_int(This,value_Next,pos) 	}
}

