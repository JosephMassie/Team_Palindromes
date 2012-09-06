#pragma once

#include "templatevectorlist.h"

/** the type used to mark free elements in the pool. */
#define FREED_OVERHEAD_TYPE	short
/**
 * this should be used for particles, or game objects that are constantly 
 * created and destroyed. It doesn't make a lot of sense to use this for
 * small-sized data, since every freed element has a small amount of overhead
 */
template <class DATA_TYPE>
class TemplatePool
{
public:
	/** the pool of elements */
	TemplateVectorList<DATA_TYPE> pool;
	/** a list of elements that count as 'free', and can be overwritten */
	TemplateVector<FREED_OVERHEAD_TYPE> freed;
	TemplatePool():pool(128){}
	/** @return the list of all allocated elements */
	TemplateVectorList<DATA_TYPE> * getAllocated(){return &pool;}
	/** @return a good-as-new element */
	DATA_TYPE * newData(){
		if(freed.size() > 0){
			return &pool.get(freed.pop());
		}
		pool.add();
		return &pool.getLast();
	}
	/** mark the given element as free */
	void freeData(DATA_TYPE * data){
		int index = pool.indexOf(data);
		// if the last element in the pool was just freed
		if(index == pool.size()-1){
			// we can remove it from the end of the pool, not bothering to add it to the list of free nodes
			pool.setSize(pool.size()-1);
			// check to see if the next last-element-in-the-pool is also ready to be marked free
			index = freed.indexOf(pool.size()-1);
			// if the there are more nodes that can be cleared at the end of the pool
			if(index >= 0){
				int clearedPoolUnitsAtTheEnd = 0;
				// mark which elements in the freed list we can remove, because they are at the free end of the pool
				const int CLEAR = -1;
				// while there is a valid pool unit to remove (from the end)
				while(index >= 0){
					// mark to clear this unit from the pool, to reduce the size managed by the freed list
					freed.set(index, CLEAR);
					++clearedPoolUnitsAtTheEnd;
					// check to see if the next last-element-in-the-pool is also ready to be marked free
					index = freed.indexOf(pool.size()-1-clearedPoolUnitsAtTheEnd);
				}
				// remove all pool units marked to be cleared
				clearedPoolUnitsAtTheEnd = freed.removeAll(CLEAR);
				pool.setSize(pool.size()-clearedPoolUnitsAtTheEnd);
			}
		}else{
			freed.add((FREED_OVERHEAD_TYPE)index);
		}
	}
	/** clear the pool-- deallocates memory! */
	void release(){
		freed.release();
		pool.release();
	}
	/** clear the pool-- does NOT deallocate memory */
	void clear(){
		freed.clear();
		pool.clear();
	}
};
#undef FREED_OVERHEAD_TYPE
