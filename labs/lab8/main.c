/*******************************************************************************
 *
 * LAB 8 : CACHE SIMULATION
 *
 * CPE 315-02; D. RETZ
 *
 * CODED BY: CAMERON SIMPSON, WILLIAM RACHUY
 *
 * DATE : MARCH 16, 2018
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

/* Program specific defines. Do not modify.
 */
#define TRUE 1
#define FALSE 0
#define MISS -1

/* Defines to alter program behavior.
 * "CACHESIM 1" will enable the cache simulation behavior.
 * "VERBOSE 1" will enable verbose output to the terminal.
 * "MANUAL 1" will enable the user to input matrix values manually.
 * "CACHEPRINT 1" will enable the cache set to be printed to the terminal.
 */
#define CACHESIM 1
#define VERBOSE 0
#define MANUAL 0
#define CACHEPRINT 1

/* Defines modifying the structure of the cache.
 * "AMAX N" will set the maximum row/column value N the user can input.
 * "CACHE_SIZE N" will set the size N of cache block in each cache table.
 * "ASSOC_SIZE N" will set the size N of cache sets in the overall cache.
 */
#define AMAX 100
#define CACHE_SIZE 16
#define ASSOC_SIZE 4

/* Structure for converting memory pointer into address segments.
 */
typedef struct {
   unsigned long tag;
   unsigned long index;
   unsigned long offset;
} Address;

/* Structure for storing cache memory.
 */
typedef struct {
   unsigned long tag;
   unsigned int  valid;
   unsigned int  age;
   signed int    data;
} Block;

/* Structure for the cache table.
 */
typedef struct {
   int size;
   Block block[CACHE_SIZE];
} Cache;

/* Structure for storing multiple cache tables depending on associativity.
 */
typedef struct {
   int size;
   Cache cache[ASSOC_SIZE];
} Set;

/* Structure used in conjunction with creating an address struct.
 */
typedef struct {
   unsigned long tag;
   unsigned long index;
   unsigned long offset;
   int tagShift;
   int indexShift;
   int offsetShift;
} Mask;

/* Define global variables.
 */
static Set set;
static Mask mask;
static int m[AMAX][AMAX], a[AMAX][AMAX], b[AMAX][AMAX];
static int hits = 0, misses = 0;
static int reads = 0, writes = 0;

/* Creates an address mask based on the value of CACHE_SIZE.
 */
static Mask createMask(void) {
   Mask mask;

   if (CACHE_SIZE == 16) {
      mask.tagShift    = 6;
      mask.indexShift  = 2;
      mask.offsetShift = 0;
      mask.tag         = 0x03FFFFFFFFFFFFFF;
      mask.index       = 0x000000000000000F;
      mask.offset      = 0x0000000000000003;
   }
   else if (CACHE_SIZE == 256) {
      mask.tagShift    = 10;
      mask.indexShift  = 2;
      mask.offsetShift = 0;
      mask.tag         = 0x003FFFFFFFFFFFFF;
      mask.index       = 0x00000000000000FF;
      mask.offset      = 0x0000000000000003;
   }
   else {
      fprintf(stderr, "Invalid cache size: %d [ 16 | 256 ]\n", CACHE_SIZE);
      exit(EXIT_FAILURE);
   }
   return mask;
}

/* Function to get row and column size of two matricies.
 */
static void getRowColumn(int *r1, int *c1, int *r2, int *c2) {
   printf("\n");
   printf("Enter row and column for  first matrix: ");
   scanf("%d%d", r1, c1);
   printf("Enter row and column for second matrix: ");
   scanf("%d%d", r2, c2);
   printf("\n");
}

/* Function to create an initalized cache set.
 */
