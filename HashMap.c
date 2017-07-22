#include <stddef.h>  /* size_t */
#include <stdlib.h>
#include <stdio.h>
#include "HashMap.h"
#include "LinkedListItr.h"
#include "LinkedList.h"
#include "list_functions.h"

#define ALIVE 0xbeefbeef
#define DEAD  0xdeadbeef
#define TRUE  1
#define FALSE 0

/*--------------------------------------------*/
struct HashMap {
  size_t m_magicNumber;
  size_t m_numOfItems;
  size_t m_spaceAvailable;
  size_t m_capacity;
  List** m_lists;
  HashFunction m_hashFunction;
  EqualityFunction m_isEqual;
};

/*--------------------------------------------*/
typedef struct Item
{
  void* m_key;
  void* m_value;
}Item;

/*--------------------------------------------*/
typedef void(*DestructionFunction)(void* _item);

/*--------------------------------------------*/
typedef struct Destructor
{
  DestructionFunction m_destroyKey;
  DestructionFunction m_destroyValue;
} Destructor;

/*--------------------------------------------*/
typedef struct ActionAndContext
{
  KeyValueActionFunction m_action;
  void* m_context;
} ActionAndContext;

static int KeyValueAction(Item* _item, ActionAndContext* _actionAndContext)
{
  return _actionAndContext->m_action(_item->m_key, _item->m_value, _actionAndContext->m_context);
}

