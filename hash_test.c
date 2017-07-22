#include "HashMap.h"
#include <string.h>
#include "mu_test.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Rect
{
  size_t m_x;
  size_t m_y;
} Rect;
/*--------------------------------------------*/
int RectHash(int* _num)
{
  return *_num;
}
/*--------------------------------------------*/
int NumEqFunc(int* _num1, int* _num2)
{
  return (*_num1 == *_num2);
}
/*--------------------------------------------*/
int hash(char *str)
{
  int hash = 5381;
  int c;

  while ((c = *(str++)))
  hash = ((hash << 5) + hash) + c;

  return hash;
}
/*--------------------------------------------*/
int KeyEqualFunc(char* _str1, char* _str2)
{
  return (strcmp(_str1, _str2) == 0 ? 1 : 0);
}
/*--------------------------------------------*/
int Strrev(char* _key, char* _str, void* _context)
{
      char* p1;
    char* p2;
    char temp;

      for (p1 = _str, p2 = _str + strlen(_str) - 1; p2 > p1; ++p1, --p2)
      {
      temp = *p1;
      *p1 = *p2;
      *p2 = temp;
      }
      return 1;
}
/*--------------------------------------------*/
int DestroyItem(void* _item)
{
  free(_item);
  return 1;
}
/*--------------------------------------------*/


UNIT(creation)
  HashMap* hm;
  hm = HashMap_Create(128, (HashFunction) hash, (EqualityFunction) KeyEqualFunc);
  ASSERT_THAT(NULL != hm);
END_UNIT

UNIT(destruction)
  HashMap* hm;
  hm = HashMap_Create(128, (HashFunction) hash, (EqualityFunction) KeyEqualFunc);
  HashMap_Destroy(&hm, NULL, NULL);
  ASSERT_THAT(NULL == hm);
END_UNIT

UNIT(insert)
  HashMap* hm;
  char* res;
  hm = HashMap_Create(128, (HashFunction) hash, (EqualityFunction) KeyEqualFunc);
  HashMap_Insert(hm, "key", "value");
  HashMap_Insert(hm, "somekey", "value2");
  HashMap_Insert(hm, "monkey", "value3");
  ASSERT_THAT(HashMap_Find(hm, "key", (void**)&res) == MAP_SUCCESS);
  ASSERT_THAT(strcmp(res, "value") == 0);
  HashMap_Destroy(&hm, NULL, NULL);
  ASSERT_THAT(NULL == hm);
END_UNIT

UNIT(remove_item)
  HashMap* hm;
  char* res,*key;
  hm = HashMap_Create(128, (HashFunction) hash, (EqualityFunction) KeyEqualFunc);
  HashMap_Insert(hm, "key", "value");
  ASSERT_THAT(HashMap_Remove(hm, "key",(void**) &key, (void**)&res) == MAP_SUCCESS);
  ASSERT_THAT(strcmp(res, "value") == 0);
  HashMap_Destroy(&hm, NULL, NULL);
  ASSERT_THAT(NULL == hm);
END_UNIT

UNIT(remove_item_twice)
  HashMap* hm;
  char* res,*key;
  hm = HashMap_Create(128, (HashFunction) hash, (EqualityFunction) KeyEqualFunc);
  HashMap_Insert(hm, "key", "value");
  ASSERT_THAT(HashMap_Remove(hm, "key",(void**) &key, (void**)&res) == MAP_SUCCESS);
  ASSERT_THAT(strcmp(res, "value") == 0);
  ASSERT_THAT(HashMap_Remove(hm, "key",(void**) &key, (void**)&res) == MAP_KEY_NOT_FOUND_ERROR);
  HashMap_Destroy(&hm, NULL, NULL);
  ASSERT_THAT(NULL == hm);
END_UNIT

UNIT(for_each)
  HashMap* hm;
  char* res,*key;
  char k[] = "key";
  char v[] = "value";
  hm = HashMap_Create(128, (HashFunction) hash, (EqualityFunction) KeyEqualFunc);
  HashMap_Insert(hm, k, v);
  HashMap_ForEach(hm, (KeyValueActionFunction) Strrev, NULL);
  ASSERT_THAT(HashMap_Remove(hm, "key",(void**) &key, (void**)&res) == MAP_SUCCESS);
  ASSERT_THAT(strcmp(res, "eulav") == 0);
  HashMap_Destroy(&hm, NULL, NULL);
  ASSERT_THAT(NULL == hm);
END_UNIT

TEST_SUITE(Generic HashMap Tests)
  TEST(creation)
  TEST(destruction)
  TEST(insert)
  TEST(remove_item)
  TEST(remove_item_twice)
  TEST(for_each)
END_SUITE
