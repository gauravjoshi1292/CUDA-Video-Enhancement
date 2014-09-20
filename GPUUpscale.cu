#include "opencv2/opencv.hpp"
#pragma comment (lib , "opencv_core244d.lib")
#pragma comment (lib ,"opencv_highgui244d.lib")
#pragma comment(lib , "opencv_imgproc244d.lib")
#include <cv.h>
#include <highgui.h>
#include"CImg.h"
#include"SOIL.h"
#include<conio.h>
#include<stdio.h>
#include<iostream>
#include<math.h>
#include<time.h>
#include<cuda.h>
#include<GL/glut.h>
#include<GL/GLU.h>
#include<GL/GL.h>
#include<Windows.h>

using namespace cimg_library;
using namespace cv;
using namespace std;

CImg<unsigned char> imageup(1024,1024,1,3);

int FLAG=0;
char *str3=new char[50];


//function for capturing frames either continuously or by skipping them by a number n
IplImage* skipNFrames(CvCapture* capture, int n)
{
    for(int i = 0; i < n; ++i)
    {
        if(cvQueryFrame(capture) == NULL)
        {
            return NULL;
        }
    }

    return cvQueryFrame(capture);
}

//function for calculating I11 on a GPU device
__device__ int I11(int i,int j,int *arr,int width)
{
	return (arr[(i - 2)+ (j - 2)*width] + arr[(i + 2) + (j + 2)*width] - 2*arr[i + (j)*width]);
}

//function for calculating I22 on a GPU device
__device__ int I22(int i,int j,int *arr,int width)
{
	return (arr[(i - 2)+ (j + 2)*width]+arr[(i + 2)+ (j - 2)*width]+arr[i + (j)*width]);
}

//function for finding absolute value, executed onn a GPU
__device__ int mod(int i)
{
	if (i<0)
		return ((-1)*i);
	else
		return i;
}

__constant__ int T; //the thresshold value
__constant__ char del; //the delta value

//function for calculating energy executed on a GPU device, calculates energy for a pixel value
__device__ float energycalc(int i,int j,int *arr,int w1,int w2,int w3,int w4,int width)
{ 
	float u,uc,ue;
 	uc=w1*(mod((I11(i,j,arr,width) - I11(i+1,j+1,arr,width))) +
                     mod(I22(i,j,arr,width) - I22(i+1,j+1,arr,width))) +
				w2*(mod(I11(i,j,arr,width) - I11(i+1,j-1,arr,width)) +
					mod(I22(i,j,arr,width) - I22(i+1,j-1,arr,width)))+
				w3*(mod(I11(i,j,arr,width) - I11(i-1,j+1,arr,width)) +
					mod(I22(i,j,arr,width) - I22(i-1,j+1,arr,width))) +
				w4*(mod(I11(i,j,arr,width) - I11(i-1,j-1,arr,width)) +
					mod(I22(i,j,arr,width) - I22(i-1,j-1,arr,width)));
			  ue=-1*(mod(I11(i,j,arr,width))+mod(I22(i,j,arr,width)));
			  u=5*uc-2*ue;
return u;

}

//function for finding pixel values for (2i+1, 2j+1), a global function
__global__ void FastInterpolation1(int *red,int *green,int *blue,int width, int height)
{
    int i = (blockIdx.x * blockDim.x) + threadIdx.x+1;
	int j = (blockIdx.y * blockDim.y) + threadIdx.y+1;
	float secd1,secd2,secd3,secd4,secd5,secd6;
	
			secd1= red[(2*i-2)+width*(2*j + 2)] + red[(2*i)+width* (2*j)]+red[(2*i + 2)+width* (2*j - 2)] - 3*red[(2*i)+width* (2*j + 2)]-3*red[(2*i + 2)+width* 2*j] +red[(2*i)+width*( 2*j + 4)] + red[(2*i + 2)+width*( 2*j + 2)] + red[(2*i + 4)+width* 2*j];
			secd2=red[(2*i)+width*( 2*j-2)] + red[(2*i + 2)+width* 2*j]+red[(2*i + 4)+width*( 2*j + 2)] - 3*red[(2*i)+width* (2*j)] -3*red[(2*i + 2)+width* (2*j + 2)]+red[(2*i- 2)+width* (2*j)] + red[(2*i)+width* (2*j + 2)] + red[(2*i + 2)+width* (2*j + 4)];
			if(secd1<=secd2)
				red[(2*i+1)+width*(2*j+1)]=(red[(2*i)+width*(2*j)]+red[(2*i+2)+width*(2*j+2)])/2;
			else
				red[(2*i+1)+width*(2*j+1)]=(red[(2*i+2)+width*2*j]+red[2*i+width*(2*j+2)])/2;
			
			secd3= green[(2*i-2)+width*(2*j + 2)] + green[(2*i)+width* (2*j)]+green[(2*i + 2)+width* (2*j - 2)] - 3*green[(2*i)+width* (2*j + 2)]-3*green[(2*i + 2)+width* 2*j] +green[(2*i)+width*(2*j + 4)] + green[(2*i + 2)+width* (2*j + 2)] + green[(2*i + 4)+width* 2*j];
			secd4=green[(2*i)+width* (2*j-2)] + green [(2*i + 2)+width* (2*j)]+green [(2*i + 4)+width* (2*j + 2)] - 3*green [(2*i)+width* (2*j)] -3*green [(2*i + 2)+width* (2*j + 2)]+green [(2*i- 2)+width* (2*j)] + green [(2*i)+width* (2*j + 2)] + green[(2*i + 2)+width* (2*j + 4)];
			if(secd3<=secd4)
				green[(2*i+1)+width*(2*j+1)]=(green[(2*i)+width*(2*j)]+green[(2*i+2)+width*(2*j+2)])/2;
			else
				green[(2*i+1)+width*(2*j+1)]=(green[(2*i+2)+width*2*j]+green[2*i+width*(2*j+2)])/2;
			
			secd5= blue[(2*i-2)+width*(2*j + 2)] + blue[(2*i)+width* (2*j)]+blue[(2*i + 2)+width* (2*j - 2)] - 3*blue[(2*i)+width* (2*j + 2)]-3*blue[(2*i + 2)+width* (2*j)] +blue[(2*i)+ width*(2*j + 4)] + blue[(2*i + 2)+width* (2*j + 2)] + blue[(2*i + 4)+width* 2*j];
			secd6=blue[(2*i)+width* (2*j-2)] + blue[(2*i + 2)+width* 2*j]+blue[(2*i + 4)+width* (2*j + 2)] - 3*blue[(2*i)+width* (2*j)] -3*blue[(2*i + 2)+width* (2*j + 2)]+blue[(2*i- 2)+width* 2*j] + blue[(2*i)+width* (2*j + 2)] + blue[(2*i + 2)+width* (2*j + 4)];
			if(secd5<=secd6)
				blue[(2*i+1)+width*(2*j+1)]=(blue [(2*i)+width*(2*j)]+blue [(2*i+2)+width*(2*j+2)])/2;
			else
				blue[(2*i+1)+width*(2*j+1)]=(blue [(2*i+2)+width*(2*j)]+blue [(2*i)+width*(2*j+2)])/2;
}

