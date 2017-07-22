#include <stdio.h>
#include "list_functions.h"

static void PadList(ListItr _destination, ListItr _begin, ListItr _end);
static void Swap(void** x, void** y);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

ListItr ListItr_FindFirst(ListItr _begin, ListItr _end, PredicateFunction _predicate, void* _context)
{
	ListItr currentItr;

	currentItr  =  _begin;
	while(!ListItr_Equals(currentItr,_end))
	{
		if(_predicate(ListItr_Get(currentItr),_context))
		{
			break;
		}
		currentItr  =  ListItr_Next(currentItr);
	}

	return currentItr;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

size_t ListItr_CountIf(ListItr _begin, ListItr _end, PredicateFunction _predicate, void* _context)
{
	ListItr currentItr;
	size_t count = 0;

	currentItr =  _begin;
	while( !ListItr_Equals(currentItr,_end))
	{
		if(_predicate(ListItr_Get(currentItr),_context))
		{
			++count;
		}
		currentItr = ListItr_Next(currentItr);
	}

	return count;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

ListItr ListItr_ForEach(ListItr _begin, ListItr _end, ListActionFunction _action, void* _context)
{
	ListItr currentItr;

	currentItr =  _begin;
	while( !ListItr_Equals(currentItr,_end) )
	{
		if(!_action(ListItr_Get(currentItr),_context))
		{
			break;
		}
		currentItr = ListItr_Next(currentItr);
	}

	return currentItr;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void Swap (void** x, void** y)
{
	void* temp;

	temp = *x;
	*x = *y;
	*y = temp;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void ListItr_Sort(ListItr _begin, ListItr _end, LessFunction _less)
{
	ListItr currentItr;
	void *element1,*element2;
	int flag = 1;

	while(flag)
	{
		flag = 0;
		currentItr = _begin;
		while(! ListItr_Equals(ListItr_Next(currentItr),_end))
		{
			element1 = ListItr_Get(currentItr);
			element2 = ListItr_Get((ListItr_Next(currentItr)));

			if(!(_less(element1,element2)))
			{
				Swap(currentItr,ListItr_Next(currentItr));
				flag = 1;
			}
			currentItr = ListItr_Next(currentItr);
		}
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

ListItr ListItr_Splice(ListItr _dest, ListItr _begin, ListItr _end)
{
	ListItr currentItr;

	currentItr = _begin;
	while(!ListItr_Equals(currentItr,_end))
	{
		ListItr_InsertBefore(_dest,ListItr_Get(currentItr));
		ListItr_Remove(currentItr);
		currentItr = ListItr_Next(currentItr);
	}

	return _begin;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

ListItr ListItr_Merge(ListItr _destBegin, ListItr _firstBegin, ListItr _firstEnd,ListItr _secondBegin, ListItr _secondEnd, LessFunction _less)
{
	ListItr currentFirstItr,currentSecondItr,temp;
	void *element1,*element2;

	currentFirstItr = _firstBegin;
	currentSecondItr = _secondBegin;

	while(!ListItr_Equals(currentFirstItr,_firstEnd) && !ListItr_Equals(currentSecondItr,_secondEnd))
	{
		element1 = ListItr_Get(currentFirstItr);
		element2 = ListItr_Get(currentSecondItr);
		if(NULL !=  _less && (_less(element1,element2)))
		{
			temp = currentFirstItr;
			ListItr_InsertBefore(_destBegin,element1);
			ListItr_Remove(currentFirstItr);
			currentFirstItr = ListItr_Next(temp);
		} else
		{
			temp = currentSecondItr;
			ListItr_InsertBefore(_destBegin,element2);
			ListItr_Remove(currentSecondItr);
			currentSecondItr = ListItr_Next(temp);
		}
	}

	PadList(_destBegin,currentFirstItr,_firstEnd);
	PadList(_destBegin,currentSecondItr,_secondEnd);

	return _destBegin;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void PadList(ListItr _destination, ListItr _begin, ListItr _end)
{
	void * element;
	ListItr temp;

	while(!ListItr_Equals(_begin,_end))
	{
		temp = _begin;
		element = ListItr_Remove(_begin);
		ListItr_InsertBefore(_destination,element);
		_begin = ListItr_Next(temp);
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

List* ListItr_Cut(ListItr _begin, ListItr _end)
{
	List* ptrLs;
	ListItr currentItr;

	ptrLs = List_Create();
	currentItr =  _begin;

	while( !ListItr_Equals(currentItr,_end))
	{
		List_PushTail(ptrLs,ListItr_Remove(currentItr));
		currentItr = ListItr_Next(currentItr);
	}

	return ptrLs;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

List* ListItr_Unique(ListItr _begin, ListItr _end, EqualsFunction _equals)
{
	List* ptrLs;
	ListItr currentItr;
	void *element1,*element2;

	ptrLs = List_Create();
	currentItr = _begin;

	while(!ListItr_Equals(ListItr_Next(currentItr),_end))
	{
		element1 = ListItr_Get(currentItr);
		element2 = ListItr_Get(ListItr_Next(currentItr));
		if(_equals(element1,element2))
		{
			List_PushTail(ptrLs,ListItr_Remove(currentItr));
		}
		currentItr = ListItr_Next(currentItr);
	}
	return ptrLs;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
