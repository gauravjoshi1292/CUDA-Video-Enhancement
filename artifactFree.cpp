#include "opencv2/opencv.hpp"
#pragma comment (lib , "opencv_core244d.lib")
#pragma comment (lib ,"opencv_highgui244d.lib")
#pragma comment(lib , "opencv_imgproc244d.lib")
#include <cv.h>
#include <highgui.h>
#include"CImg.h"
#include<conio.h>
#include<stdio.h>
#include<iostream>
#include<math.h>
#include<time.h>
#define T 22
#define del 2
int w1=0,w2=0,w3=0,w4=0,i1,i2,i3,i4,FLAG=0;
float uc,ue;
using namespace cimg_library;
using namespace cv;
using namespace std;
CImg<unsigned char> image_xx("0_frame.jpg");
CImg<unsigned char> imageup(2*image_xx.width(),2*image_xx.height(),1,3);
int I11(int i,int j,int k)
{
	return (imageup(i - 2, j - 2,k) + imageup(i + 2, j + 2,k) - 2*imageup(i , j ,k));
}
int I22(int i,int j,int k)
{
	return (imageup(i-2,j+2,k)+imageup(i+2,j-2,k)+imageup(i,j,k));
}
int mod(int i)
{
	if (i<0)
		return i*-1;
	else
		return i;
}
float energycalc(int i,int j,int k)
{  float u;
	uc=w1*(mod((I11(i,j,k) - I11(i+1,j+1,k))) +
                     mod(I22(i,j,k) - I22(i+1,j+1,k))) +
				w2*(mod(I11(i,j,k) - I11(i+1,j-1,k)) +
					mod(I22(i,j,k) - I22(i+1,j-1,k)))+
				w3*(mod(I11(i,j,k) - I11(i-1,j+1,k)) +
					mod(I22(i,j,k) - I22(i-1,j+1,k))) +
				w4*(mod(I11(i,j,k) - I11(i-1,j-1,k)) +
					mod(I22(i,j,k) - I22(i-1,j-1,k)));
			  ue=-1*(mod(I11(i,j,k))+mod(I22(i,j,k)));
			  u=5*uc-2*ue;
return u;
}

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

