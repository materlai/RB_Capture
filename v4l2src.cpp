/*
  Filename : v4l2src.cpp:
  Descriptions:based on V4L2 interface to capture video stream from webcam device(/dev/video* etc)
  Author:mater_lai@163.com
*/


#include "v4l2.h"
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<iostream>


#define  DEBUG_Log(Info,...)  fprintf(stderr,Info"\n",##__VA_ARGS__)
#define  Print_Log(Info,...)  fprintf(stdout,Info"\n",##__VA_ARGS__)  


V4L2_Capture* Alloc_Capture()
{
  V4L2_Capture* capture_object= new V4L2_Capture;
  if(!capture_object)
   {
     std::cerr<<"alloc webcam capture object failed !"<<std::endl;
     return NULL;
   }  
  std::memset(capture_object,0,sizeof(V4L2_Capture));
  return capture_object;  
}

V4L2_Capture * OpenWebCamDeviceFile(int device_id)
{
   const int max_device_name_length=1024;
   char device_name[max_device_name_length];  
   if(device_id<0)
   {
     std::cerr<<"invalid device id,please make sure device id >0 ! "<<std::endl;
     return NULL;
   }
   sprintf(device_name,"/dev/video%d",device_id);
   Print_Log("open webcam device:%s",device_name);
   const int  Camera_fd = open(device_name, O_RDWR /* required */ | O_NONBLOCK, 0);//打开设备
   V4L2_Capture* capture_object=NULL;
   if( (Camera_fd)<0 || !(capture_object=Alloc_Capture()) )
   {
     DEBUG_Log("open device name: %s failed!",device_name);
     return NULL;
   }
   capture_object->camera_fd=Camera_fd;
   return capture_object;
}

/* 
    query device properity 
*/
void GetWebCamProperity(V4L2_Capture* capture_object)
{
   if(!capture_object || capture_object->camera_fd<0)
   {
       DEBUG_Log("invalid device file for quety properity!");
       return ;      
   }  
   struct v4l2_capability cap;
   std::memset(&cap,0,sizeof(struct v4l2_capability));
   if(ioctl (capture_object->camera_fd, VIDIOC_QUERYCAP, &cap)<0)
   {
     DEBUG_Log("%p:query webcam device properity failed",capture_object);
      return ;
   }
  
   Print_Log("%p:camera capability as:",capture_object);
   Print_Log("%p:camera bus info:%s",capture_object,cap.bus_info);
   Print_Log("%p:Camera name : %s",capture_object,cap.card);
   Print_Log("%p:Camera kernel version:%d",capture_object,cap.version);
   Print_Log("%p:Camera driver info:%s",capture_object,cap.driver);
}

/*
  set wemcam properity and get ready to get webcam stream  
  @frame_width,framw_height;
*/
int SetWebcamProperity(V4L2_Capture* capture_object,int width,int height)
{
        struct v4l2_format fmt;
        std::memset(&fmt,0,sizeof(struct v4l2_format));
        fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = width;
        fmt.fmt.pix.height      = height;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
        capture_object->curr_frame_width=width;
        capture_object->curr_frame_height=height;  
        if(ioctl (capture_object->camera_fd, VIDIOC_S_FMT, &fmt)<0)
        {
	   DEBUG_Log("%p:set camera Capture format error! ",capture_object);
           return -1;
        }
        struct v4l2_requestbuffers req;
        std::memset(&req,0,sizeof(struct v4l2_requestbuffers));
        req.count               = 4;
        req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory              = V4L2_MEMORY_MMAP;
        if(ioctl (capture_object->camera_fd, VIDIOC_REQBUFS, &req)<0) 
        {
	  DEBUG_Log("%p:申请缓存失败!",capture_object);
	  return -1;
        }
        if (req.count < 2)
           std::cout<<"Insufficient buffer memory"<<std::endl;;
        Print_Log("%p:申请得到缓存数目＝%d",capture_object,req.count);
        Print_Log("%p:申请得到大小= %d",capture_object,req.memory);
        Print_Log("%p:申请得到缓存类型=%d",capture_object,req.type);
        
        /* request to alloc buffer */
        //int  buffer_count=req.count; //保存缓存的帧数
        capture_object->max_buffer_count=req.count;
        capture_object->pbuffer = (V4L2_Buffer*)calloc(req.count, sizeof ( V4L2_Buffer));
        unsigned int n_buffers=0;
        for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
        {
           struct v4l2_buffer buf;   //驱动中的一帧
           std::memset(&buf,0,sizeof(struct v4l2_buffer));
           buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
           buf.memory      = V4L2_MEMORY_MMAP;
           buf.index       = n_buffers;
           if (-1 == ioctl (capture_object->camera_fd, VIDIOC_QUERYBUF, &buf)) //映射用户空间
           {
	     DEBUG_Log("%p:VIDIOC_QUERYBUF error",capture_object);
	     break;
           }
           capture_object->pbuffer[n_buffers].pbuffer_length = buf.length;
           capture_object->pbuffer[n_buffers].pbuffer =
              mmap (NULL /* start anywhere */,    //通过mmap建立映射关系
               buf.length,
               PROT_READ | PROT_WRITE /* required */,
               MAP_SHARED /* recommended */,
               capture_object->camera_fd, buf.m.offset);
           if (MAP_FAILED == capture_object->pbuffer[n_buffers].pbuffer)
           {
	     DEBUG_Log("%p:mmap failed\n",capture_object);
	      break;
           }
           std::memset(capture_object->pbuffer[n_buffers].pbuffer,0,
                 capture_object->pbuffer[n_buffers].pbuffer_length);
        }
       if(n_buffers<req.count)
       {
	 DEBUG_Log("%p: error in request v4l2 buffer!",capture_object);
                return -1;
       }

       unsigned int index=0;
       for ( index = 0; index < n_buffers; ++index)
       {
           struct v4l2_buffer buf;
           std::memset (&buf,0,sizeof(struct v4l2_buffer));
           buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
           buf.memory      = V4L2_MEMORY_MMAP;
           buf.index       = index;
           if (-1 == ioctl (capture_object->camera_fd, VIDIOC_QBUF, &buf))//申请到的缓冲进入列队
           {
	     DEBUG_Log("%p:VIDIOC_QBUF failed with index=%d",capture_object,index);
               break;
           }
        }
        
        if(index<n_buffers)
        {
	  DEBUG_Log(" %p:error in  v4l2 buffer queue! ",capture_object);
           return -2;
        }
        Print_Log("%p:WebCam set properity is finsihed !",capture_object);
        return 0;
}

