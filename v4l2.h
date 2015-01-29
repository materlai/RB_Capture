/*
     Filename: v4l2.h
     Description: v4l2.cpp head file
     Author:mater_lai@163.com
*/



#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct 
{
  void * pbuffer;
  int pbuffer_length;
 
}V4L2_Buffer;

typedef struct v4l2_capture
{
 int camera_fd; //webcam device fd 
 int curr_frame_width; //frame width
 int curr_frame_height;//frame height;  
 int max_buffer_count;
 V4L2_Buffer*pbuffer;
 int v4l2_buffer_index;    
}V4L2_Capture;

V4L2_Capture* Alloc_Capture();
V4L2_Capture * OpenWebCamDeviceFile(int device_id);

void GetWebCamProperity(V4L2_Capture* capture_object);

int SetWebcamProperity(V4L2_Capture* capture_object,int width,int height);

int WebCamCaptureStart(V4L2_Capture* capture_object);

int QueueFrameFromCapture(V4L2_Capture*capture_object);
int QueueFrameFromDualCapture(V4L2_Capture*capture_object_left,
                              V4L2_Capture* capture_obejct_right);

int DequeueBufferFromCapture(V4L2_Capture * capture_object);
int EnqueueBufferForCapture(V4L2_Capture* capture_object,int buffer_index);
void CloseWebCam(V4L2_Capture * capture_object);
