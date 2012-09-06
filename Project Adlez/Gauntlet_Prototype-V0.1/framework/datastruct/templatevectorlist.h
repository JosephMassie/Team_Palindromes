#pragma once

#include "templatevector.h"

/**
 * a Vector that grows in a way that is memory stable.
 * this data structure is ideal when a vector of elements is needed,
 * and the elements need to stay stationary in memory, because they 
 * are being referenced by pointers elsewhere
 */
template <class DATA_TYPE>
class TemplateVectorList
{
private:
	/** a list of arrays */
	TemplateVector<DATA_TYPE*> m_allocations;
	int m_allocationSize, m_allocated, m_size;
public:
	TemplateVectorList(const int & a_allocationPageSize)
		:m_allocationSize(a_allocationPageSize), m_allocated(0), m_size(0){}
//	TemplateVectorList():m_allocationSize(10),m_allocated(0),m_size(0){}
	int size()
	{
		return m_size;
	}
	bool ensureCapacity(int a_size)
	{
		while(a_size >= m_allocated)
		{
#ifdef TEMPLATEARRAY_USES_MALLOC
			DATA_TYPE* arr = (DATA_TYPE*)malloc(sizeof(DATA_TYPE)*m_allocationSize);
#else
			DATA_TYPE* arr = NEWMEM_ARR(DATA_TYPE,m_allocationSize);
#endif
			if(!arr)
				return false;
			m_allocations.add(arr);
			m_allocated += m_allocationSize;
		}
		return true;
	}
	void setSize(const int & a_size)
	{
		ensureCapacity(a_size);
		m_size = a_size;
	}
	void clear()
	{
		setSize(0);
	}
	DATA_TYPE & get(const int & a_index)
	{
		int arrIndex = a_index / m_allocationSize;
		int subIndex = a_index % m_allocationSize;
		return m_allocations.get(arrIndex)[subIndex];
	}
	DATA_TYPE & getLast()
	{
		return get(m_size-1);
	}
	/** cleans up memory */
	inline void release()
	{
		for(int i = 0; i < m_allocations.size(); ++i)
		{
#ifdef TEMPLATEARRAY_USES_MALLOC
			free(m_allocations.get(i));
#else
			DELMEM_ARR(m_allocations.get(i));
#endif
		}
		m_allocations.setSize(0);
		m_allocated = 0;
		m_size = 0;
	}
	~TemplateVectorList(){release();}
	void set(const int & a_index, const DATA_TYPE & a_value)
	{
		// complex types must overload DATA_TYPE & operator=(const DATA_TYPE &)
		get(a_index) = a_value;
	}
	void add(const DATA_TYPE & a_value)
	{
		ensureCapacity(m_size);
		set(m_size++, a_value);
	}
	/** adds a new empty element to the end of the vector list */
	void add(){
		ensureCapacity(m_size++);
	}
	/** adds a standard C array to this vector list */
	void add(DATA_TYPE * a_array, const int & a_size)
	{
		for(int i = 0; i < a_size; ++i)
		{
			add(a_array[i]);
		}
	}
	int indexOf(const DATA_TYPE & a_value, const int & a_start)
	{
		int arrIndex = a_start / m_allocationSize;
		int subIndex = a_start % m_allocationSize;
		int lastList = m_size / m_allocationSize;
		int maxInList;
		bool found = false;
		for(; !found && arrIndex < m_allocations.size(); ++arrIndex)
		{
			maxInList = (arrIndex < lastList)?m_allocationSize:(a_start % m_allocationSize);
			for(; !found && subIndex < maxInList; ++subIndex)
			{
				found = m_allocations.get(arrIndex)[subIndex] == a_value;
			}
			subIndex = 0;
		}
		return found?(arrIndex*m_allocationSize+subIndex):-1;
	}
	int indexOf(const DATA_TYPE & a_value)
	{
		return indexOf(a_value, 0);
	}
	int indexOf(const DATA_TYPE * a_memoryLocation){
		DATA_TYPE * start, * end;
		for(int i = 0; i < m_allocations.size(); ++i){
			start = m_allocations.get(i);
			end = start+m_allocationSize;
			if(a_memoryLocation >= start && a_memoryLocation < end)
			{
				int index = (int)a_memoryLocation - (int)start;
				index /= sizeof(DATA_TYPE);
				return index + i*m_allocationSize;
			}
		}
		return -1;
	}
	inline void moveUp(const int & a_from, const int & a_offset, int a_size)
	{
		for(int i = a_size-a_offset-1; i >= a_from; --i)
		{
			set(i+a_offset, get(i));
		}
	}
	void insert(const int & a_index, const DATA_TYPE & a_value)
	{
		ensureCapacity(m_size+1);
		m_size++;
		moveUp(a_index, 1, m_size);
		set(a_index, a_value);
	}
};