//function for finding pixel values (2i,2j+1)
__global__ void FastInterpolation2(int *red,int *green,int *blue,int width, int height)
{
    int i = (blockIdx.x * blockDim.x) + threadIdx.x+1;
	int j = (blockIdx.y * blockDim.y) + threadIdx.y+1;
	float secd1,secd2;
	
			secd1= red[(2*i-3)+width*(2*j + 2)] + red[(2*i-1)+width* (2*j)]+red[(2*i + 1)+width* (2*j - 2)] - 3*red[(2*i-1)+width* (2*j + 2)]-3*red[(2*i +1)+width* 2*j] +red[(2*i-1)+width*( 2*j + 4)] + red[(2*i + 1)+width*( 2*j + 2)] + red[(2*i + 3)+width* 2*j];
			secd2=red[(2*i-1)+width*( 2*j-2)] + red[(2*i + 1)+width* 2*j]+red[(2*i + 3)+width*( 2*j + 2)] - 3*red[(2*i-1)+width* (2*j)] -3*red[(2*i + 1)+width* (2*j + 2)]+red[(2*i- 3)+width* (2*j)] + red[(2*i-1)+width* (2*j + 2)] + red[(2*i + 1)+width* (2*j + 4)];
			if(secd1<=secd2)
				red[(2*i)+width*(2*j+1)]=(red[(2*i)+width*(2*j)]+red[(2*i+2)+width*(2*j+2)])/2;
			else
				red[(2*i)+width*(2*j+1)]=(red[(2*i+2)+width*2*j]+red[2*i+width*(2*j+2)])/2;
			
			secd1= green[(2*i-3)+width*(2*j + 2)] + green[(2*i-1)+width* (2*j)]+green[(2*i + 1)+width* (2*j - 2)] - 3*green[(2*i-1)+width* (2*j + 2)]-3*green[(2*i + 1)+width* 2*j] +green[(2*i-1)+width*(2*j + 4)] + green[(2*i + 1)+width* (2*j + 2)] + green[(2*i + 2)+width* 2*j];
			secd2=green[(2*i-1)+width* (2*j-2)] + green [(2*i + 1)+width* (2*j)]+green [(2*i + 2)+width* (2*j + 2)] - 3*green [(2*i-1)+width* (2*j)] -3*green [(2*i + 1)+width* (2*j + 1)]+green [(2*i- 3)+width* (2*j)] + green [(2*i-1)+width* (2*j + 2)] + green[(2*i + 1)+width* (2*j + 4)];
			if(secd1<=secd2)
				green[2*i+width*(2*j+1)]=(green[2*i+width*2*j]+green[2*i+2+width*(2*j+2)])/2;
			else
				green[2*i+width*(2*j+1)]=(green[2*i+2+width*2*j]+green[2*i+width*(2*j+2)])/2;
			
			secd1= blue[2*i-3+width*(2*j + 2)] + blue[2*i-1+width* 2*j]+blue[2*i-1 +width* (2*j - 2)] - 3*blue[2*i-1+width* (2*j + 2)]-3*blue[2*i+1 +width* 2*j] +blue[2*i-1+ (2*j + 4)*width] + blue[2*i+1 + 2+width* (2*j + 2)] + blue[2*i + 3+width* 2*j];
			secd2=blue[2*i-1+width* (2*j-2)] + blue[2*i + 1+width* 2*j]+blue[2*i + 3+width* (2*j + 1)] - 3*blue[2*i+width* 2*j] -3*blue[2*i-1 + 2+width* (2*j + 2)]+blue[2*i- 3+width* 2*j] + blue[2*i-1+width* (2*j + 2)] + blue[2*i+1 + 2+width* (2*j + 4)];
			if(secd1<=secd2)
				blue[2*i+width*(2*j+1)]=(blue [2*i+width*2*j]+blue [2*i+2+width*(2*j+2)])/2;
			else
				blue[2*i+width*(2*j+1)]=(blue [2*i+2+width*2*j]+blue [2*i+width*(2*j+2)])/2;
}