/*--------------------------------------------*/
typedef struct KeyAndComparator
{
  void* m_key;
  EqualityFunction m_isEqual;
} KeyAndComparator;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int isPrime(int size)
{
  int i;
  for(i = 3; i < size/2; ++i)
  {
    if (size%i == 0)
    {
      return FALSE;
    }
  }
  return TRUE;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static int FindHashSize(size_t _capacity)
{
  int size;
  size = _capacity + (_capacity * .3);
  while (FALSE == isPrime(size))
  {
    size++;
  }
  return size;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
static List* FindMyList(HashMap* _map, void* _key)
{
  size_t hashPlacement;
  List* foundList = NULL;

  hashPlacement = _map->m_hashFunction(_key);
  foundList = _map->m_lists[hashPlacement % _map->m_capacity];
  return foundList;
}
*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void ListItemDestroy(Item* _item, Destructor* _destructor)
{
  if(NULL != _destructor->m_destroyKey)
  {
    _destructor->m_destroyKey(_item->m_key);
  }
  if(NULL != _destructor->m_destroyValue)
  {
    _destructor->m_destroyValue(_item->m_value);
  }
  free(_item);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static int DoesListHaveKey(Item* _item,  KeyAndComparator* _keyAndComparator)
{
  return _keyAndComparator->m_isEqual(_item->m_key, _keyAndComparator->m_key);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static ListItr FoundKey(List* _list, void* _key, int (*_EqualityFunction)(const void* _firstKey, const void* _secondKey))
{
  ListItr begin = NULL;
  ListItr end = NULL;
  ListItr result = NULL;
  KeyAndComparator keyAndComparator;

  begin = ListItr_Begin(_list); /* get the start of the list we're on */
  end = ListItr_End(_list);     /* get the end of the list we're on */

  keyAndComparator.m_key = _key;
  keyAndComparator.m_isEqual = _EqualityFunction;
  result = ListItr_FindFirst(begin, end, (PredicateFunction) DoesListHaveKey, &keyAndComparator);
  return (result == end ? NULL : result);
}
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~API~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

HashMap* HashMap_Create(size_t _capacity, HashFunction _hashFunc, EqualityFunction _keysEqualFunc)
{
  HashMap* newHM = NULL;
  List** newLists;
  size_t newCapacity;
  if ((0 >= _capacity) || (NULL == _hashFunc) || (NULL == _keysEqualFunc))
  {
    return NULL;
  }

  newHM = (HashMap*) malloc(sizeof(HashMap));
  if (NULL == newHM)
  {
    return NULL;
  }
  newCapacity = FindHashSize(_capacity);
  newLists = (List**) calloc(newCapacity, sizeof(List*));
  if (NULL == newLists)
  {
    free(newHM);
    return NULL;
  }

  newHM->m_lists = newLists;
  newHM->m_magicNumber = ALIVE;
  newHM->m_numOfItems = 0;
  newHM->m_capacity = newCapacity;
  newHM->m_hashFunction = _hashFunc;
  newHM->m_isEqual = _keysEqualFunc;
  newHM->m_spaceAvailable = newHM->m_capacity;

  return newHM;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void HashMap_Destroy(HashMap** _map, void (*_keyDestroy)(void* _key), void (*_valDestroy)(void* _value))
{
  int i, capacity = 0;
  Destructor destructor;
  ListItr begin, end;
  if ((NULL != _map) && (NULL != *_map) && (ALIVE == (*_map)->m_magicNumber))
  {
    capacity = (*_map)->m_capacity;
    destructor.m_destroyKey = _keyDestroy;
    destructor.m_destroyValue = _valDestroy;
    for (i = 0; i < capacity; i++)
    {
      if(NULL != ((*_map)->m_lists[i]))
      {
        begin = ListItr_Begin((*_map)->m_lists[i]);
        end = ListItr_End((*_map)->m_lists[i]);
        ListItr_ForEach(begin, end, (ListActionFunction) ListItemDestroy, &destructor);
        List_Destroy(&((*_map)->m_lists[i]), NULL);
      }
    }
    free((*_map)->m_lists);
    (*_map)->m_magicNumber = DEAD;
    free(*_map);
    (*_map) = NULL;
  }
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*Map_Result HashMap_Rehash(HashMap *_map, size_t newCapacity)
{
  HashMap* updatedHM = NULL;
  void* itrValue = NULL;
  updatedHM = HashMap_Create(_newcapacity, _map->m_hashFunction, _map->m_isEqual);
  for (i = 0; i < _map->m_capacity; i++)
    {
      list = ((*_map)->m_lists)[i];
      if(NULL != list)
      {
        begin = ListItr_Begin(list);  first we need to get the first node of the list, which is the head */
        /*begin = ListItr_Next(begin); *//* Once we get into the head node, we take the next which is the actual first
        end = ListItr_End(list);
        while ( begin != end)
        {
          itrValue = ListItr_Get(begin);
          ListItr_ForEach(begin, end, (ListActionFunction) HashMap_Insert(), NULL);
          begin = ListItr_Next(begin);
        }

        ListItr_ForEach(begin, end, (ListActionFunction) ListItemDestroy, &destructor);
        List_Destroy(&list, NULL);
      }
    }*/
  /*
  ListItr ListItr_ForEach(ListItr _begin, ListItr _end, ListActionFunction _action, void* _context);
  Map_Result HashMap_Insert(HashMap* _map, const void* _key, const void* _value);
  Map_Result HashMap_Remove(HashMap* _map, const void* _searchKey, void** _pValue);
  void HashMap_Destroy(HashMap** _map, void (*_keyDestroy)(void* _key), void (*_valDestroy)(void* _value));
}


Map_Result HashMap_Rehash(HashMap* _map, size_t newCapacity)
{
  List** temp = NULL;
  if (NULL == temp)
  {
    return MAP_ALLOCATION_ERROR;
  }

  temp = (List**) realloc(_map->m_lists, FindHashSize(newCapacity) * sizeof(List*));
  if (NULL == temp)
  {
    return MAP_ALLOCATION_ERROR;
  }
  _map->m_lists = temp;

  return MAP_SUCCESS;
}
*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Map_Result HashMap_Insert(HashMap* _map, const void* _key, const void* _value)
{
  Item* item = NULL;
  List* newList = NULL;
  List* foundList;
  size_t hashPlacement;
  if ((NULL == _map)|| (NULL == _key) || (NULL == _value))
  {
    return MAP_UNINITIALIZED_ERROR;
  }

  item = (Item*) malloc(sizeof(Item));
  if (NULL == item)
  {
    return MAP_ALLOCATION_ERROR;
  }

  item->m_key = ((void*)_key);
  item->m_value = ((void*)_value);
  hashPlacement = _map->m_hashFunction(_key);
  foundList = _map->m_lists[hashPlacement % _map->m_capacity];
  if (NULL == foundList)
  {
    newList = List_Create();
    if (NULL == newList)
    {
      return MAP_ALLOCATION_ERROR;
    }
    _map->m_lists[hashPlacement % _map->m_capacity] = newList;
    List_PushHead(newList, item);
    _map->m_numOfItems++;
  }
  else if (NULL == FoundKey(foundList, ((void*)_key), _map->m_isEqual))
  {
    List_PushHead(foundList, item);
    _map->m_numOfItems++;
  }
  else
  {
    return MAP_KEY_DUPLICATE_ERROR;
  }
  return MAP_SUCCESS;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Map_Result HashMap_Remove(HashMap* _map, const void* _searchKey, void** _pKey, void** _pValue)
{
  size_t hashValue = 0;
  List* list = NULL;
  ListItr iter;

  hashValue = _map->m_hashFunction(_searchKey);
  list = _map->m_lists[hashValue % _map->m_capacity];
  iter = FoundKey(list, ((void*)_searchKey), _map->m_isEqual);
  if (NULL == iter)
  {
    return MAP_KEY_NOT_FOUND_ERROR;
  }
  *_pValue = ((Item*)ListItr_Remove(iter))->m_value;
  /* *_pKey = ((Item*)ListItr_Remove(iter))->m_key;  THIS IS MY PROBLEM */

  _map->m_numOfItems--;
  return MAP_SUCCESS;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Map_Result HashMap_Find(const HashMap* _map, const void* _searchKey, void** _ppValue)
{
  size_t hashValue = 0;
  List* list = NULL;
  ListItr iter;

  hashValue = _map->m_hashFunction(_searchKey);
  list = _map->m_lists[hashValue % _map->m_capacity];
  iter = FoundKey(list, ((void*)_searchKey), _map->m_isEqual);
  if (NULL == iter)
  {
    return MAP_KEY_NOT_FOUND_ERROR;
  }
  *_ppValue = ListItr_Get(iter);
  return MAP_SUCCESS;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

size_t HashMap_Size(const HashMap* _map)
{
  return ((NULL == _map) ? 0 : _map->m_numOfItems);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

size_t HashMap_ForEach(const HashMap* _map, KeyValueActionFunction _action, void* _context)
{
  int i, capacity = 0;
  List* list;
  ListItr begin, end;
  size_t iterations = 0;
  ActionAndContext actionAndContext;

  if ((NULL == _map)|| (NULL == _action))
  {
    return MAP_UNINITIALIZED_ERROR;
  }

  actionAndContext.m_action = _action;
  actionAndContext.m_context = _context;

  capacity = _map->m_capacity;


  for( i = 0; i < capacity; ++i)
    {
      list = (_map->m_lists)[i];

      if(NULL != list)
      {
        begin = ListItr_Begin(list);
        begin = ListItr_Next(begin);
        end = ListItr_End(list);
        iterations += ListItr_CountIf(begin, end, (PredicateFunction) KeyValueAction, &actionAndContext);
      }
    }

  return iterations;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
