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
#include <pthread.h>

void picomenso_mutate(struct ParametersBlock *model)
{
  int step = random()%0xff;
  if (!step)
    step = 2;
  struct ParametersBlock *cblock = model;
  while(cblock)
  {
    for(int i=0; i!= cblock->nElements; i++)
    {
      if (random()%2)
      {
        char case_ = random()%2;
        if (case_ == 0)
          cblock->data[i] += (random()%2 ? 1 :-1)* ((random() % (step)) + ((float)(random()%0xfffff))/0xfffff);
        else
          cblock->data[i] = (random()%2 ? 1:-1)*((float)(random()%0xfffff))/0xfffff;
      }
    }
    cblock = cblock->next;
  }
}
void picomenso_thread_wrapper(void (*forwardFunction)(struct ParametersBlock *,float *,float *),
                              struct ParametersBlock *model,float *inputs, float *predicted ,char * stopSignal)
{
 
  forwardFunction(model,inputs,predicted);
  *stopSignal=1;
}

void picomenso_optimizer(struct ParametersBlock *model, 
                         void (*forwardFunction)(struct ParametersBlock *,float *,float *),
                         float *inputs,
                         float *grounds,
                         int inputSize,
                         int groundsSize,
                         int batchSize,
                         int epochs,
                         int mutation_space,
                         float minimal_expected_loss)
{
  //pthread_t *thread_pool  = (pthread_t*)malloc(sizeof(pthread_t)*mutation_space);
  float predicted[groundsSize*mutation_space];
  float global_error=0xffff,local_error,mutated_error=0;
  int parameterCount = block_count_parameters(model);
  struct ParametersBlock *mutations = (struct ParametersBlock *) 
                                      malloc(sizeof(struct ParametersBlock) * (mutation_space));

  for(int m=0; m!= mutation_space; m++)
    block_clone(model, mutations+m,true);

  int bestMutation =-1;

  global_error=0;
  for(int i=0; i != batchSize; i++)
  {
    forwardFunction(model, (inputs+i*inputSize),predicted);
    local_error=0;

    for (int o=0; o != groundsSize; o++)
    {
      float tmp= (float)*(grounds+o+i*groundsSize)- (float)*(predicted+o);
      local_error+=tmp*tmp;
    }
    global_error+=local_error;
  }
  global_error /= batchSize;



  for (int e=0 ; e != epochs; e++)
  {
    printf("MSE %.10e at %d\n",global_error,e);
    if (global_error  <= minimal_expected_loss ) return;

    for(int m=0; m!= mutation_space; m++)
      picomenso_mutate(mutations+m);
    bestMutation=-1;

    for(int m=0; m != mutation_space; m++)
    {
      mutated_error=0;
      for(int i=0; i != batchSize; i++)
      { 
        forwardFunction(mutations+m, (inputs+i*inputSize),predicted);
        local_error=0;
        //current value
        for (int o=0; o != groundsSize; o++)
        {
          float tmp= (float)*(grounds+o+i*groundsSize)- (float)*(predicted+o);
          local_error+=tmp*tmp;
        }
        mutated_error+=local_error;
      }
      mutated_error/=batchSize;

      //if this is the gradient descent mutation, update parameters based on gradients
      if (mutated_error < global_error) {
        bestMutation=m;
        global_error = mutated_error;
      }
    }
    if (bestMutation > -1) 
      block_clone(mutations+bestMutation , model,false);
  }
}