//function for finding pixel values (2i+1,2j)
__global__ void FastInterpolation3(int *red,int *green,int *blue,int width, int height)
{
    int i = (blockIdx.x * blockDim.x) + threadIdx.x+1;
	int j = (blockIdx.y * blockDim.y) + threadIdx.y+1;
	float secd1,secd2;
	
			secd1= red[(2*i-2)+width*(2*j + 1)] + red[(2*i)+width* (2*j-1)]+red[(2*i + 2)+width* (2*j - 3)] - 3*red[(2*i)+width* (2*j + 1)]-3*red[(2*i+2)+width* (2*j-1)] +red[(2*i)+width*( 2*j + 3)] + red[(2*i + 2)+width*( 2*j + 1)] + red[(2*i + 4)+width* (2*j-1)];
			secd2=red[(2*i)+width*( 2*j-3)] + red[(2*i + 2)+width* (2*j-1)]+red[(2*i + 4)+width*( 2*j + 1)] - 3*red[(2*i)+width* (2*j-1)] -3*red[(2*i+2)+width* (2*j +1)]+red[(2*i- 2)+width* (2*j-1)] + red[(2*i)+width* (2*j + 1)] + red[(2*i + 2)+width* (2*j + 3)];
			if(secd1<=secd2)
				red[(2*i+1)+width*(2*j)]=(red[(2*i+1)+width*(2*j-1)]+red[(2*i+3)+width*(2*j+1)])/2;
			else
				red[(2*i+1)+width*(2*j)]=(red[(2*i+3)+width*(2*j-1)]+red[(2*i+1)+width*(2*j+1)])/2;
			
			secd1= blue[(2*i-2)+width*(2*j + 1)] + blue[(2*i)+width* (2*j-1)]+blue[(2*i + 2)+width* (2*j - 3)] - 3*blue[(2*i)+width* (2*j + 1)]-3*blue[(2*i+2)+width* (2*j-1)] +blue[(2*i)+width*( 2*j + 3)] + blue[(2*i + 2)+width*( 2*j + 1)] + blue[(2*i + 4)+width* (2*j-1)];
			secd2=blue[(2*i)+width*( 2*j-3)] + blue[(2*i + 2)+width* (2*j-1)]+blue[(2*i + 4)+width*( 2*j + 1)] - 3*blue[(2*i)+width* (2*j-1)] -3*blue[(2*i+2)+width* (2*j +1)]+blue[(2*i- 2)+width* (2*j-1)] + blue[(2*i)+width* (2*j + 1)] + blue[(2*i + 2)+width* (2*j + 3)];
			if(secd1<=secd2)
				blue[(2*i+1)+width*(2*j)]=(blue[(2*i+1)+width*(2*j-1)]+blue[(2*i+3)+width*(2*j+1)])/2;
			else
				blue[(2*i+1)+width*(2*j)]=(blue[(2*i+3)+width*(2*j-1)]+blue[(2*i+1)+width*(2*j+1)])/2;
			
			secd1= green[(2*i-2)+width*(2*j + 1)] + green[(2*i)+width* (2*j-1)]+green[(2*i + 2)+width* (2*j - 3)] - 3*green[(2*i)+width* (2*j + 1)]-3*green[(2*i+2)+width* (2*j-1)] +green[(2*i)+width*( 2*j + 3)] + green[(2*i + 2)+width*( 2*j + 1)] + green[(2*i + 4)+width* (2*j-1)];
			secd2=green[(2*i)+width*( 2*j-3)] + green[(2*i + 2)+width* (2*j-1)]+green[(2*i + 4)+width*( 2*j + 1)] - 3*green[(2*i)+width* (2*j-1)] -3*green[(2*i+2)+width* (2*j +1)]+green[(2*i- 2)+width* (2*j-1)] + green[(2*i)+width* (2*j + 1)] + green[(2*i + 2)+width* (2*j + 3)];
			if(secd1<=secd2)
				green[(2*i+1)+width*(2*j)]=(green[(2*i+1)+width*(2*j-1)]+green[(2*i+3)+width*(2*j+1)])/2;
			else
				green[(2*i+1)+width*(2*j)]=(green[(2*i+3)+width*(2*j-1)]+green[(2*i+1)+width*(2*j+1)])/2;
}

