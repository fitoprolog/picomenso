/*Copyright <YEAR> <COPYRIGHT HOLDER>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/
#include<stdlib.h>
#include<time.h>
#include<stdio.h>
#include <picomenso.h>
#include <math.h>
#include <string.h>
#include <justamodel.h>
void test_save()
{

  printf("testing serialization");
  float done[] = {1,2,3,4};
  float dtwo[] = {5,6,7};
  float dthree[] = {8,9,10,11};
  struct ParametersBlock one={0},two={0},three={0};
  
  BLOCK_SET_DATA(one,done);
  BLOCK_SET_DATA(two,dtwo);
  BLOCK_SET_DATA(three,dthree);

  one.next = &two; 
  two.next = &three;
  block_save(&one , "model.bin");
  block_print(&one);
}
void test_load()
{
  struct ParametersBlock recover = {0};
  block_load(&recover,"model.bin");
  printf("*********The moment of true from disk*************\n");
  block_print(&recover);
}

void test_one_adder(struct ParametersBlock *block, int n)
{
  for(int i=0; i != block->nElements;i++)
    *((float*)block->data +i)+=777;
}

void test_ground_function( float *input, float *output)  
{

  *output = 1/(1+expf(-(input[0]*0.1337+input[1]*0.08344)));
  float so = 1/(1+expf(-(-0.2316763*input[0]+input[1]*0.06669)));
  *output += so;
}

void test_learn_function(struct ParametersBlock *model, float *input, float *output)  
{
   *output = 1/(1+expf(-(model->data[0]*input[0]+model->data[1]*input[1])));
   float so = 1/(1+expf(-(model->next->data[1]*input[0]+input[1]*model->next->data[0])));
   *output+=so;
}

void test_read_array()
{

  struct ParametersBlock recover = {0};
  printf("************decoding******************************\n");
  block_from_array(&recover, model_bin,sizeof(model_bin)/sizeof(u_int32_t));
  printf("*********The moment of true from array*************\n");
  block_print(&recover);
  block_iterator(&recover,test_one_adder);
  block_print(&recover);
  block_randomize_init(&recover);
  block_print(&recover);
  float inputs[3*1000];
  float grounds[1000];
  float outputs=0;
  float firstLayerData[] = {3,0};
  float secondLayerData[] = {-11,0};
  struct ParametersBlock one={0},two={0};
  BLOCK_SET_DATA(one,firstLayerData);
  BLOCK_SET_DATA(two,secondLayerData);
  one.next =&two;
  printf("*************starting to test evaluator  *********************\n");
  block_randomize_init(&one);
  block_print(&one);
  printf("*************generating samples ******************************\n");
  for(int i=0; i!= 1000; i++)
  {
    inputs[i*3  ]=random()%0xff;
    inputs[i*3+1]=random()%0xff;
    inputs[i*3+2]=random()%0xff;
    test_ground_function(inputs+ i*3,grounds+i);
    printf("%f\n",*(grounds+i));
  }
  //return;
  //picomenso_optimizer(&one,test_learn_function,inputs,grounds,3,1,1000,100,100,5e-10);
  block_print(&one);
  struct ParametersBlock cloneTest; 
  block_clone(&one,&cloneTest,true);
  printf("Is clone working\n");
  block_print(&cloneTest);
  printf("Second cloning without allocation 10000 times\n");
  for (int i=0; i != 1000;i++)
    block_clone(&one,&cloneTest,false);
  block_print(&cloneTest);
  printf("*************************optimizing****************************\n");
  picomenso_optimizer(&one,test_learn_function,inputs,grounds,3,1,1000,60000,100,5e-10);
  block_print(&one);
}

int main(int argc, char **argv)
{
  srand(time(0));
  test_save();
  test_load();
  test_read_array();
}
