/*Copyright 2021 fito.prolog@gmail.com Fitus Maticus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/
#include<stdlib.h>
#include<time.h>
#include<stdio.h>
#include <picomenso.h>
#include <math.h>
#include <string.h>

char block_save(struct ParametersBlock *block,const char *fileName)
{
  struct ParametersBlock *cblock = block;
  FILE *fd = fopen(fileName, "wb");
  if (!fd) 
  {
    printf("Error ocurred trying to open file descriptor\n");
    return 0;
  }
  while(cblock)
  {
    fwrite(&(cblock->nElements), sizeof(u_int32_t),1,fd);
    fwrite(cblock->data,sizeof(float),cblock->nElements,fd);
    cblock = cblock->next;
  }
  fclose(fd);
  return 1;
}

char block_load(struct ParametersBlock *block, const char *fileName)
{
  struct ParametersBlock *cblock = block;
  FILE *fd = fopen(fileName, "rb");
  char fetchingData = 0;

  if (!fd) 
  {
    printf("Error ocurred trying to open file descriptor\n");
    return 0;
  }

  void *tmpBuffer =  (void *)malloc(sizeof(u_int32_t));
  size_t expectedBytes = 1, got=0;
  int nblock = 0;
  got = fread(tmpBuffer,sizeof(u_int32_t),1,fd);
  while(got)
  {
    if (fetchingData && !got)
    {
      printf("Corrupted incomplete file got=%d expected=%d fetchingData=%d\n",got,expectedBytes,fetchingData);
      fclose(fd);
      return 0;
    }

    if (!fetchingData)
    {
      cblock->nElements = *(u_int32_t*)tmpBuffer;
      if (!cblock->nElements) break;
      tmpBuffer = (float*) malloc ( sizeof(float) * cblock->nElements);
      expectedBytes =  cblock->nElements;
      fetchingData=1;
    }
    else 
    {
      cblock->data = (float*)tmpBuffer;
      tmpBuffer  = malloc ( sizeof(u_int32_t));
      cblock->next  = (struct ParametersBlock *) malloc (sizeof (struct ParametersBlock)); 
      cblock = cblock->next;
      memset(cblock, 0,sizeof(struct ParametersBlock));
      fetchingData =0;
      expectedBytes = 1;
    }
    got = fread(tmpBuffer,sizeof(u_int32_t),expectedBytes,fd);
  }

  fclose(fd);
  return 1;
}

char block_from_array(struct ParametersBlock *block,unsigned char *bytes,size_t dataSize)
{
  u_int32_t expectedElements = *(u_int32_t*)bytes;
  u_int32_t offset =1;
  struct ParametersBlock *cblock = block;

  if (dataSize < 2 ) return 0;
  
  while(1)
  {
    if ( (offset+expectedElements) > dataSize )
    {
      printf("corrupted model from memory\n");
      return 0;
    }
    cblock->data = ((float*)bytes+offset);
    cblock->next  = (struct ParametersBlock *) malloc (sizeof (struct ParametersBlock)); 
    cblock->nElements=expectedElements;
    cblock = cblock->next;
    offset+=expectedElements ;
    memset(cblock, 0,sizeof(struct ParametersBlock));
    if ( (offset+1) > dataSize ) return 1;
    expectedElements = *((u_int32_t*)bytes+offset);
    offset++;
  }
  return 1;
}

void block_iterator(struct ParametersBlock *block, void (*callback)(struct ParametersBlock *))
{
  struct ParametersBlock *cblock = block;
  while(cblock)
  {
    callback(cblock);
    cblock = cblock->next;
  }
}

void block_print(struct ParametersBlock *block)
{
  struct ParametersBlock *cblock = block;
  int nblock =0;
  while(cblock)
  {
    printf("block=%d\n",nblock++);
    for(int i=0; i != cblock->nElements;i++)
      printf("%f ",*((float*)cblock->data+i));
    printf("\n============================================\n");
    cblock = cblock->next;
  }
}

void block_randomize(struct ParametersBlock *block)
{
  for(int i=0; i != block->nElements;i++)
  {
    *((float*)block->data +i)=-0xffff+random() % 2*0xffff;
    if (*((float*)block->data +i) == 0)
      *((float*)block->data +i)=1;
  }
}
void block_randomize_init(struct ParametersBlock *block)
{
  block_iterator(block, block_randomize);
}

int block_count_parameters(struct ParametersBlock *block)
{
  int ret=0;
  struct ParametersBlock *cblock = block;
  while(cblock)
  {
    ret+=cblock->nElements;
    cblock = cblock->next;
  }
  return ret;
}