//function for applying iterative correction to (2i+1,2j+1)
__global__ void Iterations1(int *red,int *green,int *blue,int width, int height)
{
	int i = (blockIdx.x * blockDim.x) + threadIdx.x+1;
	int j = (blockIdx.y * blockDim.y) + threadIdx.y+1;
	int save;
	float i1,i2,i3,i4,min,uplus,uminus;
	int flag,w1=0,w2=0,w3=0,w4=0;
	for(int l=1;l<1;l++)
	{
	  		  i1= -0.5*(red[(2*i+2)+(2*j+2)*width]-red[(2*i+1)+(2*j+1)*width]);
			  i2= -0.5*(red[(2*i+2)+(2*j)*width]-red[(2*i+1)+(2*j+1)*width]);
			  i3= -0.5*(red[(2*i)+(2*j+2)*width]-red[(2*i+1)+(2*j+1)*width]);
			  i4= -0.5*(red[(2*i)+(2*j)*width]-red[(2*i+1)+(2*j+1)*width]);
			  if (i1<T)
				  w1=1;
			  else if (i1>=T)
				  w1=0;
			  if (i2<T)
				  w2=1;
			  else if (i2>=T)
				  w2=0;
			  if (i3<T)
				  w3=1;
			  else if (i3>=T)
				  w3=0;
			  if (i4<T)
				  w4=1;
			  else if (i4>=T)
				  w4=0;
			 min=energycalc(2*i+1,2*j+1,red,w1,w2,w3,w4,width);flag=0;
			 syncthreads();
			 save=red[(2*i+1)+(2*j+1)*width];
			 red[(2*i+1)+(2*j+1)*width]=save+del;
			 syncthreads();
			 uplus=energycalc(2*i+1,2*j+1,red,w1,w2,w3,w4,width);
			 if(uplus<min)
			 {min=uplus;flag=1;}
			 red [(2*i+1)+(2*j+1)*width]=save-del;
			 syncthreads();
			 uminus=energycalc(2*i+1,2*j+1,red,w1,w2,w3,w4,width);
			 if(uminus<min)
			 {min=uminus;flag=2;}

			 switch(flag)
			 {
			 case 0:
				 {red[(2*i+1)+(2*j+1)*width]=save;
				 break;}
			 case 1:
				 {red[(2*i+1)+(2*j+1)*width]=save+del;
				 break;}
			 case 2:
				 {red[(2*i+1)+(2*j+1)*width]=save-del;
				 break;}
			 }
			  i1= -0.5*(blue[(2*i+2)+(2*j+2)*width]-blue[(2*i+1)+(2*j+1)*width]);
			  i2= -0.5*(blue[(2*i+2)+(2*j)*width]-blue[(2*i+1)+(2*j+1)*width]);
			  i3= -0.5*(blue[(2*i)+(2*j+2)*width]-blue[(2*i+1)+(2*j+1)*width]);
			  i4= -0.5*(blue[(2*i)+(2*j)*width]-blue[(2*i+1)+(2*j+1)*width]);
			  if (i1<T)
				  w1=1;
			  else if (i1>=T)
				  w1=0;
			  if (i2<T)
				  w2=1;
			  else if (i2>=T)
				  w2=0;
			  if (i3<T)
				  w3=1;
			  else if (i3>=T)
				  w3=0;
			  if (i4<T)
				  w4=1;
			  else if (i4>=T)
				  w4=0;
			  
		     min=energycalc(2*i+1,2*j+1,blue,w1,w2,w3,w4,width);flag=0;
			 syncthreads();
			 save=blue[(2*i+1)+(2*j+1)*width];
			 blue[(2*i+1)+(2*j+1)*width]=save+del;
			 syncthreads();
			 uplus=energycalc(2*i+1,2*j+1,blue,w1,w2,w3,w4,width);
			 if(uplus<min)
			 {min=uplus;flag=1;}
			 blue[(2*i+1)+(2*j+1)*width]=save-del;
			 syncthreads();
			 uminus=energycalc(2*i+1,2*j+1,blue,w1,w2,w3,w4,width);
			 if(uminus<min)
			 {min=uminus;flag=2;}

			 switch(flag)
			 {
			 case 0:
				 {blue[(2*i+1)+(2*j+1)*width]=save;
				 break;}
			 case 1:
				 {blue[(2*i+1)+(2*j+1)*width]=save+del;
				 break;}
			 case 2:
				 {blue[(2*i+1)+(2*j+1)*width]=save-del;
				 break;}
			 }
			 
			  i1= -0.5*(green[(2*i+2)+(2*j+2)*width]-green[(2*i+1)+(2*j+1)*width]);
			  i2= -0.5*(green[(2*i+2)+(2*j)*width]-green[(2*i+1)+(2*j+1)*width]);
			  i3= -0.5*(green[(2*i)+(2*j+2)*width]-green[(2*i+1)+(2*j+1)*width]);
			  i4= -0.5*(green[(2*i)+(2*j)*width]-green[(2*i+1)+(2*j+1)*width]);
			  if (i1<T)
				  w1=1;
			  else if (i1>=T)
				  w1=0;
			  if (i2<T)
				  w2=1;
			  else if (i2>=T)
				  w2=0;
			  if (i3<T)
				  w3=1;
			  else if (i3>=T)
				  w3=0;
			  if (i4<T)
				  w4=1;
			  else if (i4>=T)
				  w4=0;
			  
		     min=energycalc(2*i+1,2*j+1,green,w1,w2,w3,w4,width);flag=0;
			 syncthreads();
			 save=green[(2*i+1)+(2*j+1)*width];
			 green[(2*i+1)+(2*j+1)*width]=save+del;
			 syncthreads();
			 uplus=energycalc(2*i+1,2*j+1,green,w1,w2,w3,w4,width);
			 if(uplus<min)
			 {min=uplus;flag=1;}
			 green[(2*i+1)+(2*j+1)*width]=save-del;
			 syncthreads();
			 uminus=energycalc(2*i+1,2*j+1,green,w1,w2,w3,w4,width);
			 if(uminus<min)
			 {min=uminus;flag=2;}

			 switch(flag)
			 {
			 case 0:
				 {green[(2*i+1)+(2*j+1)*width]=save;
				 break;}
			 case 1:
				 {green[(2*i+1)+(2*j+1)*width]=save+del;
				 break;}
			 case 2:
				 {green[(2*i+1)+(2*j+1)*width]=save-del;
				 break;}
			 }
		__syncthreads(); 
       }
}

