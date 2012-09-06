#pragma once

#include "templatearray.h"

/**
 * simple templated vector. Ideal for dynamic lists of primitive types and single pointers.
 * @WARNING template with virtual types, or types that will be pointed to at your own risk!
 * in those situations, templates of pointers to those types are a much better idea.
 * @author mvaganov@hotmail.com December 2009
 */
template<typename DATA_TYPE>
class TemplateVector : public TemplateArray<DATA_TYPE>
{
protected:
	/** the default size to allocate new vectors to */
	static const int DEFAULT_ALLOCATION_SIZE = 10;

	/** number of valid elements that the caller thinks we have.. */
	int m_size;

public:
	/** sets all fields to an initial data state. WARNING: can cause memory leaks if used without care */
	inline void init()
	{
		TemplateArray<DATA_TYPE>::init();
		m_size = 0;
	}

	/** cleans up memory */
	inline void release()
	{
		TemplateArray<DATA_TYPE>::release();
		m_size = 0;
	}

	/** @return true of the copy finished correctly */
	inline bool copy(const TemplateVector<DATA_TYPE> & a_vector)
	{
		if(ensureCapacity(a_vector.m_size))
		{
			for(int i = 0; i < a_vector.m_size; ++i)
			{
				set(i, a_vector.get(i));
			}
			m_size = a_vector.m_size;
			return true;
		}
		return false;
	}

	/** copy constructor */
	inline TemplateVector(
		const TemplateVector<DATA_TYPE> & a_vector)
	{
		init();
		copy(a_vector);
	}

	/** default constructor allocates no list (zero size) */
	inline TemplateVector(){init();}

	/** format constructor */
	inline TemplateVector(const int & a_size,
		const DATA_TYPE & a_defaultValue)
	{
		init();
		ensureCapacity(a_size);
		for(int i = 0; i < a_size; ++i)
			add(a_defaultValue);
	}

	/** @return the last value in the list */
	inline DATA_TYPE & getLast()
	{
		return m_data[m_size-1];
	}

	/** @return the last added value in the list, and lose that value */
	inline DATA_TYPE & pop()
	{
		return m_data[--m_size];
	}

	/**
	 * @param value to add to the list 
	 * @note adding a value may cause memory allocation
	 */
	void add(const DATA_TYPE & a_value)
	{
		// where am i storing these values?
		// if i don't have a place to store them, i better make one.
		if(m_data == 0)
		{
			// make a new list to store numbers in
			allocateToSize(DEFAULT_ALLOCATION_SIZE);
		}
		// if we don't have enough memory allocated for this list
		if(m_size >= m_allocated)
		{
			// make a bigger list
			allocateToSize(m_allocated*2);
		}
		set(m_size, a_value);
		m_size++;
	}

	/**
	 * @param a_value to add to the list if it isnt in the list already
	 * @return true the index where the element exists
	 */
	int addNoDuplicates(const DATA_TYPE & a_value)
	{
		int index = indexOf(a_value);
		if(index < 0)
		{
			index = m_size;
			add(a_value);
		}
		return index;
	}

	/** @param a_vector a vector to add all the elements from */
	inline void addVector(const TemplateVector<DATA_TYPE> & a_vector)
	{
		for(int i = 0; i < a_vector.size(); ++i)
		{
			add(a_vector.get(i));
		}
	}

	/** @return the size of the list */
	inline const int & size() const
	{
		return m_size;
	}

	/** 
	 * @param size the user wants the vector to be (chopping off elements)
	 * @return false if could not allocate memory
	 * @note may cause memory allocation if size is bigger than current
	 */
	inline bool setSize(const int & a_size)
	{
		if(!ensureCapacity(a_size))
			return false;
		m_size = a_size;
		return true;
	}

	/** sets size to zero, but does not deallocate any memory */
	inline void clear()
	{
		setSize(0);
	}

	/** 
	 * @param a_index is overwritten by the next element, which is 
	 * overwritten by the next element, and so on, till the last element
	 */
	void remove(const int & a_index)
	{
		moveDown(a_index, -1, m_size);
		setSize(m_size-1);
	}

