#ifndef __INTERN_LIST_H__
#define __INTERN_LIST_H__

#include <stddef.h>

typedef struct Node Node;

struct Node
{
	Node* m_prev;
	Node* m_next; 
	void* m_item; 
};

struct List
{
	Node m_head;
	Node m_tail;
	size_t m_numOfItems;
	size_t m_magicNumber;
};

#endif/*__INTERN_LIST_H__*/
