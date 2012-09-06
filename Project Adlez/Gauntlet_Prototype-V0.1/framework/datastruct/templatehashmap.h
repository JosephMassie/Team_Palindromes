#pragma once
// TODO replace with ../datastruct/string.h
#include <string.h>

template <class KEY, class VALUE>
struct KeyValuePair
{
	KEY k;	VALUE v;
	KeyValuePair(){}
	KeyValuePair(KEY const & k):k(k){}
	KeyValuePair(KEY const & k, VALUE const & v):k(k),v(v){}
	// operators overloaded for TemplateVector
	bool operator<(KeyValuePair<KEY,VALUE> const & kvp)const{return k < kvp.k;}
	bool operator>(KeyValuePair<KEY,VALUE> const & kvp)const{return k > kvp.k;}
	bool operator==(KeyValuePair<KEY,VALUE> const& kvp)const{return k ==kvp.k;}
	bool operator!=(KeyValuePair<KEY,VALUE> const& kvp)const{return !operator==(kvp);}
	static int hashFunction(KEY const & k)
	{
		// the KEY could very likely be pointer. This hash will turn memory 
		// addresses that are int-aligned into possibly consecutive keys, and
		// essentially modulo 32, accomplished here by bitwise-and-ing 32-1
		return (((int)k) >> (sizeof(int) >> 1)) & 31;
	}
};

template <class VALUE>
struct NameValuePair
{
	const char * k;	VALUE v;
	NameValuePair(){}
	NameValuePair(const char * k):k(k){}
	NameValuePair(const char * k, VALUE v):k(k),v(v){}
	// operators overloaded for TemplateVector
	bool operator<(NameValuePair<VALUE> const & kvp)const{return strcmp(k, kvp.k) < 0;}
	bool operator>(NameValuePair<VALUE> const & kvp)const{return strcmp(k, kvp.k) > 0;}
	bool operator==(NameValuePair<VALUE> const& kvp)const{return strcmp(k, kvp.k) ==0;}
	bool operator!=(NameValuePair<VALUE> const& kvp)const{return !operator==(kvp);}
	static int hashFunction(const char* const & k){return ((k[0]+k[1]) & 31);}
};

/**
 * a simple HashMap data structure, using TemplateVectors of KeyValuePairs as
 * buckets. The hash size is DEFAULT_BUCKET_SIZE, or 32. The hashFunction 
 * values between 0 and 31 by bitshifting and masking
 */
template <class KEY, class VALUE, class KVP_STRUCT>
class TemplateHashMap_BASE
{
	TemplateVector<TemplateVector<KVP_STRUCT>*> hash;

	static const int DEFAULT_BUCKET_SIZE = 32;

	int numElements;
public:
	int elementCount(){return numElements;}

	void clear(){
		for(int i = 0; i < hash.size(); ++i)
			hash.get(i).clear();
		numElements = 0;
	}
	TemplateHashMap_BASE()
	{
		hash.setSize(DEFAULT_BUCKET_SIZE);
		for(int i = 0; i < hash.size(); ++i){
			hash.set(i, NEWMEM(TemplateVector<KVP_STRUCT>()));
			hash.get(i)->init();
		}
	}
	/** clears the hash table (does not delete hash elements! they had better be referenced elsewhere...) */
	void release(){
		for(int i = 0; i < hash.size(); ++i){
			TemplateVector<KVP_STRUCT>* list = hash.get(i);
			if(list){
				DELMEM(list);
				hash.set(i, 0);
			}
		}
		hash.clear();
	}
	/** clears and deletes hash table elements (the elements had better be pointers!) */
	void deleteAll(){
		for(int y = 0; y < hash.size(); ++y){
			TemplateVector<KVP_STRUCT>* list = hash.get(y);
			for(int x = 0; x < list->size(); ++x){
				if(list->get(x).v){
					DELMEM(list->get(x).v);
					list->get(x).v = 0;
				}
			}
			list->clear();
			DELMEM(list);
			hash.set(y, 0);
		}
		hash.clear();
	}
	~TemplateHashMap_BASE(){release();hash.release();}

	/** @return the value associated with the given KEY */
	VALUE * getByKey(KEY const & k)
	{
		int index = KVP_STRUCT::hashFunction(k);
		TemplateVector<KVP_STRUCT> * bucket = hash.get(index);
		KVP_STRUCT kvp(k);
		int bucketIndex = bucket->indexOfWithBinarySearch(kvp);
		if(bucketIndex < 0)
			return 0;
		return &bucket->get(bucketIndex).v;
	}

	/** sets up a key/value pair association */
	void set(KEY const & k, VALUE const & v)
	{
		int index = KVP_STRUCT::hashFunction(k);
		TemplateVector<KVP_STRUCT> * bucket = hash.get(index);
		if(bucket->insertSorted(KVP_STRUCT(k,v), true) >= 0)
		{
			numElements++;
		}
	}
	/** @return the structure that does all the work for the hash map */
	TemplateVector<TemplateVector<KVP_STRUCT>*> * getRawDatabase(){
		return hash;
	}
};

template <class KEY, class VALUE>
class TemplateHashMap : public TemplateHashMap_BASE<KEY, VALUE, KeyValuePair<KEY,VALUE> >{};

template <class VALUE>
class TemplateHashMapNamed : public TemplateHashMap_BASE<const char*, VALUE, NameValuePair<VALUE> >{};
