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

void picomenso_mutate(float *mutations,int totalSpace, float error)
{
  int derror  = error;
  int step = random()%0xff;
  if (!step)
    step = 2;
  float tmp;
  int randp = 0;
  for(int i=0; i!= totalSpace; i++)
  {
    if (random()%2)
    {
      char case_ = random()%2;
      if (case_ == 0)
        *(mutations+i) += (random()%2 ? 1 :-1)* ((random() % (step)) + ((float)(random()%0xffff))/0xffff);
      else
        *(mutations+i) = (random()%2 ? 1:-1)*((float)(random()%0xffff))/0xffff;
    }

  }
  printf("\n");
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
  float quadratic_error=0;
  float predicted[groundsSize];
  float global_error=0xffff,local_error,mutated_error=0;
  int parameterCount = block_count_parameters(model);
  float **allParameters = (float **)malloc( sizeof(float *) * parameterCount);
  float *backup    = (float *) malloc(sizeof(float) * parameterCount);
  float *mutations = (float *) malloc(sizeof(float) * parameterCount * (mutation_space+1));
  struct ParametersBlock *block=model;
  int nparameter = 0;
  int bestMutation =-1;

  while(block)
  {
    for(int p=0; p!= block->nElements; p++)
      allParameters[nparameter++]=block->data+p;
    block = block->next;
  }

  global_error=0;
  for(int i=0; i != batchSize; i++)
  {
    test_learn_function(model, (inputs+i*inputSize),predicted);
    local_error=0;
    //current value
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
    
    picomenso_mutate(mutations,parameterCount * mutation_space, global_error);
    bestMutation=-1;

    for(int i=0; i!= parameterCount; i++){
      *(backup+i)=*(*(allParameters+i));
    }

    for(int m=0; m != mutation_space+1; m++)
    {
      mutated_error=0;
      for(int p=0; p != parameterCount; p++)
        *(*(allParameters+p)) = *(mutations+p +parameterCount*m);

      for(int i=0; i != batchSize; i++)
      {
        test_learn_function(model, (inputs+i*inputSize),predicted);
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
        if (bestMutation == mutation_space+1)
          printf("yuju\n");
        global_error = mutated_error;
      }
    }
    if (bestMutation > -1) 
    {
      for(int i=0; i!= parameterCount; i++)
         *(*(allParameters+i)) = *(mutations+i+parameterCount*bestMutation);
    }
    else 
      for(int i=0; i!= parameterCount; i++)
        *(*(allParameters+i)) = *(backup+i);

    for(int i=0; i != mutation_space+1; i++)
        for (int p=0; p!= parameterCount; p++)
          if (bestMutation!= i || bestMutation == -1)
            *(mutations+p+parameterCount*i)= *(*(allParameters+p));
  }
}
