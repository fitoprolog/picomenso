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
