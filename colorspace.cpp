/*
  Filename: colorsapce.cpp
  Description: Color space convert : from YUV to RGB and Reverse 
  Author: mater_lai@163.com
*/
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<assert.h>
#include<iostream>
#include "colorspace.h"



unsigned int GetPixBufferSize(enum ColorSpace_Type type)
{
  if(type==ColorSpace_Type_RGB888 || type== ColorSpace_Type_YUV444)   return 3;
  else if(type==ColorSpace_Type_YUV422)   return 2;
  return 0;
  
}

void Stro_Offset_Set_limit(int &offset,const int min_offset,const int max_offset)
{
  if(offset<min_offset)   offset=min_offset;
  if(offset>max_offset)   offset=max_offset; 
  
}

 Capture_Window_Buffer * Alloc_Capture_Window_Buffer(unsigned int width,unsigned int height)
 {
     Capture_Window_Buffer * window_object =new Capture_Window_Buffer ;
     //YUV422 object
    window_object->yuv422_object.type= ColorSpace_Type_YUV422;
    window_object->yuv422_object.pbuffer_length=GetPixBufferSize(
                              ColorSpace_Type_YUV422)*width*height;
    window_object->yuv422_object.pbuffer=new unsigned char[
   			window_object->yuv422_object.pbuffer_length];
    ::memset(window_object->yuv422_object.pbuffer,0,window_object->yuv422_object.pbuffer_length);
    //RGB888 obejct 
    window_object->rgb888_object.type=ColorSpace_Type_RGB888;
    window_object->rgb888_object.pbuffer_length=GetPixBufferSize(
                    ColorSpace_Type_RGB888)*width*height;
    window_object->rgb888_object.pbuffer =new unsigned char[  window_object->rgb888_object.pbuffer_length];
    ::memset(window_object->rgb888_object.pbuffer,0,window_object->rgb888_object.pbuffer_length);
    return window_object;
     
       
}

void Free_Capture_Window_Buffer(Capture_Window_Buffer * & window_object)
{
    if(! window_object )  return ; 
    if(window_object->yuv422_object.pbuffer)  delete[] window_object->yuv422_object.pbuffer;
    if(window_object->rgb888_object.pbuffer)  delete [] window_object->rgb888_object.pbuffer;
    delete window_object;
    window_object=NULL;
       
}


void Convert_YUV422_YUV444(unsigned char * pyuv422,unsigned char * pyuv444,
               const int frame_width,const int frame_height)
{
     assert(pyuv422 && pyuv444);
     for( int index=0;index<frame_height;index++)
       for( int j=0;j<frame_width;j+=2)
	 {
	    unsigned char * pyuv422_start =pyuv422+ index*frame_width*2 + j*2;
            unsigned char * pyuv444_start =pyuv444+index*frame_width*3+j*3;
            //copy Y 
            *pyuv444_start=*pyuv422_start;
            *(pyuv444_start+3)=*(pyuv422_start+2);
            //Copy U 
            *(pyuv444_start+1)=*(pyuv422_start+1);
            *(pyuv444_start+4)=*(pyuv422_start+1);
            //copy V 
            *(pyuv444_start+2)=*(pyuv422_start+3);
            *(pyuv444_start+5)=*(pyuv422_start+3);                
         }    	                   
}

void Convert_YUV444_YUV422(unsigned char * pyuv422,unsigned char * pyuv444,
               const int frame_width,const int frame_height)
{
     assert(pyuv422 && pyuv444);
     for( int index=0;index<frame_height;index++)
       for( int j=0;j<frame_width;j+=2)
	 {
	    unsigned char* pyuv422_start =pyuv422+ index*frame_width*2 + j*2;
            unsigned char * pyuv444_start =pyuv444+index*frame_width*3+j*3;
            //copy Y 
            *(pyuv422_start)=*(pyuv444_start);
            *(pyuv422_start+2)=*(pyuv444_start+3);
            //Copy U 
            *(pyuv422_start+1)=*(pyuv444_start+1);
            //copy V 
	       *(pyuv422_start+3)=*(pyuv444_start+5);
         }
	 
}

unsigned char  color_limit(int color_data )
{  
  if(color_data<0)  return 0;
  if(color_data>255)  return 255;
  return (unsigned char) color_data;
  
}