//function for applying iterative correction to (2i,2j+1)
__global__ void Iterations2(int *red,int *green,int *blue,int width, int height)
{
	int i = (blockIdx.x * blockDim.x) + threadIdx.x+1;
	int j = (blockIdx.y * blockDim.y) + threadIdx.y+1;
	char save;
	float i1,i2,i3,i4,min,uplus,uminus;int flag,w1=0,w2=0,w3=0,w4=0;
	for(int l=1;l<1;l++)
	{
	  		  i1= -0.5*(red[2*i+1+(2*j+2)*width]-red[2*i+(2*j+1)*width]);
			  i2= -0.5*(red[2*i+1+(2*j)*width]-red[2*i+(2*j+1)*width]);
			  i3= -0.5*(red[2*i-1+(2*j+2)*width]-red[2*i+(2*j+1)*width]);
			  i4= -0.5*(red[2*i-1+(2*j)*width]-red[2*i+(2*j+1)*width]);
			  if (i1<T)
				  w1=1;
			  else if (i1>=T)
				  w1=0;
			  if (i2<T)
				  w2=1;
			  else if (i2>=T)
				  w2=0;
			  if (i3<T)
				  w3=1;
			  else if (i3>=T)
				  w3=0;
			  if (i4<T)
				  w4=1;
			  else if (i4>=T)
				  w4=0;
			  
		     min=energycalc(2*i,2*j+1,red,w1,w2,w3,w4,width);flag=0;
			 syncthreads();
			 save=red[2*i+(2*j+1)*width];
			 red[2*i+(2*j+1)*width]=save+del;
			 syncthreads();
			 uplus=energycalc(2*i+1,2*j+1,red,w1,w2,w3,w4,width);
			 if(uplus<min)
			 {min=uplus;flag=1;}
			red[2*i+(2*j+1)*width]=save-del;
			syncthreads(); 
			uminus=energycalc(2*i,2*j+1,red,w1,w2,w3,w4,width);
			 if(uminus<min)
			 {min=uminus;flag=2;}

			 switch(flag)
			 {
			 case 0:
				 {red[2*i+(2*j+1)*width]=save;
				 break;}
			 case 1:
				 {red[2*i+(2*j+1)*width]=save+del;
				 break;}
			 case 2:
				 {red[2*i+(2*j+1)*width]=save-del;
				 break;}
			 }
			  i1= -0.5*(blue[2*i+1+(2*j+2)*width]-blue[2*i+(2*j+1)*width]);
			  i2= -0.5*(blue[2*i+1+(2*j)*width]-blue[2*i+(2*j+1)*width]);
			  i3= -0.5*(blue[2*i-1+(2*j+2)*width]-blue[2*i+(2*j+1)*width]);
			  i4= -0.5*(blue[2*i-1+(2*j)*width]-blue[2*i+(2*j+1)*width]);
			  if (i1<T)
				  w1=1;
			  else if (i1>=T)
				  w1=0;
			  if (i2<T)
				  w2=1;
			  else if (i2>=T)
				  w2=0;
			  if (i3<T)
				  w3=1;
			  else if (i3>=T)
				  w3=0;
			  if (i4<T)
				  w4=1;
			  else if (i4>=T)
				  w4=0;
			  
		     min=energycalc(2*i,2*j+1,blue,w1,w2,w3,w4,width);flag=0;
			 syncthreads();
			 save=blue[2*i+(2*j+1)*width];
			 blue[2*i+(2*j+1)*width]=save+del;
			 syncthreads();
			 uplus=energycalc(2*i,2*j+1,blue,w1,w2,w3,w4,width);
			 if(uplus<min)
			 {min=uplus;flag=1;}
			 blue[2*i+(2*j+1)*width]=save-del;
			 syncthreads();
			 uminus=energycalc(2*i,2*j+1,blue,w1,w2,w3,w4,width);
			 if(uminus<min)
			 {min=uminus;flag=2;}

			 switch(flag)
			 {
			 case 0:
				 {blue[2*i+(2*j+1)*width]=save;
				 break;}
			 case 1:
				 {blue[2*i+(2*j+1)*width]=save+del;
				 break;}
			 case 2:
				 {blue[2*i+(2*j+1)*width]=save-del;
				 break;}
			 }
			 
			  i1= -0.5*(green[2*i+1+(2*j+2)*width]-green[2*i+(2*j+1)*width]);
			  i2= -0.5*(green[2*i+1+(2*j)*width]-green[2*i+(2*j+1)*width]);
			  i3= -0.5*(green[2*i-1+(2*j+2)*width]-green[2*i+(2*j+1)*width]);
			  i4= -0.5*(green[2*i-1+(2*j)*width]-green[2*i+(2*j+1)*width]);
			  if (i1<T)
				  w1=1;
			  else if (i1>=T)
				  w1=0;
			  if (i2<T)
				  w2=1;
			  else if (i2>=T)
				  w2=0;
			  if (i3<T)
				  w3=1;
			  else if (i3>=T)
				  w3=0;
			  if (i4<T)
				  w4=1;
			  else if (i4>=T)
				  w4=0;
			  
		     min=energycalc(2*i,2*j+1,green,w1,w2,w3,w4,width);flag=0;
			 syncthreads();
			 save=green[2*i+(2*j+1)*width];
			 green[2*i+(2*j+1)*width]=save+del;
			 syncthreads();
			 uplus=energycalc(2*i,2*j+1,green,w1,w2,w3,w4,width);
			 if(uplus<min)
			 {min=uplus;flag=1;}
			 green[2*i+(2*j+1)*width]=save-del;
			 syncthreads();
			 uminus=energycalc(2*i,2*j+1,green,w1,w2,w3,w4,width);
			 if(uminus<min)
			 {min=uminus;flag=2;}

			 switch(flag)
			 {
			 case 0:
				 {green[2*i+(2*j+1)*width]=save;
				 break;}
			 case 1:
				 {green[2*i+(2*j+1)*width]=save+del;
				 break;}
			 case 2:
				 {green[2*i+(2*j+1)*width]=save-del;
				 break;}
			 }
		__syncthreads(); 
       }
}