int WebCamCaptureStart(V4L2_Capture* capture_object)
{
        if(capture_object->camera_fd<0) 
	{
	  DEBUG_Log("%p:Invalid device name to start capture !",capture_object);
               return -1;
        }
        enum v4l2_buf_type type;
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == ioctl (capture_object->camera_fd, VIDIOC_STREAMON, &type)) //开始捕捉图像数据
        {
	  DEBUG_Log("%p:Start VIDIOC_STREAMON failed",capture_object);
          return -1;
        }
        Print_Log("%p:WebCam is ready to go  !",capture_object);
        return 0;
}

int QueueFrameFromCapture(V4L2_Capture*capture_object)
{
           fd_set fds;
           struct timeval tv;

           FD_ZERO (&fds);//将指定的文件描述符集清空
           FD_SET (capture_object->camera_fd, &fds);//在文件描述符集合中增加一个新的文件描述符
           /* Timeout. */
           tv.tv_sec = 1;
           tv.tv_usec = 0;
           int r = ::select(capture_object->camera_fd+1, &fds, NULL, NULL, &tv);
           if (-1 == r)
           {
             if (EINTR == errno)
                   return -1 ;
             std::cerr<<"select err"<<std::endl;
             return -2;
           }   
           else if(r==0)  return -1;
           struct v4l2_buffer buf;
           std::memset(&buf,0,sizeof(struct v4l2_buffer));
           buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
           buf.memory = V4L2_MEMORY_MMAP;
           if(ioctl(capture_object->camera_fd, VIDIOC_DQBUF, &buf)<0) //出列采集的帧缓中
           {
	     DEBUG_Log("%p:dequeue frame from webcam failed ! ",capture_object);
             return -2;
           }
           assert (buf.index < (unsigned int)capture_object->max_buffer_count);
           return buf.index;
           
}

int QueueFrameFromDualCapture(V4L2_Capture*capture_object_left,
                              V4L2_Capture* capture_object_right)
{
           fd_set fds;
           struct timeval tv;
           FD_ZERO (&fds);//将指定的文件描述符集清空
           FD_SET (capture_object_left->camera_fd, &fds);//在文件描述符集合中增加一个新的文件描述符
           FD_SET (capture_object_right->camera_fd, &fds);//在文件描述符集合中增加一个新的文件描述符 
           int max_fds= capture_object_left->camera_fd>capture_object_right->camera_fd?
	                capture_object_left->camera_fd:capture_object_right->camera_fd;
           /* Timeout. */
           tv.tv_sec = 1;
           tv.tv_usec = 0;
           int available_capture_mask=0;  
           int r = ::select(max_fds+1, &fds, NULL, NULL, &tv);
           if (-1 == r)
           {
             if (EINTR == errno)
                   return -1 ;
             std::cerr<<"select err"<<std::endl;
             return -2;
           }   
           else if(r==0)  return available_capture_mask;
	   if(FD_ISSET(capture_object_left->camera_fd,&fds))
	     available_capture_mask |=0x01;
           if(FD_ISSET(capture_object_right->camera_fd,&fds))
	     available_capture_mask |=0x02;
           return available_capture_mask;   
	   
}

int DequeueBufferFromCapture(V4L2_Capture * capture_object)
{
           struct v4l2_buffer buf;
           std::memset(&buf,0,sizeof(struct v4l2_buffer));
           buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
           buf.memory = V4L2_MEMORY_MMAP;
           if(ioctl(capture_object->camera_fd, VIDIOC_DQBUF, &buf)<0) //出列采集的帧缓中
           {
	     DEBUG_Log("%p:dequeue frame from webcam failed ! ",capture_object);
             return -1;
           }
           assert (buf.index>=0 && buf.index < (unsigned int)capture_object->max_buffer_count);
           return buf.index;
}

int EnqueueBufferForCapture(V4L2_Capture* capture_object,int buffer_index)
{
          struct v4l2_buffer buf;
           std::memset(&buf,0,sizeof(struct v4l2_buffer));

           buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
           buf.memory = V4L2_MEMORY_MMAP;
           buf.index=buffer_index;
           if(ioctl (capture_object->camera_fd, VIDIOC_QBUF, &buf)<0)
	   {
	     DEBUG_Log("%p:Enqueue buffer index =%d failed !",capture_object,buffer_index);
             return -1;
           }   
           return 0;
}


void  CloseWebCam(V4L2_Capture * capture_object)
{
  if(!capture_object || capture_object->camera_fd<0)
    return ;
  close(capture_object->camera_fd);
  
 }



