/*
   Filename: SDL2.cpp
   Description: SDL2 to paint  update   RGB buffer on screen
   Author:mater_lai@163.com.For more information,please visit:
   http://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-2-dont-put-everything-in-main for help. 
*/
#include "CSDL2.h"

void logSDLError(std::ostream  & os,const std::string & string)
{
    os<<string<<std::endl;
}

/*
 * These specializations serve to free the passed argument and also provide the
 * base cases for the recursive call above, eg. when args is only a single item
 * one of the specializations below will be called by
 * cleanup(std::forward<Args>(args)...), ending the recursion
 * We also make it safe to pass nullptrs to handle situations where we
 * don't want to bother finding out which values failed to load (and thus are null)
 * but rather just want to clean everything up and let cleanup sort it out
 */
template<class T>
void cleanup(T buffer)
{
  
}

template<>
void cleanup<SDL_Window*>(SDL_Window *win)
{
	if (!win){
		return;
	}
	SDL_DestroyWindow(win);
}
template<>
void cleanup<SDL_Renderer*>(SDL_Renderer *ren){
	if (!ren){
		return;
	}
	SDL_DestroyRenderer(ren);
}
template<>
void cleanup<SDL_Texture*>(SDL_Texture *tex){
	if (!tex){
		return;
	}
	SDL_DestroyTexture(tex);
}
template<>
void cleanup<SDL_Surface*>(SDL_Surface *surf){
	if (!surf){
		return;
	}
 	SDL_FreeSurface(surf);

}


 unsigned int SDL_GetPixSize(enum SDL_ColorSpace_Type type)
 {
   if(type==SDL_ColorSpace_RGB888 || type==SDL_ColorSpace_YUV444)  return 3;
   else if(type==SDL_ColorSpace_YUV422 || type==SDL_ColorSpace_YUV422_Planner)  return 2;
   else if(type==SDL_ColorSpace_RGBA) return 4;
   return 0;
  
 }



 SDL_Interface_Object * Create_SDL_Interface(const int frame_width,const int frame_height)
 {
     if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
     {
 	logSDLError(std::cout, "SDL_Init");
 	return NULL;;
     }
     SDL_Interface_Object * sdl_object=new SDL_Interface_Object ;
     ::memset(sdl_object,0,sizeof(SDL_Interface_Object));
     sdl_object->win= SDL_CreateWindow("RB_Capture", 100, 100, frame_width,
 	                  frame_height, SDL_WINDOW_SHOWN);
     if (sdl_object->win == NULL)
     {
 	logSDLError(std::cout, "CreateWindow");
 	SDL_Quit();
 	return NULL;
     }
     sdl_object->ren = SDL_CreateRenderer(sdl_object->win, -1,
                       SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
     if (sdl_object->ren == NULL)
     {
 	logSDLError(std::cout, "CreateRenderer");
 	cleanup(sdl_object->win);
 	SDL_Quit();
 	return NULL;
     }
     sdl_object->texture= SDL_CreateTexture(sdl_object->ren,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
 					   frame_width, frame_height);
     if(!sdl_object->texture)
     {
       logSDLError(std::cerr,"failed to create texture !");
       cleanup(sdl_object->win);
       SDL_Quit();
       return NULL;    
     }
      //alloc rennder window buffer  
     sdl_object->rect.w=frame_width;
     sdl_object->rect.h=frame_height;
     sdl_object->rect.x=0;
     sdl_object->rect.y=0;
    
     sdl_object->type=SDL_ColorSpace_RGBA;;
     sdl_object->window_buffer_size=frame_width*frame_height*SDL_GetPixSize(sdl_object->type); 
     sdl_object->window_buffer=new unsigned char [sdl_object->window_buffer_size];
     ::memset(sdl_object->window_buffer,0,sdl_object->window_buffer_size);
     
     return sdl_object;
 }

 void  Free_SDL_Interface(SDL_Interface_Object * sdl_object)
 {
      if(!sdl_object)  return ;
      cleanup(sdl_object->win); 
      cleanup(sdl_object->ren); 
      cleanup(sdl_object->texture); 
      if(sdl_object->window_buffer)  delete[]  sdl_object->window_buffer;
       
     
 }


 void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y)
 {
 	//Setup the destination rectangle to be at the position we want
 	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
 	//Query the texture to get its width and height to use
 	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
 	SDL_RenderCopy(ren, tex, NULL, &dst);
	
 }

 //only  copy three bytes 
 void ARGB_Pix_Copy(unsigned char * pargb_buffer,unsigned char * prgb_buffer,unsigned int copy_size)
 { 
    if(copy_size>SDL_GetPixSize(SDL_ColorSpace_RGB888))  return ;
    *pargb_buffer= *(prgb_buffer+2);
    *(pargb_buffer+1)=*(prgb_buffer+1);
    *(pargb_buffer+2)=*prgb_buffer; 
     
 }

 void SDL_Show(SDL_Interface_Object * sdl_object)
 {
       if(!sdl_object || ! sdl_object->window_buffer)  return ;
       /* SDL interprets each pixel as a 32-bit number, so our masks must depend
        on the endianness (byte order) of the machine */
       /*
       sdl_object->bmp=SDL_CreateRGBSurfaceFrom(sdl_object->window_buffer,
 					       sdl_object->rect.w,sdl_object->rect.h,
 					       8*SDL_GetPixSize(sdl_object->type),
 					       sdl_object->rect.w*SDL_GetPixSize(sdl_object->type),
 					       0x00FF0000,
                                                0x0000FF00,
                                                0x000000FF,
                                                0xFF000000); 
       if(!sdl_object->bmp)
       {
 	  logSDLError(std::cout,"Create 32 bits bmp from buffer failed ");
 	  return ;
 	  }
       */
       SDL_UpdateTexture(sdl_object->texture, NULL, sdl_object->window_buffer, 
                         sdl_object->rect.w *SDL_GetPixSize(sdl_object->type) );
      
       // copy texture to rennder 
           //renderTexture(sdl_object->texture,sdl_object->ren,0,0);
      SDL_RenderClear(sdl_object->ren);
      SDL_RenderCopy(sdl_object->ren, sdl_object->texture, NULL, NULL);
      SDL_RenderPresent(sdl_object->ren); 
      printf("OK");      
       
 }

