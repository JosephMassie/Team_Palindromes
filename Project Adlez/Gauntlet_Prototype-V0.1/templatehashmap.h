#pragma once

/**
 * a simple HashMap data structure, using TemplateVectors of KeyValuePairs as
 * buckets. The hash size is DEFAULT_BUCKET_SIZE, or 32. The hashFunction 
 * values between 0 and 31 by bitshifting and masking
 */
template <class KEY, class VALUE>
class TemplateHashMap
{
	struct KeyValuePair
	{
		KEY k;	VALUE v;
		KeyValuePair(){}
		KeyValuePair(KEY k):k(k){}
		KeyValuePair(KEY k, VALUE v):k(k),v(v){}
		// operators overloaded for TemplateVector
		bool operator<(KeyValuePair const & kvp){return k < kvp.k;}
		bool operator>(KeyValuePair const & kvp){return k > kvp.k;}
		bool operator==(KeyValuePair const& kvp){return k ==kvp.k;}
	};

	TemplateVector<TemplateVector<KeyValuePair>> hash;

	static const int DEFAULT_BUCKET_SIZE = 32;
public:
	void clear(){
		for(int i = 0; i < hash.size(); ++i)
			hash.get(i).clear();
	}
	TemplateHashMap()
	{
		hash.setSize(DEFAULT_BUCKET_SIZE);
	}

	int hashFunction(KEY k)
	{
		// the KEY could very likely be pointer. This hash will turn memory 
		// addresses that are int-aligned into possibly consecutive keys, and
		// essentially modulo 32, accomplished here by bitwise-and-ing 32-1
		return (((int)k) >> (sizeof(int) >> 1)) & 31;
	}

	/** @return the value associated with the given KEY */
	VALUE * getByKey(KEY k)
	{
		int index = hashFunction(k);
		TemplateVector<KeyValuePair> * bucket = &hash.get(index);
		KeyValuePair kvp(k);
		int bucketIndex = bucket->indexOf(kvp);
		if(bucketIndex < 0)
			return 0;
		return &bucket->get(bucketIndex).v;
	}

	/** sets up a key/value pair association */
	void set(KEY k, VALUE v)
	{
		int index = hashFunction(k);
		TemplateVector<KeyValuePair> * bucket = &hash.get(index);
		bucket->add(KeyValuePair(k,v));
	}
};