static Set createSet(void) {
   int i, j;
   Set set;

   set.size = ASSOC_SIZE;
   for (i = 0; i < ASSOC_SIZE; i++) {
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

/* Function to create an Address struct for comparing with cache blocks.
 */
static Address createAddress(int *mp) {
   Address address;

   address.tag    = ((unsigned long)mp >> mask.tagShift)    & mask.tag;
   address.index  = ((unsigned long)mp >> mask.indexShift)  & mask.index;
   address.offset = ((unsigned long)mp >> mask.offsetShift) & mask.offset;
   return address;
}

/* Function to display the cache in the terminal.
 */
static void printCacheSet(void) {
   int i, j;

   for (i = 0; i < ASSOC_SIZE; i++) {
      printf("CACHE %d\n", i);
      for (j = 0; j < CACHE_SIZE; j++) {
         printf("[ V : %d ][ A : %04d ][ T : 0x%08lX ][ D : 0x%08X ]\n",
            set.cache[i].block[j].valid, set.cache[i].block[j].age,
            set.cache[i].block[j].tag, set.cache[i].block[j].data);
      }
      printf("\n");
   }
}

/* Function to lookup an entry in the cache set. Returns -1 if entry is not
 * found. Otherwise, returns the index of the set that the entry was found in.
 */
static int cacheLookup(int *mp) {
   Address address = createAddress(mp);
   int i, iReturn = -1;

   for (i = 0; i < set.size; i++) {
      if (set.cache[i].block[address.index].valid == TRUE) {
         set.cache[i].block[address.index].age++;
         if (set.cache[i].block[address.index].tag == address.tag) {
            set.cache[i].block[address.index].age = 0;
            iReturn = i;
         }
      }
   }
   return iReturn;
}

/* Function to write entry into a cache block. Returns the index of the cache
 * set that the entry was written to.
 */
static int cacheWrite(Address address, int *mp, int i) {
   set.cache[i].block[address.index].tag   = address.tag;
   set.cache[i].block[address.index].data  = *mp;
   set.cache[i].block[address.index].valid = TRUE;
   set.cache[i].block[address.index].age   = 0;
   return i;
}

/* Function to initiate a cache block insertion. The function first searches for
 * any invalid blocks in the cache set to insert the entry into. If no invalid
 * block is found, the function then searches for the block with the oldest
 * access time (LRU algorithm) to insert the entry into. The function returns
 * the index of the cache set that the entry was written into.
 */
static int cacheInsert(int *mp) {
   Address address = createAddress(mp);
   int i, iOldest = 0;

   for (i = 0; i < set.size; i++) {
      if (set.cache[i].block[address.index].valid == FALSE)
         return cacheWrite(address, mp, i);
   }
   for (i = 0; i < set.size; i++) {
      iOldest = (set.cache[iOldest].block[address.index].age >
         set.cache[i].block[address.index].age) ?
         iOldest : i;
   }
   return cacheWrite(address, mp, iOldest);
}
/* Function to read memory using the DRAM memory pointer "mp". Increments "hits"
 * or "misses" depending on whether the memory pointer's data was found in the
 * cache set. Increments "reads".
 */
static void memRead(int *mp) {
   if (VERBOSE)
      printf("   Memory R: 0x%016lX ", (unsigned long)mp);
   if (cacheLookup(mp) == MISS) {
      cacheInsert(mp);
      if (VERBOSE)
         printf("-\n");
      misses++;
   }
   else {
      if (VERBOSE)
         printf("+\n");
      hits++;
   }
   reads++;
}

/* Function to write memory using the DRAM memory pointer "mp". Increments
 * "hits or "misses" depending on whether the memory pointer's data was found in
 * the cache set. Increments "writes".
 */
static void memWrite(int *mp) {
   if (VERBOSE)
      printf("   Memory W: 0x%016lX ", (unsigned long)mp);
   if (cacheLookup(mp) == MISS) {
      cacheInsert(mp);
      if (VERBOSE)
         printf("-\n");
      misses++;
   }
   else {
      if (VERBOSE)
         printf("+\n");
      hits++;
   }
   writes++;
}

/* Function to multiply the user-defined matricies.
 */
static void matMult(int r1, int c1, int c2) {
   int i, j, k;
   int *mPtr, *aPtr, *bPtr;

   for (i = 0; i < r1; i++)
      for (j = 0; j < c2; j++)
         m[i][j] = 0;
   for (i = 0; i < r1; i++) {
      for (j = 0; j < c2; j++) {
         for (k = 0; k < c1; k++) {
            if (CACHESIM) {
               mPtr = &m[i][j];
               aPtr = &a[i][k];
               bPtr = &b[k][j];
               memRead(mPtr);
               memRead(aPtr);
               memRead(bPtr);
               memWrite(mPtr);
            }
            m[i][j] += a[i][k] * b[k][j];
         }
      }
   }
}

/* Function to print the cross-product matrix, if VERBOSE is set to 1.
 */
static void printMatrix(int r1, int c1, int r2, int c2) {
   int i, j;
   printf("\nOutput Matrix =========================================\n\n");
   for (i = 0; i < r1; i++) {
      printf("   ");
      for (j = 0; j < c2; j++) {
         printf("%04d ", m[i][j]);
         if (j == (c2 - 1))
            printf("\n\n");
      }
   }
}

/* Function to print the results of the cache simulation, if CACHESIM is set
 * to 1.
 */
static void printResults(void) {
   printf("Simulation Results ====================================\n");
   printf("\n"                                                       );
   printf("    Cache Size: %03d\n", CACHE_SIZE                       );
   printf("    Assoc Size: %03d\n", ASSOC_SIZE                       );
   printf("   Replacement: LRU\n"                                    );
   printf("\n"                                                       );
   printf("         Reads: %04d\n", reads                            );
   printf("        Writes: %04d\n", writes                           );
   printf("         Total: %04d\n", reads + writes                   );
   printf("\n"                                                       );
   printf("          Hits: %04d\n", hits                             );
   printf("        Misses: %04d\n", misses                           );
   printf("         Total: %04d\n", hits + misses                    );
   printf("\n"                                                       );
   printf("      Hit Rate: %.02f\n", (float)(hits * 100) / (hits + misses));
   printf("\n"                                                       );
   printf("=======================================================\n");
   printf("\n");
}

/* MAIN
 */
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
         if (MANUAL) {
            printf("Enter elements a[%d][%d]: ", i + 1, j + 1);
            scanf("%d", &a[i][j]);
         }
         else {
            a[i][j] = i + j;
         }
      }
   }
   for (i = 0; i < r2; i++) {
      for (j = 0; j < c2; j++) {
         if (MANUAL) {
            printf("Enter elements b[%d][%d]: ", i + 1, j + 1);
            scanf("%d", &b[i][j]);
         }
         else {
            b[i][j] = 10 + i + j;
         }
      }
   }
   matMult(r1, c1, c2);
   if (VERBOSE)
      printMatrix(r1, c1, r2, c2);
   if (CACHESIM)
      printResults();
   if (CACHEPRINT)
      printCacheSet();
   exit(EXIT_SUCCESS);
}