void SDL_Show(SDL_Interface_Object * sdl_object,unsigned char *prgb888_left,
      unsigned char* prgb888_right,const int frame_width,const int frame_height,
	      const int frame_X_offset,unsigned char * stro_rgb_buffer,const int frame_Y_offset )
{
  if(!sdl_object || ! prgb888_left || !prgb888_right || ! stro_rgb_buffer)
  {
       logSDLError(std::cerr,"Invalid data source!");
       return ;
  }
  ::memset(sdl_object->window_buffer,0,sdl_object->window_buffer_size); 
  for( int index=0;index<frame_height;index++)
    for( int j=0;j<frame_width;j++)
     {
         unsigned char * prgba_start= sdl_object->window_buffer + index*sdl_object->rect.w*
	   SDL_GetPixSize(sdl_object->type)+j*SDL_GetPixSize(sdl_object->type);
         //copy left rgb buffer 
         ARGB_Pix_Copy(prgba_start,prgb888_left+index*frame_width*SDL_GetPixSize(
                  SDL_ColorSpace_RGB888)+ j*SDL_GetPixSize(SDL_ColorSpace_RGB888),3);
	 //copy right RGB  buffer 
         int right_x_offset=(frame_width+frame_X_offset)*SDL_GetPixSize(sdl_object->type);
         ARGB_Pix_Copy(prgba_start+right_x_offset,prgb888_right+index*frame_width*SDL_GetPixSize(                                  SDL_ColorSpace_RGB888)+j*SDL_GetPixSize(SDL_ColorSpace_RGB888),3);
         //copy 3D buffer
         unsigned int stro_top_y_offset= frame_height+frame_Y_offset;
         unsigned int stro_left_x_offset= ((sdl_object->rect.w-frame_height)/2+0x0F) &0xFFFFFFF0;
         unsigned char * prgba_stro_buffer= sdl_object->window_buffer+ stro_top_y_offset * 
         sdl_object->rect.w*SDL_GetPixSize(sdl_object->type)+stro_left_x_offset*
                 SDL_GetPixSize(sdl_object->type);
         ARGB_Pix_Copy(prgba_stro_buffer+index*sdl_object->rect.w*SDL_GetPixSize(sdl_object->type)
                    +j*SDL_GetPixSize(sdl_object->type),
                  stro_rgb_buffer+index*frame_width*SDL_GetPixSize( SDL_ColorSpace_RGB888)
                  +j*SDL_GetPixSize(SDL_ColorSpace_RGB888),3);
	 	 
     }
  SDL_Show(sdl_object); 
    
}
