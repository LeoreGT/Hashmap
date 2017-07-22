#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"
#include "LinkedListItr.h"

#define ALIVE 0xbeefbeef
#define DEAD 0xdeadbeef
#define TRUE 1
#define FALSE 0

#define CHECK(input, inputAgainst, output) if(inputAgainst == input) \
                              { \
                              return output; \
                              }
#define CHECK_VOID(input, inputAgainst) if(inputAgainst == input) \
                              { \
                              return; \
                              }

typedef struct Node Node;

static Node*        NodeCreate(void* _data);
static List_Result  AddToList(Node* _addTo ,void* _data);
static void         RemoveFromList(Node* _removeFrom ,void** _data);

struct Node
{
	void* m_data;
	Node* m_next;
	Node* m_previous;
};

struct List
{
	Node m_headNode;
	Node m_tailNode;
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
List* List_Create(void)
{
	List* tempList;

	tempList=(List*) malloc(1*sizeof(List));

	CHECK(NULL, tempList, NULL);

	tempList->m_headNode.m_next = &(tempList->m_tailNode);
	tempList->m_tailNode.m_previous = &(tempList->m_headNode);

  tempList->m_headNode.m_previous = NULL;
	tempList->m_tailNode.m_next = NULL;

	tempList->m_tailNode.m_data = (void*)ALIVE;

	return tempList;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void DestroyNode(Node* _ptrNode)
{
  CHECK_VOID(NULL, _ptrNode);
	free(_ptrNode);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void List_Destroy(List** _ptrList, void (*_elementDestroy)(void* _item))
{
	Node* tempNode;

  CHECK_VOID(NULL, _ptrList);
  CHECK_VOID(NULL, *_ptrList);
  CHECK_VOID((void*) DEAD, (*_ptrList)->m_tailNode.m_data);

	tempNode = (*_ptrList)->m_headNode.m_next;

	while( tempNode->m_next != NULL )
	{
		if(NULL == _elementDestroy)
		{
			_elementDestroy(tempNode->m_data);
		}
		tempNode = tempNode->m_next;
		DestroyNode(tempNode->m_previous);
	}

	(*_ptrList)->m_tailNode.m_data = (void*) DEAD;
	free(*_ptrList);
	*_ptrList=NULL;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static Node* NodeCreate(void* _data)
{
	Node* tempNode;

	tempNode = (Node*) malloc(1*sizeof(Node));
	if (NULL == tempNode)
	{
		return NULL;
	}

	tempNode->m_data = _data;
	return tempNode;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static List_Result AddToList(Node* _addTo ,void* _data)
{
	Node* tempNode;

	tempNode = NodeCreate(_data);
  CHECK(NULL, tempNode, LIST_ALLOCATION_ERROR);

	tempNode->m_previous = _addTo->m_previous;
	tempNode->m_next = _addTo;
	_addTo->m_previous->m_next = tempNode;
	_addTo->m_previous = tempNode;

	return LIST_SUCCESS;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

List_Result List_PushHead(List* _list, void* _item)
{

  CHECK(NULL, _list, LIST_UNINITIALIZED_ERROR);
  CHECK(NULL, _item, LIST_NULL_ELEMENT_ERROR);

	return AddToList(_list->m_headNode.m_next,_item);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

List_Result List_PushTail(List* _list, void* _item)
{
	CHECK(NULL, _list, LIST_UNINITIALIZED_ERROR);
  CHECK(NULL, _item, LIST_NULL_ELEMENT_ERROR);

	return AddToList(&(_list->m_tailNode),_item);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void RemoveFromList(Node* _removeFrom ,void** _data)
{
	*_data = _removeFrom->m_data;
	_removeFrom->m_next->m_previous = _removeFrom->m_previous;
	_removeFrom->m_previous->m_next = _removeFrom->m_next;

	DestroyNode(_removeFrom);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

List_Result List_PopHead(List* _list, void** _pItem)
{
  CHECK(NULL, _list, LIST_UNINITIALIZED_ERROR);
  CHECK(NULL, _pItem, LIST_UNINITIALIZED_ERROR);
  CHECK(0, List_Size(_list), LIST_UNDERFLOW_ERROR);

	RemoveFromList(_list->m_headNode.m_next,_pItem);
	return LIST_SUCCESS;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

List_Result List_PopTail(List* _list, void** _pItem)
{
  CHECK(NULL, _list, LIST_UNINITIALIZED_ERROR);
  CHECK(NULL, _pItem, LIST_UNINITIALIZED_ERROR);
  CHECK(0, List_Size(_list), LIST_UNDERFLOW_ERROR);

	RemoveFromList(_list->m_tailNode.m_previous,_pItem);
	return LIST_SUCCESS;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

size_t List_Size(const List* _list)
{
	int count=0;
	Node *ptr;

  CHECK(NULL, _list, 0);

	ptr=_list->m_headNode.m_next;
	while(NULL != ptr->m_next)
	{
		count++;
		ptr = ptr->m_next;
	}

	return count;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void NodePrint(void* _node)
{
	printf("\nData: %d\nPrev: %d\nNext Node: %d\n",(int) ((Node*)_node)->m_previous, *(int*) ((Node*)_node)->m_data, (int) ((Node*)_node)->m_next);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void ListPrint(List* _list)
{
	Node* headNode;

  CHECK_VOID(NULL, _list)

	headNode =_list->m_headNode.m_next;
	while(headNode->m_next != NULL)
	{
		NodePrint(headNode);
		headNode = headNode->m_next;
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

ListItr ListItr_Begin(const List* _list)
{
  CHECK(NULL, _list, NULL)
	return _list->m_headNode.m_next;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

ListItr ListItr_End(const List* _list)
{
	CHECK(NULL, _list, NULL)
	return (ListItr) &(_list->m_tailNode);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int ListItr_Equals(const ListItr _a, const ListItr _b)
{
	CHECK(_a, _b, TRUE);
	return FALSE;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

ListItr ListItr_Next(ListItr _itr)
{
	CHECK(NULL, _itr, NULL);
  CHECK(NULL, ((Node*)_itr)->m_next, _itr) /* if itr points to the end of the list */

	return ((Node*)_itr)->m_next;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

ListItr ListItr_Prev(ListItr _itr)
{
	CHECK(NULL, _itr, NULL);
  CHECK(NULL, ((Node*)_itr)->m_previous, _itr) /* if itr points to the end of the list */

	return ((Node*)_itr)->m_previous;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void* ListItr_Get(ListItr _itr)
{
	CHECK(NULL, _itr, NULL);
  CHECK(NULL, ((Node*)_itr)->m_next, NULL) /* if itr points to the end of the list */

	return ((Node*)_itr)->m_data;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void* ListItr_Set(ListItr _itr, void* _element)
{
	void* temp;
  CHECK(NULL, _itr, NULL);
  CHECK(NULL, ((Node*)_itr)->m_next, NULL) /* if itr points to the end of the list */

	temp = ((Node*)_itr)->m_data;
	((Node*)_itr)->m_data=_element;

	return temp;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

ListItr ListItr_InsertBefore(ListItr _itr, void* _element)
{
	CHECK(NULL, _itr, NULL);
  CHECK(NULL, _element, NULL) /* if itr points to the end of the list */
  CHECK(LIST_ALLOCATION_ERROR, AddToList((Node*)_itr,_element), NULL);

	return ((Node*)_itr)->m_previous;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void* ListItr_Remove(ListItr _itr)
{
	void* temp;
  CHECK(NULL, _itr, NULL);

	RemoveFromList((Node*)_itr,&temp);
	return temp;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
