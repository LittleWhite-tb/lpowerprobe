#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

// This test will do some cpu intensive calculation, put results in an array
// and copy this array in another one in order to have a memory intensive section
#define ARRAY_SIZE 1000000 // Should not be below 1000 (because devided by 8)
#define REPET 1000

void cpuCalculation(float* pCpuArray)
{
   assert(pCpuArray);
   
   unsigned int i =0;
   for ( i = 0 ; i < ARRAY_SIZE ; i++ )
   {
      pCpuArray[i] = pCpuArray[i] * 43 + 90 * (i-3);
      pCpuArray[i] = cos(i* 0.1);
   }
}

void memCalculation(float* pCpuArray, float* pMemArray)
{
   unsigned int i =0;
   for ( i = 0 ; i < ARRAY_SIZE ; i+=8 )
   {
      pMemArray[i] = pCpuArray[i];
      pMemArray[i+1] = pCpuArray[i+1];
      pMemArray[i+2] = pCpuArray[i+2];
      pMemArray[i+3] = pCpuArray[i+3];
      pMemArray[i+4] = pCpuArray[i+4];
      pMemArray[i+5] = pCpuArray[i+5];
      pMemArray[i+6] = pCpuArray[i+6];
      pMemArray[i+7] = pCpuArray[i+7];
   }
}

int main()
{
   float* pCpuArray = calloc(ARRAY_SIZE,sizeof(float));
   float* pMemArray = calloc(ARRAY_SIZE,sizeof(float));
   if ( !pCpuArray || !pMemArray )
   {
      fprintf(stderr,"Fail to alloc array for the test\n");
      free(pCpuArray);
      free(pMemArray);
      return -1;
   }
   
   unsigned int i = 0;
   for ( i = 0 ; i < REPET ; i++ )
   {
      cpuCalculation(pCpuArray);
      memCalculation(pCpuArray,pMemArray);
   }
   
   
   free(pCpuArray);
   free(pMemArray);
   
   return 0;
}
