/*
   Filename:main.cpp
   Description:RGB/Blue 3D capture program from two webcam
   Author: mater_lai@163.com
*/

#include "v4l2.h"
#include "colorspace.h"
#include "CSDL2.h"
//#define SDL2_VERSION_RELEASE 1



int WebCamCaptureSetting(V4L2_Capture * capture_object ,const int width,const int height)
{
       
      GetWebCamProperity(capture_object);
      
      SetWebcamProperity(capture_object,width,height);
      
      WebCamCaptureStart(capture_object);
      return 0;
}


int main(int argc,char ** argv)
{
    printf("This is a Red/Blue 3D  capture  program from two webcam" 
           "and is writen by mater_lai@163.com ! \n ");
     const int frame_width=640, frame_height=480;
     //step 1 : open webcam 
     V4L2_Capture* capture_object_left=OpenWebCamDeviceFile(0);
     V4L2_Capture* capture_object_right=OpenWebCamDeviceFile(1);
     if(!capture_object_left ||  capture_object_left->camera_fd<0 || !capture_object_right
	|| capture_object_right->camera_fd<0)
           return -1;
      //step 2 :settting capture object and make webcam be ready 
      WebCamCaptureSetting(capture_object_left,frame_width,frame_height);
      WebCamCaptureSetting(capture_object_right,frame_width,frame_height);   
      //step 3 : make SDL init 
      const int SDL_window_offset_X=10,SDL_window_offset_Y=10;
     
      const int SDL_window_width=frame_width*2+SDL_window_offset_X;
      const int SDL_window_height=frame_height*2+SDL_window_offset_Y;
      SDL_Interface_Object * sdl_object=Create_SDL_Interface(SDL_window_width,SDL_window_height);
      assert(sdl_object);
      
      Capture_Window_Buffer * window_buffer_left=Alloc_Capture_Window_Buffer(									 frame_width,frame_height);
      Capture_Window_Buffer * window_buffer_right=Alloc_Capture_Window_Buffer(								            frame_width,frame_height);  
      Capture_Window_Buffer * stro_window_buffer=Alloc_Capture_Window_Buffer(							      	      frame_width,frame_height);  
      
      int stro_x_offset=0,stro_y_offset=0;const int stro_step_offset=4;             
      //step 4 : loop capture frame from left and  right webcam and show it
      SDL_Event sdl_event;
      bool event_loop=true;
      while(event_loop)
      {
	 int  available_capture_mask=QueueFrameFromDualCapture(capture_object_left,
							      capture_object_right);
         if(available_capture_mask==-2)  break;
         else if(available_capture_mask<=0)  continue; //time out or be interrupted
         if(available_capture_mask&0x01) //left webcam is ready 
	 { 
	      int buffer_index=DequeueBufferFromCapture(capture_object_left);
	      ::memcpy(window_buffer_left->yuv422_object.pbuffer ,
	               capture_object_left->pbuffer[buffer_index].pbuffer,  
	               capture_object_left->pbuffer[buffer_index].pbuffer_length);
              EnqueueBufferForCapture(capture_object_left,buffer_index);
         }
	 if(available_capture_mask&0x02)
	 {
              int buffer_index=DequeueBufferFromCapture(capture_object_right);
              ::memcpy(window_buffer_right->yuv422_object.pbuffer,
	       capture_object_right->pbuffer[buffer_index].pbuffer,  
	       capture_object_right->pbuffer[buffer_index].pbuffer_length);
              EnqueueBufferForCapture(capture_object_right,buffer_index);
         }
         	 	                           
	 Capture_Window_Buffer_YUV422_Clear_Channel(window_buffer_left,frame_width,frame_height,
	 	                   (enum Channel_Mask) (RGB_Channel_G_Mask|RGB_Channel_B_Mask));  
         Capture_Window_Buffer_YUV422_Clear_Channel(window_buffer_right,frame_width,frame_height,
	 			     RGB_Channel_R_Mask);
         //merge R channel from left and G&B channel from right to stro buffer 
	 Merge_RGB888_Buffer(window_buffer_left->rgb888_object.pbuffer,
			     window_buffer_right->rgb888_object.pbuffer,
			     stro_window_buffer->rgb888_object.pbuffer,
                             stro_x_offset,stro_y_offset,
                             frame_width,frame_height);
         
	  //Convert_RGB888_To_YUV422(stro_window_buffer->rgb888_object.pbuffer,
	  //			  stro_window_buffer->yuv422_object.pbuffer,
	  //			  frame_width,frame_height);    
         //combine two buffer and show it
	 
         SDL_Show(sdl_object,window_buffer_left->rgb888_object.pbuffer,
	                   window_buffer_right->rgb888_object.pbuffer,
		  frame_width,frame_height, SDL_window_offset_X,
		  stro_window_buffer->rgb888_object.pbuffer,SDL_window_offset_Y);

      	 //handle SDL event 
         while(SDL_PollEvent(&sdl_event))
      	 { 
      	  switch(sdl_event.type)
      	  {
      	    case SDL_QUIT:
      	    {
	         std::cout<<"user request to exit! "<<std::endl;
		 event_loop=false;
                  break;
            }
	   case SDL_KEYDOWN:
	   {  
	      SDL_KeyboardEvent * key_event = &sdl_event.key;
              int key_buffer =key_event->keysym.sym;
	      if(key_buffer==SDLK_UP)
	  	 stro_y_offset-=stro_step_offset;
	      else if(key_buffer==SDLK_DOWN)
                  stro_y_offset+=stro_step_offset;
              else if(key_buffer==SDLK_LEFT)
		 stro_x_offset-=stro_step_offset;
	      else if(key_buffer==SDLK_RIGHT)
		stro_x_offset+=stro_step_offset;
	      // offset limit handle  
             Stro_Offset_Set_limit(stro_x_offset,-frame_width/2,frame_width/2);
             Stro_Offset_Set_limit(stro_y_offset,-frame_height/2,frame_height/2);         
           }
	
            default:    break;
           } 
        }//end while()              
    }
    CloseWebCam(capture_object_left);
    CloseWebCam(capture_object_right);
    Free_Capture_Window_Buffer(window_buffer_left);
    Free_Capture_Window_Buffer(window_buffer_right);
    return 0 ;
 
}
