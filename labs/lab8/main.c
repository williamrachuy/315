#include <stdio.h>
#include <stdlib.h>

#define CACHESIM
#define AMAX 10
#define CACHE_SIZE 16
#define SET_SIZE 2

typedef struct {
   unsigned long tag;
   unsigned int data;
   unsigned int valid;
   unsigned int age;
} Block;

typedef struct {
   int size;
   Block block[CACHE_SIZE];
} Cache;

typedef struct {
   int size;
   Cache cache[SET_SIZE];
} Set;

typedef struct {
   unsigned long tag;
   unsigned long index;
   unsigned long offset;
   int tagShift;
   int indexShift;
   int offsetShift;
} Mask;

static Set set;
static Mask mask;
static int m[AMAX][AMAX], a[AMAX][AMAX], b[AMAX][AMAX];
static int hits = 0, misses = 0;

static Mask createMask(void) {
   Mask mask;

   if (CACHE_SIZE == 16) {
      mask.tagShift    = 6;
      mask.indexShift  = 2;
      mask.offsetShift = 0;
      mask.tag         = 0x03FFFFFFFFFFFFF;
      mask.index       = 0x00000000000000F;
      mask.offset      = 0x000000000000003;
   }
   else if (CACHE_SIZE == 256) {
      mask.tagShift    = 10;
      mask.indexShift  = 2;
      mask.offsetShift = 0;
      mask.tag         = 0x003FFFFFFFFFFFF;
      mask.index       = 0x0000000000000FF;
      mask.offset      = 0x000000000000003;
   }
   else {
      fprintf(stderr, "Invalid cache size [ 16 | 256 ]\n");
      exit(EXIT_FAILURE);
   }
   return mask;
}

static void getRowColumn(int *r1, int *c1, int *r2, int *c2) {
   printf("Enter row and column for first matrix: ");
   scanf("%d%d", r1, c1);
   printf("Enter row and column for second matrix: ");
   scanf("%d%d", r2, c2);
}

static Set createSet(void) {
   int i, j;
   Set set;

   set.size = SET_SIZE;
   for (i = 0; i < SET_SIZE; i++) {
      set.cache[i].size = CACHE_SIZE;
      for (j = 0; j < CACHE_SIZE; j++) {
         set.cache[i].block[j].tag   = 0;
         set.cache[i].block[j].data  = 0;
         set.cache[i].block[j].valid = 0;
         set.cache[i].block[j].age   = 0;
      }
   }
   return set;
}

static void memRead(int *mp) {
   printf("memory read from location %p\n", mp);
}

static void memWrite(int *mp) {
   printf("Memory write to location %p\n", mp);
}

static void matMult(int r1, int c1, int c2) {
   int i, j, k;
   int *mPtr, *aPtr, *bPtr;

   for (i = 0; i < r1; i++)
      for (j = 0; j < c2; j++)
         m[i][j] = 0;
   for (i = 0; i < r1; i++) {
      for (j = 0; j < c2; j++) {
         for (k = 0; k < c1; k++) {
            #ifdef CACHESIM
            mPtr = &m[i][j];
            aPtr = &a[i][k];
            bPtr = &b[k][j];
            memRead(mPtr);
            memRead(aPtr);
            memRead(bPtr);
            memWrite(mPtr);
            #endif
            m[i][j] += a[i][k] * b[k][j];
         }
      }
   }
}

int main(void) {
   int r1, c1, r2, c2;
   int i, j;

   set = createSet();
   mask = createMask();

   getRowColumn(&r1, &c1, &r2, &c2);
   while (c1 != r2) {
      printf("Error! Column of first matrix not equal to row of second\n");
      getRowColumn(&r1, &c1, &r2, &c2);
   }
   for (i = 0; i < r1; i++) {
      for (j = 0; j < c1; j++) {
         #ifdef MANUAL
         printf("Enter elements a[%d][%d]: ", i + 1, j + 1);
         scanf("%d", &a[i][j]);
         #endif
         #ifndef MANUAL
         a[i][j] = i + j;
         #endif
      }
   }
   for (i = 0; i < r2; i++) {
      for (j = 0; j < c2; j++) {
         #ifdef MANUAL
         printf("Enter elements b[%d][%d]: ", i + 1, j + 1);
         scanf("%d", &b[i][j]);
         #endif
         #ifndef MANUAL
         b[i][j] = i + j;
         #endif
      }
   }
   matMult(r1, c1, c2);
   printf("\nOutput matrix:\n");
   for (i = 0; i < r1; i++) {
      for (j = 0; j < c2; j++) {
         printf("%04d ", m[i][j]);
         if (j == (c2 - 1))
            printf("\n\n");
      }
   }

   exit(EXIT_SUCCESS);
}