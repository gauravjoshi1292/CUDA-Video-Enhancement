#include"CImg.h"
#include<conio.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <deque>
#include <stack>
#include <bitset>
#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>


using namespace cimg_library;

#define T 22
#define del 4
#define maxImages 5
int w1=0,w2=0,w3=0,w4=0,i1,i2,i3,i4;
float uc,ue;
float a=10,b=1;

CImg<unsigned char> imageup(1024,1024,1,3);

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
{
	float u;
	uc= w1*(mod((I11(i,j,k) - I11(i+1,j+1,k))) +
            mod(I22(i,j,k) - I22(i+1,j+1,k))) +
		w2*(mod(I11(i,j,k) - I11(i+1,j-1,k)) +
			mod(I22(i,j,k) - I22(i+1,j-1,k)))+
		w3*(mod(I11(i,j,k) - I11(i-1,j+1,k)) +
			mod(I22(i,j,k) - I22(i-1,j+1,k))) +
		w4*(mod(I11(i,j,k) - I11(i-1,j-1,k)) +
			mod(I22(i,j,k) - I22(i-1,j-1,k)));
	ue=-1*(mod(I11(i,j,k))+mod(I22(i,j,k)));
	u=a*uc-b*ue;
	return u;
}

void main(){
	char input[20];
	char upscaled[20];
	for(int q=1;q<maxImages;q++){
		strcpy(input,"image");
		char c[5];
		itoa(q,c,10);
		strcat(input,c);
		strcat(input,".jpg");
		CImg<unsigned char> image(input);
		static int count1=0,count2=0;
	//CImg<unsigned char> image("lena.jpg");
	float min,secd1,secd2,u,uplus,uminus;
	int save,flag;

//Initializing the new upscaled image
	for(int j=0;j<512;j++)
    {
	  for(int i=0;i<512;i++)
	  {
		  for(int k=0;k<3;k++)
		  {
			  imageup(2*i,2*j,k)=image(i,j,k);

		  }
	  }
	}
	//imageup.save("initialized.bmp");

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

	//imageup.save("afterfillingboundary.bmp");

	//FAST INTERPOLATION
	//STAGE 1 - FILLING 2i+1,2j+1
	 for(int j=1;j<510;j++)
    {
	  for(int i=1;i<510;i++)
	  {
		  for(int k=0;k<3;k++)
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
	 //imageup.save("stage1.bmp");

	 //Iterating for the N times
	for(int l=30;l<1;l++)
	{
	 for(int j=1;j<510;j++)
    {
	  for(int i=1;i<510;i++)
	  {
		  for(int k=0;k<3;k++)
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
	//imageup.save("stage1_afterIteration.bmp");

	//STAGE 2 - FILLING the other two
	//filling 2i,2j+1
	 for(int j=1;j<510;j++)
    {
	  for(int i=1;i<510;i++)
	  {
		  for(int k=0;k<3;k++)
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
	 //imageup.save("stage2.bmp");

	 //Iterating for the N times
	for(int l=30;l<1;l++)
	{
	 for(int j=1;j<510;j++)
    {
	  for(int i=1;i<510;i++)
	  {
		  for(int k=0;k<3;k++)
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
	//imageup.save("stage2_afterIteration.bmp");

	//STAGE 3 - FILLING the last two
	//filling 2i+1,2j
	 for(int j=2;j<510;j++)
    {
	  for(int i=2;i<510;i++)
	  {
		  for(int k=0;k<3;k++)
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
	 //imageup.save("stage3.bmp");

	 //Iterating for the N times
	for(int l=30;l<1;l++)
	{
	 for(int j=2;j<510;j++)
    {
	  for(int i=2;i<510;i++)
	  {
		  for(int k=0;k<3;k++)
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
	//imageup.save("stage3_afterIteration.bmp");

	//correcting absurd pixel values
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

	//STAGE 4
	//now applying iterative correction to the enlarged image

	 //Iterating for the N times on 2i+1,2j+1
	for(int l=30;l<1;l++)
	{
	 for(int j=1;j<510;j++)
    {
	  for(int i=1;i<510;i++)
	  {
		  for(int k=0;k<3;k++)
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
	 //Iterating for the N times on 2i,2j+1
	for(int l=30;l<1;l++)
	{
	 for(int j=1;j<510;j++)
    {
	  for(int i=1;i<510;i++)
	  {
		  for(int k=0;k<3;k++)
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
	for(int l=30;l<1;l++)
	{
	 for(int j=2;j<510;j++)
    {
	  for(int i=2;i<510;i++)
	  {
		  for(int k=0;k<3;k++)
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

	strcpy(upscaled,"upscaled");
	char s[5];
	itoa(q,s,10);
	strcat(upscaled,s);
	strcat(upscaled,".bmp");
	imageup.save(upscaled);
	}
	getch();
}