	/** 
	 * @param a_index where to insert a_value. shifts elements in the vector.
	 */
	void insert(const int & a_index, const DATA_TYPE & a_value)
	{
		setSize(m_size+1);
		moveUp(a_index, 1, m_size);
		set(a_index, a_value);
	}

	/** 
	 * @return first element from the list and moves the rest up 
	 * @note removes the first element from the list
	 */
	inline const DATA_TYPE pull()
	{
		DATA_TYPE value = get(0);
		remove(0);
		return value;
	}

	/** @param a_index is replaced by the last element, then size is reduced. */
	inline void removeFast(const int & a_index)
	{
		swap(a_index, m_size-1);
		setSize(m_size-1);
	}

	/** @return the index of the first appearance of a_value in this vector. uses == */
	inline int indexOf(const DATA_TYPE & a_value) const
	{
		for(int i = 0; i < m_size; ++i)
		{
			if(get(i) == a_value)
				return i;
		}
		return -1;
	}

	/** @return index of 1st a_value at or after a_startingIndex. uses == */
	inline int indexOf(const DATA_TYPE & a_value, const int & a_startingIndex) const
	{
		return TemplateArray::indexOf(a_value, a_startingIndex, m_size);
	}

	/** @return index of 1st a_value at or after a_startingIndex. uses == */
	inline int indexOf(const DATA_TYPE & a_value, const int & a_startingIndex, int const & a_size) const
	{
		return TemplateArray::indexOf(a_value, a_startingIndex, a_size);
	}

	/**
	 * will only work correctly if the TemplateVector is sorted.
	 * @return the index of the given value, -1 if the value is not in the list
	 */
	int indexOfWithBinarySearch(const DATA_TYPE & a_value)
	{
		if(m_size)
		{
			return TemplateArray::indexOfWithBinarySearch(a_value, 0, m_size);
		}
		return -1;    // failed to find key
	}

	/**
	 * uses binary sort to put values in the correct index. safe if soring is always used
	 * @param a_value value to insert in order
	 * @param a_allowDuplicates will not insert duplicates if set to false
	 * @return the index where a_value was inserted
	 */
	int insertSorted(const DATA_TYPE & a_value, const bool & a_allowDuplicates)
	{
		int index;
		if(!m_size || a_value < get(0))
		{
			index = 0;
		}
		else if(!(a_value < get(m_size-1)))
		{
			index = m_size;
		}
		else
		{
			int first = 0, last = m_size;
			while (first <= last)
			{
				index = (first + last) / 2;
				if (!(a_value < m_data[index]))
					first = index + 1;
				else if (a_value < m_data[index]) 
					last = index - 1;
			}
			if(!(a_value < m_data[index]))
				index++;
		}
		if(!m_size  || a_allowDuplicates || a_value != m_data[index])
			insert(index, a_value);
		return index;
	}

	/**
	 * @param a_value first appearance replaced by last element. breaks if not in list
	 */
	inline void removeDataFast(const DATA_TYPE & a_value)
	{
		removeFast(indexOf(a_value));
	}

	/**
	 * @param a_listToExclude removes these elements from *this list
	 * @return true if at least one element was removed
	 */
	inline bool removeListFast(const TemplateVector<DATA_TYPE> & a_listToExclude)
	{
		bool aTermWasRemoved = false;
		for(int e = 0; e < a_listToExclude.size(); ++e)
		{
			for(int i = 0; i < size(); ++i)
			{
				if(a_listToExclude.get(e) == get(i))
				{
					removeFast(i);
					--i;
					aTermWasRemoved = true;
				}
			}
		}
		return aTermWasRemoved;
	}

	/** @param a_value first appearance is removed. breaks if not in list */
	inline void removeData(const DATA_TYPE & a_value)
	{
		remove(indexOf(a_value));
	}

	/** destructor */
	inline ~TemplateVector()
	{
		release();
	}
};