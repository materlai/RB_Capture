/*
   Filename: colorspace.h
   Description: colorspace.cpp head file 
   Author:mater_lai@163.com
*/
enum ColorSpace_Type 
{
  ColorSpace_Type_RGB888=0,
  ColorSpace_Type_YUV444=1,
  ColorSpace_Type_YUV422=2,
};

enum  Channel_Mask
  {
    RGB_Channel_R_Mask=0x01,
    RGB_Channel_G_Mask=0x02,
    RGB_Channel_B_Mask=0x04,
    RGB_Channel_All_Mask= RGB_Channel_R_Mask| RGB_Channel_G_Mask| RGB_Channel_B_Mask
  };


typedef struct 
{ 
  unsigned char * pbuffer;
  enum ColorSpace_Type type; 
  unsigned long pbuffer_length;

}ColorSpace_Buffer_Object;

typedef struct 
{
  ColorSpace_Buffer_Object yuv422_object;
  ColorSpace_Buffer_Object rgb888_object;
}Capture_Window_Buffer;

unsigned int GetPixBufferSize(enum ColorSpace_Type type);
Capture_Window_Buffer * Alloc_Capture_Window_Buffer(unsigned int width,unsigned int height);

void Free_Capture_Window_Buffer(Capture_Window_Buffer * & window_object);
void Convert_YUV422_YUV444(unsigned char * pyuv422,unsigned char * pyuv444,
			   const int frame_width,const int frame_height);
void Convert_YUV444_YUV422(unsigned char * pyuv422,unsigned char * pyuv444,
			   const int frame_width,const int frame_height);

void Convert_RGB_To_YUV444(unsigned char *prgb,unsigned char * pyuv444,
			   const int frame_width,const int frame_height);

void Convert_YUV444_To_RGB(unsigned char *prgb,unsigned char * pyuv444,
			   const unsigned int frame_width,const unsigned int frame_height);

void Convert_YUV422_To_RGB888(unsigned char * pyuv422,unsigned char * prgb,
			      const int width,const int height);
void Convert_RGB888_To_YUV422(unsigned char * prgb,unsigned char * pyuv422,
			      const int width,const int height);
void RGB_Channel_Delete(unsigned char * prgb_buffer,unsigned int width,
			unsigned int height,enum  Channel_Mask clear_channel_mask);
void Capture_Window_Buffer_YUV422_Clear_Channel(Capture_Window_Buffer* window_buffer,
						unsigned int frame_width,unsigned int frame_height,
						enum Channel_Mask clear_channel_mask);
void Merge_RGB888_Buffer(unsigned char * prgb_buffer_left,unsigned char *prgb_buffer_right,
			 unsigned char * prgb_buffer_des, 
                         int offset_x,
			 int offset_y,
                         unsigned int frame_width,unsigned int frame_height);
void Stro_Offset_Set_limit(int &offset,const int min_offset,const int max_offset);
