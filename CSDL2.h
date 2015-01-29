/*
    Filename : CSDL2.h 
    Description:SDL2 version and show iamge on window
    Author : mater_lai@163.com
*/

#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<iostream>
#include<SDL2/SDL.h>

enum SDL_ColorSpace_Type
{
  SDL_ColorSpace_RGB888=0,
  SDL_ColorSpace_YUV422,
  SDL_ColorSpace_YUV422_Planner,
  SDL_ColorSpace_YUV444,
  SDL_ColorSpace_RGBA,
};

typedef struct 
{
     SDL_Window *win;
     SDL_Renderer *ren;
     //SDL_Surface *bmp;
     SDL_Texture *texture;
     //SDL frame buffer 
     SDL_Rect        rect;
     enum SDL_ColorSpace_Type type;
     unsigned int window_buffer_size;
     unsigned char *window_buffer;
     
}SDL_Interface_Object;

SDL_Interface_Object* Create_SDL_Interface(const int frame_width,const int frame_height);
void  Fre_SDL_Interface(SDL_Interface_Object* sdl_object);
void SDL_Show(SDL_Interface_Object * sdl_object);
void SDL_Show(SDL_Interface_Object * sdl_object,unsigned char *pyuv422_left,
      unsigned char* pyuv422_right,const int frame_width,const int frame_height,
			  const int frame_X_offset,unsigned char * stro_buffer,const int frame_Y_offset);


