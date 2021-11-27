#include <sstream> 
#include <iostream>
#include <vector> 
#include <opencv2/opencv.hpp>
#include <time.h>
#include <sys/time.h>
#include <random>
#include <time.h>

#define SUPER_KERNEL_SIZE 40
#define WIDTH 160
#define HEIGHT 120

int main(void)
{
  struct timeval lastTime;
  gettimeofday(&lastTime, NULL);
  cv::VideoCapture videoSource("rtsp://admin:pikachu123@192.168.1.110:554/cam/realmonitor?channel=8&subtype=1");
  cv::Mat image ;
  srand(time(0));
  cv::Mat superKernel(SUPER_KERNEL_SIZE,SUPER_KERNEL_SIZE,CV_32FC3);
  float *data = (float *)superKernel.data;
  
  for(int i=0; i!= SUPER_KERNEL_SIZE*SUPER_KERNEL_SIZE*3;i++)
    *(data+i)=(float)(random()%0xffff)/0xffff;

  while(true)
  {
    for(int i=0; i!= SUPER_KERNEL_SIZE*SUPER_KERNEL_SIZE*3;i++)
      *(data+i)=(float)(random()%0xffff)/0xffff;
    videoSource >> image;
    cv::resize(image,image,cv::Size(WIDTH,HEIGHT));
    cv::Mat normalized;
    image.convertTo(normalized,CV_32FC3);
    normalized/=255;
    float *normdata = (float *)normalized.data;

    for(int i=0;i!=3; i++)
    {
      for(int j=0;j!=4;j++)
      {
        for(int k=0; k!= SUPER_KERNEL_SIZE*SUPER_KERNEL_SIZE*3;k++)
          *(normdata + i*SUPER_KERNEL_SIZE*3*WIDTH + k%(SUPER_KERNEL_SIZE*3) +
              k/(SUPER_KERNEL_SIZE*3)*WIDTH*3 +j*SUPER_KERNEL_SIZE*3 )*= *(data+k); 
      }
    }
    cv::imshow("result",normalized);
    cv::waitKey(1);

  }
}