//function for applying iterative correction to (2i+1,2j)
__global__ void Iterations3(int *red,int *green,int *blue,int width, int height)
{
	int i = (blockIdx.x * blockDim.x) + threadIdx.x+1;
	int j = (blockIdx.y * blockDim.y) + threadIdx.y+1;
	int save;
	float i1,i2,i3,i4,min,uplus,uminus;int flag,w1=0,w2=0,w3=0,w4=0;
	for(int l=1;l<1;l++)
	{
	  		  i1= -0.5*(red[2*i+2+(2*j+1)*width]-red[2*i+1+(2*j)*width]);
			  i2= -0.5*(red[2*i+2+(2*j-1)*width]-red[2*i+1+(2*j)*width]);
			  i3= -0.5*(red[2*i+(2*j+1)*width]-red[2*i+1+(2*j)*width]);
			  i4= -0.5*(red[2*i+(2*j-1)*width]-red[2*i+1+(2*j)*width]);
			  if (i1<T)
				  w1=1;
			  else if (i1>=T)
				  w1=0;
			  if (i2<T)
				  w2=1;
			  else if (i2>=T)
				  w2=0;
			  if (i3<T)
				  w3=1;
			  else if (i3>=T)
				  w3=0;
			  if (i4<T)
				  w4=1;
			  else if (i4>=T)
				  w4=0;
			  
		     min=energycalc(2*i+1,2*j,red,w1,w2,w3,w4,width);flag=0;
			 syncthreads();
			 save=red[2*i+1+(2*j)*width];
			 red[2*i+1+(2*j)*width]=save+del;
			 syncthreads();
			 uplus=energycalc(2*i+1,2*j,red,w1,w2,w3,w4,width);
			 if(uplus<min)
			 {min=uplus;flag=1;}
			 red[2*i+1+(2*j)*width]=save-del;
			 syncthreads();
			 uminus=energycalc(2*i+1,2*j,red,w1,w2,w3,w4,width);
			 if(uminus<min)
			 {min=uminus;flag=2;}

			 switch(flag)
			 {
			 case 0:
				 {red[2*i+1+(2*j)*width]=save;
				 break;}
			 case 1:
				 {red[2*i+1+(2*j)*width]=save+del;
				 break;}
			 case 2:
				 {red[2*i+1+(2*j)*width]=save-del;
				 break;}
			 }
			 i1= -0.5*(blue[2*i+2+(2*j+1)*width]-blue[2*i+1+(2*j)*width]);
			  i2= -0.5*(blue[2*i+2+(2*j-1)*width]-blue[2*i+1+(2*j)*width]);
			  i3= -0.5*(blue[2*i+(2*j+1)*width]-blue[2*i+1+(2*j)*width]);
			  i4= -0.5*(blue[2*i+(2*j-1)*width]-blue[2*i+1+(2*j)*width]);
			  if (i1<T)
				  w1=1;
			  else if (i1>=T)
				  w1=0;
			  if (i2<T)
				  w2=1;
			  else if (i2>=T)
				  w2=0;
			  if (i3<T)
				  w3=1;
			  else if (i3>=T)
				  w3=0;
			  if (i4<T)
				  w4=1;
			  else if (i4>=T)
				  w4=0;
			  
		     min=energycalc(2*i+1,2*j,blue,w1,w2,w3,w4,width);flag=0;
			 syncthreads();
			 save=blue[2*i+1+(2*j)*width];
			 blue[2*i+1+(2*j)*width]=save+del;
			 syncthreads();
			 uplus=energycalc(2*i+1,2*j,blue,w1,w2,w3,w4,width);
			 if(uplus<min)
			 {min=uplus;flag=1;}
			 blue[2*i+1+(2*j)*width]=save-del;
			 syncthreads();
			 uminus=energycalc(2*i+1,2*j,blue,w1,w2,w3,w4,width);
			 if(uminus<min)
			 {min=uminus;flag=2;}

			 switch(flag)
			 {
			 case 0:
				 {blue[2*i+1+(2*j)*width]=save;
				 break;}
			 case 1:
				 {blue[2*i+1+(2*j)*width]=save+del;
				 break;}
			 case 2:
				 {blue[2*i+1+(2*j)*width]=save-del;
				 break;}
			 }
			 
			  i1= -0.5*(green[2*i+2+(2*j+1)*width]-green[2*i+1+(2*j)*width]);
			  i2= -0.5*(green[2*i+2+(2*j-1)*width]-green[2*i+1+(2*j)*width]);
			  i3= -0.5*(green[2*i+(2*j+1)*width]-green[2*i+1+(2*j)*width]);
			  i4= -0.5*(green[2*i+(2*j-1)*width]-green[2*i+1+(2*j)*width]);
			  if (i1<T)
				  w1=1;
			  else if (i1>=T)
				  w1=0;
			  if (i2<T)
				  w2=1;
			  else if (i2>=T)
				  w2=0;
			  if (i3<T)
				  w3=1;
			  else if (i3>=T)
				  w3=0;
			  if (i4<T)
				  w4=1;
			  else if (i4>=T)
				  w4=0;
			  
		     min=energycalc(2*i+1,2*j,green,w1,w2,w3,w4,width);flag=0;
			 syncthreads();
			 save=green[2*i+1+(2*j)*width];
			 green[2*i+1+(2*j)*width]=save+del;
			 syncthreads();
			 uplus=energycalc(2*i+1,2*j,green,w1,w2,w3,w4,width);
			 if(uplus<min)
			 {min=uplus;flag=1;}
			 green[2*i+1+(2*j)*width]=save-del;
			 syncthreads();
			 uminus=energycalc(2*i+1,2*j,green,w1,w2,w3,w4,width);
			 if(uminus<min)
			 {min=uminus;flag=2;}

			 switch(flag)
			 {
			 case 0:
				 {green[2*i+1+(2*j)*width]=save;
				 break;}
			 case 1:
				 {green[2*i+1+(2*j)*width]=save+del;
				 break;}
			 case 2:
				 {green[2*i+1+(2*j)*width]=save-del;
				 break;}
			 }
		__syncthreads(); 
       }
}

