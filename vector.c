#include "vector.h"
#include <stdlib.h>
#include <stdio.h>

#define SIGN_ALLOCATED 0xdeadbeef
#define SIGN_DEALLOCATED 0

struct Vector
{
	void** m_items;
	size_t m_size;
	size_t m_initialCapacity;
	size_t m_capacity;
	size_t m_blockSize;
	int m_signature;
};

/****************************************************/
/*       Static fucntions declarations              */

static Vector_Result Vector_Shrink(Vector* _vector);
static Vector_Result Vector_Grow(Vector* _vector);

/****************************************************/
/*        API functions implementation              */

Vector* Vector_Create(size_t _initialCapacity, size_t _blockSize)
{
	Vector* vector = NULL;
	
	if(0 == _initialCapacity && 0 == _blockSize)
	{
		return NULL;
	}
	
	vector = (Vector*) malloc(sizeof(Vector));
	
	if(NULL == vector)
	{
		return NULL;
	}
	
	/*TODO: check with zero initial capacity. Check with realloc in that case as well*/
	vector->m_items = malloc(_initialCapacity * sizeof(void*));
	
	if(NULL == vector->m_items)
	{
		free(vector);
		return NULL;
	}

	vector->m_size = 0;
	vector->m_initialCapacity = _initialCapacity;
	vector->m_capacity = _initialCapacity;
	vector->m_blockSize = _blockSize;
	vector->m_signature = SIGN_ALLOCATED;
	
	return vector;
}

void Vector_Destroy(Vector** _vector, void (*_elementDestroy)(void* _item))
{
	int itemIndex, numOfItems;
	if(NULL != _vector && NULL != *_vector && SIGN_ALLOCATED == (*_vector)->m_signature)
	{
		numOfItems = (*_vector)->m_size;	
		
		/*TODO what if elementDestroy func is NULL??*/
		if(_elementDestroy != NULL)
		{
			for(itemIndex = 0; itemIndex < numOfItems; ++itemIndex)
			{
				_elementDestroy((*_vector)->m_items[itemIndex]);
			}
		}
		
		free((*_vector)->m_items);
		(*_vector)->m_signature = SIGN_DEALLOCATED;
		free(*_vector);
		*_vector = NULL;
	}
}

/*@see Vector_Grow*/
Vector_Result Vector_Append(Vector* _vector, void* _item)
{
	if(NULL == _vector || NULL == _item)
	{
		return VECTOR_UNITIALIZED_ERROR;
	}
	
	/*reallocate more memory for array if capacity is reached*/	
	if(_vector->m_size == _vector->m_capacity)
	{
		if(Vector_Grow(_vector) != VECTOR_SUCCESS)
		{
			return VECTOR_ALLOCATION_ERROR;
		}
	}
	
	_vector->m_items[_vector->m_size] = _item;
	++(_vector->m_size);
	
	return VECTOR_SUCCESS;
}

/*@see Vector_Shrink*/ 
Vector_Result Vector_Remove(Vector* _vector, void** _pValue)
{
	if(NULL == _vector || NULL == _pValue)
	{
		return VECTOR_UNITIALIZED_ERROR;
	}
	
	*_pValue = _vector->m_items[_vector->m_size - 1];
	--(_vector->m_size);
	
	/*shrink array's memory if free space is at least twise as large as extention block and capacity is larger than initial capacity*/	
	if((_vector->m_capacity - _vector->m_size > _vector->m_blockSize * 2) && _vector->m_capacity > _vector->m_initialCapacity)
	{
		if(Vector_Shrink(_vector) != VECTOR_SUCCESS)
		{
			return VECTOR_ALLOCATION_ERROR;
		}
	}
	
	return VECTOR_SUCCESS;
}

Vector_Result Vector_Get(const Vector* _vector, size_t _index, void** _pValue)
{
	if(NULL == _vector || NULL == _pValue)
	{
		return VECTOR_UNITIALIZED_ERROR;
	}
	
	if(_index > _vector->m_size || _index == 0)
	{
		return VECTOR_INDEX_OUT_OF_BOUNDS_ERROR;
	}
	
	*_pValue = _vector->m_items[_index - 1];
	
	return VECTOR_SUCCESS;
}

Vector_Result Vector_Set(Vector* _vector, size_t _index, void*  _value)
{
	if(NULL == _vector || NULL == _value)
	{
		return VECTOR_UNITIALIZED_ERROR;
	}
	
	if(_index > _vector->m_size)
	{
		return VECTOR_INDEX_OUT_OF_BOUNDS_ERROR;
	}
	
	_vector->m_items[_index - 1] = _value;/*potential for memory leak here, if user doesn't have nother pointer on replaced item*/
	
	return VECTOR_SUCCESS;
}

size_t Vector_Size(const Vector* _vector)
{
	if(NULL == _vector)
	{
		return 0;
	}
	
	return _vector->m_size;
}

size_t Vector_Capacity(const Vector* _vector)
{
	if(NULL == _vector)
	{
		return 0;
	}
	
	return _vector->m_capacity;
}

size_t Vector_ForEach(const Vector* _vector, VectorElementAction _action, void* _context)
{
	int itemIndex, numOfItems;
	if(NULL == _vector || NULL == _action)
	{
		return 0;
	}
	
	numOfItems = _vector->m_size;
	
	for(itemIndex = 1; itemIndex <= numOfItems; ++itemIndex)
	{
		if(_action(_vector->m_items[itemIndex - 1], itemIndex, _context) == 0)
		{
			break;
			return itemIndex;
		}
	}
	
	return numOfItems;
}

/***********************************************************************/
/*               Static functions implementation                       */

static Vector_Result Vector_Shrink(Vector* _vector)
{
	size_t newCap;	
	void* temp;
	temp = _vector->m_items;
	/*new capacity is either current capacity minus one extention block or the einitial capacity, whichever is larger*/
	newCap = (_vector->m_capacity - _vector->m_blockSize > _vector->m_initialCapacity ? _vector->m_capacity - _vector->m_blockSize : _vector->m_initialCapacity);
	_vector->m_items = realloc(_vector->m_items, newCap * sizeof(void*));
	if(NULL == _vector->m_items)
	{
		return VECTOR_ALLOCATION_ERROR;
	}
	_vector->m_items = temp;
	_vector->m_capacity = newCap;
	return VECTOR_SUCCESS;
}

static Vector_Result Vector_Grow(Vector* _vector)
{
	void* temp;	
	if(_vector->m_blockSize == 0)
	{
		return VECTOR_ALLOCATION_ERROR;/*TODO: vector insertion error / vector is full*/
	}
	
	temp = _vector->m_items;/*TODO realloc to temp. Also use local variables here*/
	_vector->m_items = realloc(_vector->m_items, (_vector->m_capacity + _vector->m_blockSize) * sizeof(void*));
	if(NULL == _vector->m_items)
	{
		_vector->m_items = temp;
		return VECTOR_ALLOCATION_ERROR;
	}
	_vector->m_capacity += _vector->m_blockSize;
	return VECTOR_SUCCESS;
}










































































