// Convert  RGB to YUV444 
void Convert_RGB_To_YUV444(unsigned char *prgb,unsigned char * pyuv444,
			   const int frame_width,const int frame_height)
{
   
      assert(prgb && pyuv444);
      for( int index=0;index<frame_height;index++)
	for( int j=0;j<frame_width;j++)
	  {
	     unsigned char * prgb_start=prgb+index*frame_width*3+j*3; 
             unsigned char r= *prgb_start;
		  
             unsigned char g=*(prgb_start+1);
             unsigned char b=*(prgb_start+2);
             /*
                Y = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16
                U = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128
                V = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128
             
	     */
	     int color_Y= ( (  66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
             int color_U=( ( -38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
             int color_V=( ( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128;
             
             unsigned char * pyuv444_start =pyuv444+index*frame_width*3+j*3;
	     *pyuv444_start = color_limit(color_Y);
	     *(pyuv444_start+1)=color_limit(color_U);
	     *(pyuv444_start+2)=color_limit(color_V);           
         } 
    
}

// Convert YUV444 To RGB 
void Convert_YUV444_To_RGB(unsigned char *prgb,unsigned char * pyuv444,
			   const unsigned int frame_width,const unsigned int frame_height)
{
      assert(prgb && pyuv444);
     for(unsigned int index=0;index<frame_height;index++)
       for(unsigned int j=0;j<frame_width;j++)
	 {  
	   unsigned char * pyuv444_start =pyuv444+index*frame_width*3+j*3;
           unsigned char color_Y=*pyuv444_start;
           unsigned char color_U=*(pyuv444_start+1);
           unsigned char color_V=*(pyuv444_start+2);
	   int color_coefficients_C = color_Y - 16;
	   int  color_coefficients_D = color_U - 128;
	   int color_coefficients_E = color_V - 128  ;
           unsigned char * prgb_start=prgb+index*frame_width*3+j*3;
	    *prgb_start =    color_limit (  (298 * color_coefficients_C + 
                                                 409 *  color_coefficients_E + 128) >> 8 );
            *(prgb_start+1) =color_limit (  ( 298 * color_coefficients_C - 
                             100 * color_coefficients_D - 208 * color_coefficients_E + 128) >> 8 );
	    *(prgb_start+2) = color_limit( ( 298 *color_coefficients_C + 
                              516 * color_coefficients_D + 128) >> 8 ); 
     }
}

void convert_YUV444_RGB888_Unit(unsigned char y,unsigned char u,unsigned char v,
			   unsigned char &R,unsigned char &G,unsigned char &B)
{
           int color_coefficients_C = y - 16;
	   int  color_coefficients_D =u - 128;
	   int color_coefficients_E = v - 128  ;
	   
       	   R =  color_limit (  (298 * color_coefficients_C + 
                                     409 *  color_coefficients_E + 128) >> 8 );
           G =color_limit (  ( 298 * color_coefficients_C - 
                             100 * color_coefficients_D - 208 * color_coefficients_E + 128) >> 8 );
	   B = color_limit( ( 298 *color_coefficients_C + 
	   516 * color_coefficients_D + 128) >> 8 );
	   	   	    

}

void convert_RGB888_YUV444_Unit(unsigned char R,unsigned char G,unsigned char B,
			   unsigned char & Y,unsigned char &U,unsigned char &V)
{
   
             int color_Y= ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16;
             int color_U=( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
             int color_V=( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128;
             Y=(unsigned char) color_Y;
             U=(unsigned char) color_U;
     	     V=(unsigned char) color_V;
            
}

//convert YUV422 to RGB888 
void Convert_YUV422_To_RGB888(unsigned char * pyuv422,unsigned char * prgb,
			      const int width,const int height)
{
     assert(pyuv422 && prgb);      
     for( int index=0;index<height;index++)
      for( int j=0;j<width;j+=2)
       {    
	  unsigned char * prgb_start=prgb+index*width*3+j*3;
          unsigned char * pyuv422_start=pyuv422+index*width*2+j*2;
          convert_YUV444_RGB888_Unit(*pyuv422_start,*(pyuv422_start+1),*(pyuv422_start+3),
				     *prgb_start,*(prgb_start+1),*(prgb_start+2));
          convert_YUV444_RGB888_Unit(*(pyuv422_start+2),*(pyuv422_start+1),*(pyuv422_start+3),
				     *(prgb_start+3),*(prgb_start+4),*(prgb_start+5)); 
       }     
}

//convert from RGB888 to YUV422 
void Convert_RGB888_To_YUV422(unsigned char * prgb,unsigned char * pyuv422,
			      const int width,const int height)
{
     assert(pyuv422 && prgb);      
     for( int index=0;index<height;index++)
      for( int j=0;j<width;j+=2)
       {    
	  unsigned char * prgb_start=prgb+index*width*3+j*3;
          unsigned char * pyuv422_start=pyuv422+index*width*2+j*2;
          unsigned char U=0,V=0;
          convert_RGB888_YUV444_Unit(*prgb_start,*(prgb_start+1),*(prgb_start+2),
				     *pyuv422_start,*(pyuv422_start+1),V);
          convert_RGB888_YUV444_Unit(*(prgb_start+3),*(prgb_start+4),*(prgb_start+5),
				     *(pyuv422_start+2),U,*(pyuv422_start+3));
           
       }
        
}


void Merge_RGB888_Buffer(unsigned char * prgb_buffer_left,unsigned char *prgb_buffer_right,
			 unsigned char * prgb_buffer_des, 
                         int offset_x,int offset_y,
                         unsigned int frame_width,unsigned int frame_height)
{
   /*
      merge channel R from left and channel G&B from right to des buffer
   */
   if(!prgb_buffer_left || ! prgb_buffer_right || !prgb_buffer_des)   return ;
   ::memcpy(prgb_buffer_des,prgb_buffer_right,frame_width*frame_height*3); 
   int copy_top_x=(offset_x>0)?offset_x:0;
   int copy_top_y=(offset_y>0)?offset_y:0;
   int copy_bottom_x= (offset_x>0) ?frame_width:(frame_width+offset_x);
      int copy_bottom_y=(offset_y>0)?frame_height:(frame_height+offset_y);
      for( int index=copy_top_y;index<copy_bottom_y;index++)
     for(int  j=copy_top_x;j<copy_bottom_x;j++)
       {
	  unsigned long rgb_offset= index*frame_width*3+j*3;
          unsigned long left_offset = (index-offset_y)*frame_width*3+(j-offset_x)*3;
          *(prgb_buffer_des+rgb_offset)=*(prgb_buffer_left+left_offset);
          
	   }   
}

/* 
      RGB channel implemention
      channel bit mask :    mask&0x01   : red channel 
                            mask&0x02   : green channel
                            mask&ox04   : blue channel  
  */
void RGB_Channel_Delete(unsigned char * prgb_buffer,unsigned int width,
			  unsigned int height,enum  Channel_Mask clear_channel_mask)
{
    if(!prgb_buffer || clear_channel_mask<=0 || clear_channel_mask>=RGB_Channel_All_Mask) 
      return ;
    for(unsigned int index=0;index<height;index++)
      for(unsigned int j=0;j<width;j++)
	{
   	      unsigned char * prgb_start=prgb_buffer+ index*width*3+j*3;
          if(clear_channel_mask & RGB_Channel_R_Mask)  *prgb_start=255;
          if(clear_channel_mask & RGB_Channel_G_Mask)  *(prgb_start+1)=255;
	      if(clear_channel_mask & RGB_Channel_B_Mask)  *(prgb_start+2)=255;            
    }
                
}

void Capture_Window_Buffer_YUV422_Clear_Channel(Capture_Window_Buffer* window_buffer,
						unsigned int frame_width,unsigned int frame_height,
                enum  Channel_Mask clear_channel_mask)
{
    /* 
              1)color space convert from YUV422 to RGB888
              2) clear green & blue channel
              3) color space convert from rgb888 to yuv422
     */
  if(!window_buffer || !window_buffer->yuv422_object.pbuffer || 
     !window_buffer->rgb888_object.pbuffer)  return ;
  
     Convert_YUV422_To_RGB888(window_buffer->yuv422_object.pbuffer,
                window_buffer->rgb888_object.pbuffer,frame_width,frame_height);
     RGB_Channel_Delete(window_buffer->rgb888_object.pbuffer,frame_width,frame_height, clear_channel_mask);  
     //Convert_RGB888_To_YUV422(window_buffer->rgb888_object.pbuffer,
     //				  window_buffer->yuv422_object.pbuffer,
     //		     frame_width,frame_height) ; 
	
	 
     
}