void main() {
	static int count1=0,count2=0;
	clock_t begin, end;
    double time_spent;
   	begin =clock();
	float min,secd1,secd2,u,uplus,uminus;
	int save,flag,x1,x2,x3,max;
	int loopOverFrames=0;
	
	/*
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
	*/
	
	//enlarging second part
	
	for(loopOverFrames=1;loopOverFrames<=1;loopOverFrames++)
	{
		char *str2=new char[50];
		sprintf(str2,"%d",loopOverFrames);
		strcat(str2,"_frame");
		strcat(str2,".jpg");
		//cout<<str2;
		CImg<unsigned char> image(str2);

		//Initializing the new upscaled image
		for(int j=0;j<image.height();j++)
		{
		  for(int i=0;i<image.width();i++)
		  {
			  for(int k=0;k<3;k++)
			  {
				  imageup(2*i,2*j,k)=image(i,j,k);

			  }
		  }
		}
		imageup.save("initialized.bmp");
		//cout<<image.spectrum();
		//FAST INTERPOLATION
		//STAGE 1 - FILLING 2i+1,2j+1
		 for(int j=1;j<image.height()-2;j++)
		{
		  for(int i=1;i<image.width()-2;i++)
		  {
			  for(int k=0;k<image.spectrum();k++)
			  {
				secd1=imageup(2*i-2, 2*j + 2,k) + imageup(2*i, 2*j,k)+imageup(2*i + 2, 2*j - 2,k) - 3*imageup(2*i, 2*j + 2,k)-3*imageup(2*i + 2, 2*j,k) +imageup(2*i, 2*j + 4,k) + imageup(2*i + 2, 2*j + 2,k) + imageup(2*i + 4, 2*j,k);
				secd2=imageup(2*i, 2*j-2,k) + imageup(2*i + 2, 2*j,k)+imageup(2*i + 4, 2*j + 2,k) - 3*imageup(2*i, 2*j,k) -3*imageup(2*i + 2, 2*j + 2,k)+imageup(2*i- 2, 2*j,k) + imageup(2*i, 2*j + 2,k) + imageup(2*i + 2, 2*j + 4,k);
				if(secd1<secd2)
					imageup(2*i+1,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j+2))/2;
				else
					imageup(2*i+1,2*j+1,k)=(imageup(2*i+2,2*j,k)+imageup(2*i,2*j+2,k))/2;
			  }
		  }
		 }
		 imageup.save("stage1.bmp");
	 
		 //Iterating for the N times
		for(int l=100;l<1;l++)
		{
		 for(int j=1;j<image.height()-2;j++)
		{
		  for(int i=1;i<image.width()-2;i++)
		  {
			  for(int k=0;k<image.spectrum();k++)
			  {
				  i1= -0.5*(imageup(2*i+2,2*j+2,k)-imageup(2*i+1,2*j+1,k));
				  i2= -0.5*(imageup(2*i+2,2*j,k)-imageup(2*i+1,2*j+1,k));
				  i3= -0.5*(imageup(2*i,2*j+2,k)-imageup(2*i+1,2*j+1,k));
				  i4= -0.5*(imageup(2*i,2*j,k)-imageup(2*i+1,2*j+1,k));
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
			  
				 min=energycalc(2*i+1,2*j+1,k);flag=0;
				 save=imageup(2*i+1,2*j+1,k);
				 imageup(2*i+1,2*j+1,k)=imageup(2*i+1,2*j+1,k)+del;
				 uplus=energycalc(2*i+1,2*j+1,k);
				 if(uplus<min)
				 {min=uplus;flag=1;}
				 imageup(2*i+1,2*j+1,k)=imageup(2*i+1,2*j+1,k)-2*del;
				 uminus=energycalc(2*i+1,2*j+1,k);
				 if(uminus<min)
				 {min=uminus;flag=2;}

				 switch(flag)
				 {
				 case 0:
					 {imageup(2*i+1,2*j+1,k)=save;
					 break;}
				 case 1:
					 {imageup(2*i+1,2*j+1,k)=save+del;
					 break;}
				 case 2:
					 {imageup(2*i+1,2*j+1,k)=save-del;
					 break;}
				 }
			  }
		  }
		 }
		}
	
		imageup.save("stage1_afterIteration.bmp");

		//STAGE 2 - FILLING the other two
		//filling 2i,2j+1
		 for(int j=1;j<image.height()-2;j++)
		{
		  for(int i=1;i<image.width()-2;i++)
		  {
			  for(int k=0;k<image.spectrum();k++)
			  {
				secd1=imageup(2*i-3, 2*j + 2,k) + imageup(2*i-1, 2*j,k)+imageup(2*i + 1, 2*j - 2,k) - 3*imageup(2*i-1, 2*j + 2,k)-3*imageup(2*i + 1, 2*j,k) +imageup(2*i-1, 2*j + 4,k) + imageup(2*i + 1, 2*j + 2,k) + imageup(2*i + 3, 2*j,k);
				secd2=imageup(2*i-1, 2*j-2,k) + imageup(2*i + 1, 2*j,k)+imageup(2*i + 3, 2*j + 2,k) - 3*imageup(2*i-1, 2*j,k) -3*imageup(2*i + 1, 2*j + 2,k)+imageup(2*i- 3, 2*j,k) + imageup(2*i-1, 2*j + 2,k) + imageup(2*i + 1, 2*j + 4,k);
				if(secd1<secd2)
					imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j+2))/2;
				else
					imageup(2*i,2*j+1,k)=(imageup(2*i+2,2*j,k)+imageup(2*i,2*j+2,k))/2;
			  }
		  }
		 }
		 imageup.save("stage2.bmp");

		 //Iterating for the N times
		for(int l=100;l<1;l++)
		{
		 for(int j=1;j<image.height()-2;j++)
		{
		  for(int i=1;i<image.width()-2;i++)
		  {
			  for(int k=0;k<image.spectrum();k++)
			  {
				  i1= -0.5*(imageup(2*i+1,2*j+2,k)-imageup(2*i,2*j+1,k));
				  i2= -0.5*(imageup(2*i+1,2*j,k)-imageup(2*i,2*j+1,k));
				  i3= -0.5*(imageup(2*i-1,2*j+2,k)-imageup(2*i,2*j+1,k));
				  i4= -0.5*(imageup(2*i-1,2*j,k)-imageup(2*i,2*j+1,k));
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

				// printf("i=%d\tj=%d\n",i,j);
				 min=energycalc(2*i,2*j+1,k);flag=0;
				 save=imageup(2*i,2*j+1,k);
				 imageup(2*i,2*j+1,k)=imageup(2*i,2*j+1,k)+del;
				 uplus=energycalc(2*i,2*j+1,k);
				 if(uplus<min)
				 {min=uplus;flag=1;}
				 imageup(2*i,2*j+1,k)=imageup(2*i,2*j+1,k)-2*del;
				 uminus=energycalc(2*i,2*j+1,k);
				 if(uminus<min)
				 {min=uminus;flag=2;}

				 switch(flag)
				 {
				 case 0:
					 {imageup(2*i,2*j+1,k)=save;
					 break;}
				 case 1:
					 {imageup(2*i,2*j+1,k)=save+del;
					 break;}
				 case 2:
					 {imageup(2*i,2*j+1,k)=save-del;
					 break;}
				 }
			  }
		  }
		 }
		}
		imageup.save("stage2_afterIteration.bmp");

		//STAGE 3 - FILLING the last two
		//filling 2i+1,2j
		 for(int j=2;j<image.height()-2;j++)
		{
		  for(int i=2;i<image.width()-2;i++)
		  {
			  for(int k=0;k<image.spectrum();k++)
			  {
				secd1=imageup(2*i-2, 2*j + 1,k) + imageup(2*i, 2*j-1,k)+imageup(2*i + 2, 2*j - 3,k) - 3*imageup(2*i, 2*j + 1,k)-3*imageup(2*i + 2, 2*j-1,k) +imageup(2*i, 2*j + 3,k) + imageup(2*i + 2, 2*j + 1,k) + imageup(2*i + 4, 2*j-1,k);
				secd2=imageup(2*i, 2*j-3,k) + imageup(2*i + 2, 2*j-1,k)+imageup(2*i + 4, 2*j + 1,k) - 3*imageup(2*i, 2*j-1,k) -3*imageup(2*i + 2, 2*j + 1,k)+imageup(2*i- 2, 2*j-1,k) + imageup(2*i, 2*j + 1,k) + imageup(2*i + 2, 2*j + 3,k);
				if(secd1<secd2)
					imageup(2*i+1,2*j,k)=(imageup(2*i+1,2*j-1,k)+imageup(2*i+3,2*j+1))/2;
				else
					imageup(2*i+1,2*j,k)=(imageup(2*i+3,2*j-1,k)+imageup(2*i+1,2*j+1,k))/2;
			  }
		  }
		 }
		 imageup.save("stage3.bmp");

		 //Iterating for the N times
		for(int l=100;l<1;l++)
		{
		 for(int j=2;j<image.height()-2;j++)
		{
		  for(int i=2;i<image.width()-2;i++)
		  {
			  for(int k=0;k<image.spectrum();k++)
			  {
				  i1= -0.5*(imageup(2*i+2,2*j+1,k)-imageup(2*i+1,2*j,k));
				  i2= -0.5*(imageup(2*i+2,2*j-1,k)-imageup(2*i+1,2*j,k));
				  i3= -0.5*(imageup(2*i,2*j+1,k)-imageup(2*i+1,2*j,k));
				  i4= -0.5*(imageup(2*i,2*j-1,k)-imageup(2*i+1,2*j,k));
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

				// printf("i=%d\tj=%d\n",i,j);
				 min=energycalc(2*i+1,2*j,k);flag=0;
				 save=imageup(2*i+1,2*j,k);
				 imageup(2*i+1,2*j,k)=imageup(2*i+1,2*j,k)+del;
				 uplus=energycalc(2*i+1,2*j,k);
				 if(uplus<min)
				 {min=uplus;flag=1;}
				 imageup(2*i+1,2*j,k)=imageup(2*i+1,2*j,k)-2*del;
				 uminus=energycalc(2*i+1,2*j,k);
				 if(uminus<min)
				 {min=uminus;flag=2;}

				 switch(flag)
				 {
				 case 0:
					 {imageup(2*i+1,2*j,k)=save;
					 break;}
				 case 1:
					 {imageup(2*i+1,2*j,k)=save+del;
					 break;}
				 case 2:
					 {imageup(2*i+1,2*j,k)=save-del;
					 break;}
				 }
			  }
		  }
		 }
		}
		imageup.save("stage3_afterIteration.bmp");

		//STAGE 4
		//now applying iterative correction to the enlarged image

		 //Iterating for the N times on 2i+1,2j+1
 		for(int l=100;l<1;l++)
		{
		 for(int j=1;j<image.height()-2;j++)
		{
		  for(int i=1;i<image.width()-2;i++)
		  {
			  for(int k=0;k<image.spectrum();k++)
			  {
				  i1= -0.5*(imageup(2*i+2,2*j+2,k)-imageup(2*i+1,2*j+1,k));
				  i2= -0.5*(imageup(2*i+2,2*j,k)-imageup(2*i+1,2*j+1,k));
				  i3= -0.5*(imageup(2*i,2*j+2,k)-imageup(2*i+1,2*j+1,k));
				  i4= -0.5*(imageup(2*i,2*j,k)-imageup(2*i+1,2*j+1,k));
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

				 //printf("i=%d\tj=%d\n",i,j);
				 min=energycalc(2*i+1,2*j+1,k);flag=0;
				 save=imageup(2*i+1,2*j+1,k);
				 imageup(2*i+1,2*j+1,k)=imageup(2*i+1,2*j+1,k)+del;
				 uplus=energycalc(2*i+1,2*j+1,k);
				 if(uplus<min)
				 {min=uplus;flag=1;}
				 imageup(2*i+1,2*j+1,k)=imageup(2*i+1,2*j+1,k)-2*del;
				 uminus=energycalc(2*i+1,2*j+1,k);
				 if(uminus<min)
				 {min=uminus;flag=2;}

				 switch(flag)
				 {
				 case 0:
					 {imageup(2*i+1,2*j+1,k)=save;
					 break;}
				 case 1:
					 {imageup(2*i+1,2*j+1,k)=save+del;
					 break;}
				 case 2:
					 {imageup(2*i+1,2*j+1,k)=save-del;
					 break;}
				 }
			  }
		  }
		 }
		}
		 //Iterating for the N times on 2i,2j+1
		for(int l=100;l<1;l++)
		{
		 for(int j=1;j<image.height()-2;j++)
		{
		  for(int i=1;i<image.width()-2;i++)
		  {
			  for(int k=0;k<image.spectrum();k++)
			  {
				  i1= -0.5*(imageup(2*i+1,2*j+2,k)-imageup(2*i,2*j+1,k));
				  i2= -0.5*(imageup(2*i+1,2*j,k)-imageup(2*i,2*j+1,k));
				  i3= -0.5*(imageup(2*i-1,2*j+2,k)-imageup(2*i,2*j+1,k));
				  i4= -0.5*(imageup(2*i-1,2*j,k)-imageup(2*i,2*j+1,k));
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

			  //   printf("i=%d\tj=%d\n",i,j);
				 min=energycalc(2*i,2*j+1,k);flag=0;
				 save=imageup(2*i,2*j+1,k);
				 imageup(2*i,2*j+1,k)=imageup(2*i,2*j+1,k)+del;
				 uplus=energycalc(2*i,2*j+1,k);
				 if(uplus<min)
				 {min=uplus;flag=1;}
				 imageup(2*i,2*j+1,k)=imageup(2*i,2*j+1,k)-2*del;
				 uminus=energycalc(2*i,2*j+1,k);
				 if(uminus<min)
				 {min=uminus;flag=2;}

				 switch(flag)
				 {
				 case 0:
					 {imageup(2*i,2*j+1,k)=save;
					 break;}
				 case 1:
					 {imageup(2*i,2*j+1,k)=save+del;
					 break;}
				 case 2:
					 {imageup(2*i,2*j+1,k)=save-del;
					 break;}
				 }
			  }
		  }
		 }
		}
		//Iterating for the N times on 2i+1,2j
		for(int l=100;l<1;l++)
		{
		 for(int j=2;j<image.height()-2;j++)
		{
		  for(int i=2;i<image.width()-2;i++)
		  {
			  for(int k=0;k<image.spectrum();k++)
			  {
				  i1= -0.5*(imageup(2*i+2,2*j+1,k)-imageup(2*i+1,2*j,k));
				  i2= -0.5*(imageup(2*i+2,2*j-1,k)-imageup(2*i+1,2*j,k));
				  i3= -0.5*(imageup(2*i,2*j+1,k)-imageup(2*i+1,2*j,k));
				  i4= -0.5*(imageup(2*i,2*j-1,k)-imageup(2*i+1,2*j,k));
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

			   //  printf("i=%d\tj=%d\n",i,j);
				 min=energycalc(2*i+1,2*j,k);flag=0;
				 save=imageup(2*i+1,2*j,k);
				 imageup(2*i+1,2*j,k)=imageup(2*i+1,2*j,k)+del;
				 uplus=energycalc(2*i+1,2*j,k);
				 if(uplus<min)
				 {min=uplus;flag=1;}
				 imageup(2*i+1,2*j,k)=imageup(2*i+1,2*j,k)-2*del;
				 uminus=energycalc(2*i+1,2*j,k);
				 if(uminus<min)
				 {min=uminus;flag=2;}

				 switch(flag)
				 {
				 case 0:
					 {imageup(2*i+1,2*j,k)=save;
					 break;}
				 case 1:
					 {imageup(2*i+1,2*j,k)=save+del;
					 break;}
				 case 2:
					 {imageup(2*i+1,2*j,k)=save-del;
					 break;}
				 }
			  }
		  }
		 }
		}
		//imageup.save("stage4.bmp");
		 for(int j=2;j<imageup.height()-4;j++)
		{
		  for(int i=2;i<imageup.width()-4;i++)
		  {
			  for(int k=0;k<imageup.spectrum();k++)
			  { if((imageup(i,j,k)>1.05*imageup(i+1,j,k))&&(imageup(i,j,k)>1.05*imageup(i-1,j,k)))
					   imageup(i,j,k)=(imageup(i-1,j,k)+imageup(i+1,j,k))/2;
			  }
		  }
		 }
		 for(int j=1;j<4;j++)
		{
		  for(int i=0;i<imageup.width();i++)
		  {
			  for(int k=0;k<imageup.spectrum();k++)
			  { 
				  if((imageup(i,2*j+1,k)>1.05*imageup(i,2*j,k))&&(imageup(i,2*j+1,k)>1.05*imageup(i,2*j+2,k)))
					   imageup(i,2*j+1,k)=(imageup(i,2*j,k)+imageup(i,2*j+2,k))/2;
			  }
		  }
		 }
		 for(int j=image.height()-4;j<image.height()-2;j++)
		{
		  for(int i=0;i<imageup.width();i++)
		  {
			  for(int k=0;k<imageup.spectrum();k++)
			  { 
				  if((imageup(i,2*j+1,k)>1.05*imageup(i,2*j,k))&&(imageup(i,2*j+1,k)>1.05*imageup(i,2*j+2,k)))
					   imageup(i,2*j+1,k)=(imageup(i,2*j,k)+imageup(i,2*j+2,k))/2;
			  }
		  }
		 }
		 for(int j=1;j<imageup.height()-1;j++)
		{
		  for(int i=2;i<10;i++)
		  {
			  for(int k=0;k<imageup.spectrum();k++)
			  { 
				  if((imageup(i,j,k)>1.05*imageup(i,j-1,k))&&(imageup(i,j,k)>1.05*imageup(i,j+1,k)))
					   imageup(i,j,k)=(imageup(i,j-1,k)+imageup(i,j+1,k))/2;
			  }
		  }
		 }
		  for(int j=1;j<imageup.height()-1;j++)
		{
		  for(int i=imageup.width()-7;i<imageup.width()-4;i++)
		  {
			  for(int k=0;k<imageup.spectrum();k++)
			  { 
				  if((imageup(i,j,k)>1.05*imageup(i,j-1,k))&&(imageup(i,j,k)>1.05*imageup(i,j+1,k)))
					   imageup(i,j,k)=(imageup(i,j-1,k)+imageup(i,j+1,k))/2;
			  }
		  }
		 }

		  //filling up pixels at boundary//
		for(int j=0;j<3;j++){
			for(int i=0;i<image.width()-2;i++){
				for(int k=0;k<imageup.spectrum();k++){
					imageup(2*i+1,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+2,2*j+2,k)+imageup(2*i,2*j+2,k))/4;
					imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j+1,k))/3;
				}
			}
		}
		for(int j=0;j<3;j++){
			for(int i=1;i<image.width()-2;i++){
				for(int k=0;k<imageup.spectrum();k++){
					imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i+1,2*j+1,k)+imageup(2*i,2*j+2,k)+imageup(2*i-1,2*j+1,k))/4;
				}
			}
		}
		for(int j=image.height()-2;j<image.height()-1;j++){
			for(int i=0;i<image.width()-2;i++){
				for(int k=0;k<imageup.spectrum();k++){
					imageup(2*i+1,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+2,2*j+2,k)+imageup(2*i,2*j+2,k))/4;
					if(i==0)
						imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i+1,2*j+1,k))/3;
					else
						imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i+1,2*j+1,k)+imageup(2*i-1,2*j+1,k))/4;
					imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j+1,k))/3;
				}
			}
		}
		for(int j=image.height()-1;j<image.height();j++){
			for(int i=0;i<image.width()-2;i++){
				for(int k=0;k<image.spectrum();k++){
					imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j-1,k))/3;
					imageup(2*i+1,2*j+1,k)=imageup(2*i+1,2*j,k);
					imageup(2*i,2*j+1,k)=imageup(2*i,2*j,k);
				}
			}
		}
	
		for(int j=0;j<image.height()-2;j++){
			for(int i=0;i<3;i++){
				for(int k=0;k<imageup.spectrum();k++){
					imageup(2*i+1,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+2,2*j+2,k)+imageup(2*i,2*j+2,k))/4;
					imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i+1,2*j+1,k))/3;
				}
			}
		}
		for(int j=1;j<image.height()-2;j++){
			for(int i=0;i<3;i++){
				for(int k=0;k<imageup.spectrum();k++){
					imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+1,2*j-1,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j+1,k))/4;
				}
			}
		}
		for(int j=0;j<image.height()-2;j++){
			for(int i=image.width()-2;i<image.width()-1;i++){
				for(int k=0;k<imageup.spectrum();k++){
					imageup(2*i+1,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+2,2*j+2,k)+imageup(2*i,2*j+2,k))/4;
					if(j==0)
						imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j+1,k))/3;
					else
						imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j+1,k)+imageup(2*i+1,2*j-1,k))/4;
					imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i+1,2*j+1,k))/3;
				}
			}
		}
		for(int j=0;j<image.height()-2;j++){
			for(int i=image.width()-1;i<image.width();i++){
				for(int k=0;k<imageup.spectrum();k++){
					imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i-1,2*j+1,k))/3;
					imageup(2*i+1,2*j+1,k)=imageup(2*i,2*j+1,k);
					imageup(2*i+1,2*j,k)=imageup(2*i,2*j,k);
				}
			}
		}
	
		for(int j=image.height()-2;j<image.height();j++){
			for(int i=image.width()-2;i<image.width();i++){
				for(int k=0;k<imageup.spectrum();k++){
					if(i==image.width()-2 && j==image.height()-2){
						imageup(2*i+1,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i+2,2*j+2,k)+imageup(2*i+2,2*j,k))/4;
						imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i-1,2*j+1,k)+imageup(2*i+1,2*j+1,k))/4;
						imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j-1,k)+imageup(2*i+1,2*j+1,k))/4;
					}
					if(i==image.width()-1 && j==image.height()-2){
						imageup(2*i,2*j+1,k)=(imageup(2*i,2*j,k)+imageup(2*i,2*j+2,k)+imageup(2*i-1,2*j+1,k))/3;
					}
					if(i==image.width()-2 && j==image.height()-1){
						imageup(2*i+1,2*j,k)=(imageup(2*i,2*j,k)+imageup(2*i+2,2*j,k)+imageup(2*i+1,2*j-1,k))/3;
					}
					if(i==image.width()-1){
						imageup(2*i+1,2*j+1,k)=imageup(2*i,2*j+1,k);
						imageup(2*i+1,2*j,k)=imageup(2*i,2*j,k);
					}
					if(j==image.height()-1){
						imageup(2*i+1,2*j+1,k)=imageup(2*i+1,2*j,k);
						imageup(2*i,2*j+1,k)=imageup(2*i,2*j,k);
					}
				}
			}
		}

		char *str3=new char[50];
		sprintf(str3,"%d",loopOverFrames);
		strcat(str3,"_upscaledFrame");
		strcat(str3,".bmp");
		cout<<str3;
		//imageup=imageup.sharpen(100); 
		imageup.save(str3);
		
		// imageup=imageup.sharpen(100); 	
	}
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Time in sec = %f",time_spent);
	//imageup.save("upscaled.bmp");
	getch();


}


