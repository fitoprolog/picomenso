/*Copyright 2021 fito.prolog@gmail.com Fitus Maticus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/
#ifndef __PICOMENSO_H_ 
#define __PICOMENSO_H_ 1 
#define BLOCK_SET_DATA(B,D)  B.nElements = sizeof(D)/sizeof(int32_t);B.data = D;                             
struct ParametersBlock
{
  u_int32_t nElements;   
  float *data; 
  struct ParametersBlock *next;
};

char block_save(struct ParametersBlock *block,const char *fileName);
char block_load(struct ParametersBlock *block, const char *fileName);
char block_from_array(struct ParametersBlock *block,unsigned char *bytes,size_t dataSize);
void block_print(struct ParametersBlock *block);
void block_randomize(struct ParametersBlock *block);
void block_iterator(struct ParametersBlock *block, void (*callback)(struct ParametersBlock *));
void picomenso_optimizer(struct ParametersBlock *model, 
                         void (*forwardFunction)(struct ParametersBlock *,float *,float *),
                         float *inputs,
                         float *grounds,
                         int inputSize,
                         int groundsSize,
                         int batchSize,
                         int epochs,
                         int mutation_space,
                         float minimal_expected_loss);
int block_count_parameters(struct ParametersBlock *block);
#endif 