void resize(int height, int width) {
    const float ar = (float) width / (float) height;
    glViewport(0, 10, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 90.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}
static void Draw(void)
{   
	GLuint texture;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f,0.0f,-3.01f);
    texture = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
    (
        str3,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
	glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    glEnd();
    glutSwapBuffers();
}
	int main(int argc, char **argv)
	{
	static int count1=0,count2=0;
	clock_t begin, end;
    double time_spent;
	float min,secd1,secd2,u,uplus,uminus;
	int save,flag,x1,x2,x3,max;
	int *red;
	int *green;
	int *blue;
	int *devPtrR;
	int *devPtrG;
	int *devPtrB;
	int th=22;
	int delh=2;
	float elapsedTime;
	int width = 1024, height =1024;
	int loopOverFrames=0;
	dim3 grid(63,63);
	dim3 blockallot(8,8);
	cudaEvent_t start, stop;
	cudaEventCreate( &start );
	cudaEventCreate( &stop ) ;
	//Allocating 2D arrays on GPU for copying data
	cudaMalloc((void**)&devPtrR, sizeof(int) * width*height);
	cudaMalloc((void**)&devPtrG, sizeof(int) * width*height);
	cudaMalloc((void**)&devPtrB, sizeof(int) * width*height);
	cudaMemcpyToSymbol(T,&th,1*sizeof(int),0,cudaMemcpyHostToDevice);
	cudaMemcpyToSymbol(del,&delh,1*sizeof(char),0,cudaMemcpyHostToDevice);
	red=(int*)malloc(width*height*sizeof(int));
	green=(int*)malloc(width*height*sizeof(int));
	blue=(int*)malloc(width*height*sizeof(int));
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInit(&argc, argv);
    glutInitWindowSize(1024,1024);
    glutCreateWindow("COP - CUDA VIDEO ENHANCEMENT");
    glutReshapeFunc(resize);
    glutDisplayFunc(Draw);
	glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	begin=clock();	
	
	//video capturing first part
	CvCapture* capture = cvCaptureFromFile("try.flv");
	IplImage* frame = NULL;
    do
    {
        frame = skipNFrames(capture, 1);
        cvNamedWindow("frame", CV_WINDOW_AUTOSIZE);
        cvShowImage("frame", frame);
        //cvWaitKey(0);
		char *str=new char[50];
		FLAG++;
		sprintf(str,"%d",FLAG);
		strcat(str,"_frame");
		strcat(str,".jpg");
		Mat image=frame;
		imwrite(str,image);
    }while(frame!=NULL );

    cvReleaseCapture(&capture);
    cvDestroyWindow("frame");
    cvReleaseImage(&frame);   
	
	
	for(loopOverFrames=1;loopOverFrames<=10;loopOverFrames++)
	{
		char *str2=new char[50];
		sprintf(str2,"%d",loopOverFrames);
		strcat(str2,"_frame");
		strcat(str2,".jpg");
		//cout<<str2;
		CImg<unsigned char> image(str2);

//Initializing the new upscaled image
	
	for(int j=0;j<512;j++)
    {
	  for(int i=0;i<512;i++)
	  {
		  for(int k=0;k<3;k++)
		  {  
			 
			  switch(k)
				  {
					case 0:
					{
						red[(2*i)+(2*j)*width]=image(i,j,k);
						imageup((2*i),(2*j),k)=red[(2*i)+(2*j)*width];
						break;
					}
				   case 1:
					{
						green[(2*i)+(2*j)*width]=image(i,j,k);
						imageup((2*i),(2*j),k)=green[(2*i)+(2*j)*width];
						break;
					}
					case 2:
					{
						blue[(2*i)+(2*j)*width]=image(i,j,k);
						imageup((2*i),(2*j),k)=blue[(2*i)+(2*j)*width];
						break;
					}
				 }
		  }
	  }
	}
	
    //kernel call for FAST INTERPOLATION
	cudaEventRecord( start, 0 );
	cudaMemcpy(devPtrR, red, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	cudaMemcpy(devPtrG, green, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	cudaMemcpy(devPtrB, blue, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	
	FastInterpolation1<<<grid,blockallot>>>(devPtrR,devPtrG,devPtrB,width,height);
	 
	cudaMemcpy(red,devPtrR, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	cudaMemcpy(green,devPtrG, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	cudaMemcpy(blue,devPtrB, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	
	//kernel call for iterating 2i+1,2j+1
	
	/*
	cudaMemcpy(devPtrR, red, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	cudaMemcpy(devPtrG, green, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	cudaMemcpy(devPtrB, blue, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	
	Iterations1<<<grid,blockallot>>>(devPtrR,devPtrG,devPtrB,width,height);
	
	cudaMemcpy(red,devPtrR, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	cudaMemcpy(green,devPtrG, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	cudaMemcpy(blue,devPtrB, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	*/
	
	//STAGE 2 - FILLING the other two
	//filling 2i,2j+1
	cudaMemcpy(devPtrR, red, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	cudaMemcpy(devPtrG, green, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	cudaMemcpy(devPtrB, blue, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	
	FastInterpolation2<<<grid,blockallot>>>(devPtrR,devPtrG,devPtrB,width,height);
	 
	cudaMemcpy(red,devPtrR, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	cudaMemcpy(green,devPtrG, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	cudaMemcpy(blue,devPtrB, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	/*
	cudaMemcpy(devPtrR, red, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	cudaMemcpy(devPtrG, green, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	cudaMemcpy(devPtrB, blue, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	
	Iterations2<<<grid,blockallot>>>(devPtrR,devPtrG,devPtrB,width,height);
	
	cudaMemcpy(red,devPtrR, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	cudaMemcpy(green,devPtrG, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	cudaMemcpy(blue,devPtrB, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	*/
	//kernel call for iterations
	
	//filling 2i+1,2j
	cudaMemcpy(devPtrR, red, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	cudaMemcpy(devPtrG, green, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	cudaMemcpy(devPtrB, blue, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	
	FastInterpolation3<<<grid,blockallot>>>(devPtrR,devPtrG,devPtrB,width,height);
	 
	cudaMemcpy(red,devPtrR, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	cudaMemcpy(green,devPtrG, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	cudaMemcpy(blue,devPtrB, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	
	
	
	/*
	//kernel call for iterations
	cudaMemcpy(devPtrR, red, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	cudaMemcpy(devPtrG, green, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	cudaMemcpy(devPtrB, blue, sizeof(int) * width*height, cudaMemcpyHostToDevice);
	
	Iterations3<<<grid,blockallot>>>(devPtrR,devPtrG,devPtrB,width,height);
	
	cudaMemcpy(red,devPtrR, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	cudaMemcpy(green,devPtrG, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	cudaMemcpy(blue,devPtrB, sizeof(int) * width*height, cudaMemcpyDeviceToHost);
	*/
	cudaEventRecord( stop, 0 );
	cudaEventSynchronize( stop ) ;
	cudaEventElapsedTime( &elapsedTime,start, stop );
	cudaEventDestroy( start );  
	cudaEventDestroy( stop );
	printf( "Time to generate: %3.1f ms\n", elapsedTime ); 
	//Iterating for the N times
	
	for(int j=1;j<510;j++)
    {
	  for(int i=1;i<510;i++)
	  {
		  for(int k=0;k<3;k++)
		  {
			switch(k)
				  {
					case 0:
					{
						imageup((2*i+1),(2*j+1),k)=red[(2*i+1)+width*(2*j+1)];
						imageup((2*i+1),(2*j),k)=red[(2*i+1)+width*(2*j)];
						imageup(2*i,2*j+1,k)=red[2*i+(2*j+1)*width];
						break;
					}
				   case 1:
					{
						imageup((2*i+1),(2*j+1),k)=green[(2*i+1)+width*(2*j+1)];
						imageup((2*i+1),(2*j),k)=green[(2*i+1)+width*(2*j)];
						imageup(2*i,2*j+1,k)=green[2*i+(2*j+1)*width];

						break;
					}
					case 2:
					{
						imageup((2*i+1),(2*j+1),k)=blue[(2*i+1)+width*(2*j+1)];
						imageup((2*i+1),(2*j),k)=blue[(2*i+1)+width*(2*j)];
						imageup(2*i,2*j+1,k)=blue[2*i+(2*j+1)*width];
						break;
					}
				 }
			 }
	  }
	 
	}
			
	 for(int j=2;j<1020;j++)
    {
	  for(int i=2;i<1020;i++)
	  {
		  for(int k=0;k<3;k++)
		  { if((imageup(i,j,k)>1.05*imageup(i+1,j,k))&&(imageup(i,j,k)>1.05*imageup(i-1,j,k)))
		           imageup(i,j,k)=(imageup(i-1,j,k)+imageup(i+1,j,k))/2;
		  }
	  }
	 }
	 for(int j=1;j<4;j++)
    {
	  for(int i=0;i<1024;i++)
	  {
		  for(int k=0;k<3;k++)
		  { 
			  if((imageup(i,2*j+1,k)>1.05*imageup(i,2*j,k))&&(imageup(i,2*j+1,k)>1.05*imageup(i,2*j+2,k)))
		           imageup(i,2*j+1,k)=(imageup(i,2*j,k)+imageup(i,2*j+2,k))/2;
		  }
	  }
	 }
	 for(int j=508;j<510;j++)
    {
	  for(int i=0;i<1024;i++)
	  {
		  for(int k=0;k<3;k++)
		  { 
			  if((imageup(i,2*j+1,k)>1.05*imageup(i,2*j,k))&&(imageup(i,2*j+1,k)>1.05*imageup(i,2*j+2,k)))
		           imageup(i,2*j+1,k)=(imageup(i,2*j,k)+imageup(i,2*j+2,k))/2;
		  }
	  }
	 }
	 for(int j=1;j<1023;j++)
    {
	  for(int i=2;i<10;i++)
	  {
		  for(int k=0;k<3;k++)
		  { 
			  if((imageup(i,j,k)>1.05*imageup(i,j-1,k))&&(imageup(i,j,k)>1.05*imageup(i,j+1,k)))
		           imageup(i,j,k)=(imageup(i,j-1,k)+imageup(i,j+1,k))/2;
		  }
	  }
	 }
	  for(int j=1;j<1023;j++)
    {
	  for(int i=1017;i<1020;i++)
	  {
		  for(int k=0;k<3;k++)
		  { 
			  if((imageup(i,j,k)>1.05*imageup(i,j-1,k))&&(imageup(i,j,k)>1.05*imageup(i,j+1,k)))
		           imageup(i,j,k)=(imageup(i,j-1,k)+imageup(i,j+1,k))/2;
		  }
	  }
	 }

	  //filling up pixels at boundary//
	for(int j=0;j<3;j++){
		for(int i=0;i<510;i++){
			for(int k=0;k<3;k++){
				imageup(2*i+1,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+2,2*j+2,k)+imageup(2*i,2*j+2,k))/4;
				imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j+1,k))/3;
			}
		}
	}
	for(int j=0;j<3;j++){
		for(int i=1;i<510;i++){
			for(int k=0;k<3;k++){
				imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i+1,2*j+1,k)+imageup(2*i,2*j+2,k)+imageup(2*i-1,2*j+1,k))/4;
			}
		}
	}
	for(int j=510;j<511;j++){
		for(int i=0;i<510;i++){
			for(int k=0;k<3;k++){
				imageup(2*i+1,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+2,2*j+2,k)+imageup(2*i,2*j+2,k))/4;
				if(i==0)
					imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i+1,2*j+1,k))/3;
				else
					imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i+1,2*j+1,k)+imageup(2*i-1,2*j+1,k))/4;
				imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j+1,k))/3;
			}
		}
	}
	for(int j=511;j<512;j++){
		for(int i=0;i<510;i++){
			for(int k=0;k<3;k++){
				imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j-1,k))/3;
				imageup(2*i+1,2*j+1,k)=imageup(2*i+1,2*j,k);
				imageup(2*i,2*j+1,k)=imageup(2*i,2*j,k);
			}
		}
	}
	
	for(int j=0;j<510;j++){
		for(int i=0;i<3;i++){
			for(int k=0;k<3;k++){
				imageup(2*i+1,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+2,2*j+2,k)+imageup(2*i,2*j+2,k))/4;
				imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i+1,2*j+1,k))/3;
			}
		}
	}
	for(int j=1;j<510;j++){
		for(int i=0;i<3;i++){
			for(int k=0;k<3;k++){
				imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+1,2*j-1,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j+1,k))/4;
			}
		}
	}
	for(int j=0;j<510;j++){
		for(int i=510;i<511;i++){
			for(int k=0;k<3;k++){
				imageup(2*i+1,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+2,2*j+2,k)+imageup(2*i,2*j+2,k))/4;
				if(j==0)
					imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j+1,k))/3;
				else
					imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j+1,k)+imageup(2*i+1,2*j-1,k))/4;
				imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i+1,2*j+1,k))/3;
			}
		}
	}
	for(int j=0;j<510;j++){
		for(int i=511;i<512;i++){
			for(int k=0;k<3;k++){
				imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i-1,2*j+1,k))/3;
				imageup(2*i+1,2*j+1,k)=imageup(2*i,2*j+1,k);
				imageup(2*i+1,2*j,k)=imageup(2*i,2*j,k);
			}
		}
	}
	
	for(int j=510;j<512;j++){
		for(int i=510;i<512;i++){
			for(int k=0;k<3;k++){
				if(i==510 && j==510){
					imageup(2*i+1,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i+2,2*j+2,k)+imageup(2*i+2,2*j,k))/4;
					imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i-1,2*j+1,k)+imageup(2*i+1,2*j+1,k))/4;
					imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j-1,k)+imageup(2*i+1,2*j+1,k))/4;
				}
				if(i==511 && j==510){
					imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i-1,2*j+1,k))/3;
				}
				if(i==510 && j==511){
					imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j-1,k))/3;
				}
				if(i==511){
					imageup(2*i+1,2*j+1,k)=imageup(2*i,2*j+1,k);
					imageup(2*i+1,2*j,k)=imageup(2*i,2*j,k);
				}
				if(j==511){
					imageup(2*i+1,2*j+1,k)=imageup(2*i+1,2*j,k);
					imageup(2*i,2*j+1,k)=imageup(2*i,2*j,k);
				}
			}
		}
	}
		sprintf(str3,"%d",loopOverFrames);
		strcat(str3,"_upscaledFrame");
		strcat(str3,".bmp");
		cout<<str3;
		imageup.save(str3);
		glutMainLoop();
			}
	end=clock();
	 time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Time in millisec = %f\n",time_spent);
	getch();
	 return 0;

}

