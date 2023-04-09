# Yolov5 inference - detect labels and set GPIO
To add a network in **Tappas** that is not already supported, I needed to implement a new postprocess. Thanks to the *hailofilter* element, there is only need to provide the .so (compiled shared object bianry) that applies the filter.
<br>
<br>

## **Downloading the libraries**
To compile the postprocess, we will need to download two libraries: *rapidjson* and *pigpio*.

To install *rapidjson* I ran these commands:
```shell
hailo@raspberrypi:/local/workspace/tappas$ git clone https://github.com/Tencent/rapidjson
hailo@raspberrypi:/local/workspace/tappas$ cd rapidjson/include/
hailo@raspberrypi:/local/workspace/tappas/rapidjson/include$ mv rapidjson/ $TAPPAS_WORKSPACE/core/hailo/libs/postprocesses/
```
This will fix the dependencies for some source files. The rapidjson library provided in *$TAPPAS_WORKSPACE/core/open_source/rapidjson* gives errors when compiling the .so file.

To install *pigpio* I ran these commands:
```shell
hailo@raspberrypi:/local/workspace/tappas$ wget https://github.com/joan2937/pigpio/archive/master.zip
hailo@raspberrypi:/local/workspace/tappas$ unzip master.zip
hailo@raspberrypi:/local/workspace/tappas$ cd pigpio-master
hailo@raspberrypi:/local/workspace/tappas/pigpio-master$ make
hailo@raspberrypi:/local/workspace/tappas/pigpio-master$ sudo make install
```
(source: https://abyz.me.uk/rpi/pigpio/download.html)

This will be necessary to make use of the **RaspberryPi GPIO**.
To run this file it was necessary to add these lines in the *run_tappas_docker.sh* script, to have access to GPIO files inside the docker container:
```shell
function prepare_docker_args_ubuntu() {
    DOCKER_ARGS="--privileged --net=host \
        --name "$CONTAINER_NAME" \
	    --user root \
	    --device /dev/gpiomem \
	    --device /dev/mem \
        ... # remaining arguments
}
```
<br>
<br>

## **Preparing the header file**
The *postprocesses* folder contains the necessary files to write the .so file.
```shell
hailo@raspberrypi:~$ cd $TAPPAS_WORKSPACE/core/hailo/libs/postprocesses/
hailo@raspberrypi:/local/workspace/tappas/core/hailo/libs/postprocesses$ vim yolo_teddybear.hpp
```
Here is what I wrote in the *yolo_teddybear.hpp* file:
```cpp
#pragma once
#include "hailo_objects.hpp"
#include "hailo_common.hpp"
#include "detection/yolo_postprocess.cpp"
#include "detection/yolo_output.cpp"
#include <pigpio.h>

__BEGIN_DECLS
void my_function(HailoROIPtr roi, void *params_void_ptr);
__END_DECLS
```
*hailo_objects.hpp* contains classes that represent the **inputs** (tensors) and **outputs** (detections, classifications, ...) that the process might handle. The main point of entry for data is the *HailoROI*, which can have tensors attached. 

*hailo_common.hpp* provides functions for handling these classes.

*params_void_ptr* is used to store information on the inference of the yolo model.

*yolo_postprocess.cpp* and *yolo_postprocess.hpp* provide **classes** and **functions** to run inference with the yolov5 model. They can be found in the *detection* folder. These files need the ***rapidjson*** library to function, since they need to work with a file named *yolov5.json* which holds the data for the yolo tensors.
<br>
<br>

## **Preparing the cpp file**
Then I proceeded to write a .cpp file.
```shell
hailo@raspberrypi:/local/workspace/tappas/core/hailo/libs/postprocesses$ vim yolo_teddybear.cpp
```
Here is what I wrote in the *yolo_teddybear.cpp* file:
```cpp
#include <iostream>
#include <typeinfo>
#include "yolo_teddybear.hpp"

#define LEDPIN 8

// Get the output layers from the hailo frame.
void my_function(HailoROIPtr roi, void *params_void_ptr)
{
  YoloParams *params = reinterpret_cast<YoloParams *>(params_void_ptr);
  // Gets the Yolo5 object
  auto post = Yolov5(roi, params);
  // Gets the detections (an object to represent the Yolo tensor)
  auto detections = post.decode();
  // Creates the bounding boxes
  hailo_common::add_detections(roi, detections);
  
  for(int i=0; i < detections.size(); i++){
	// Prints the labels from all the detections
	std::cout << "Detection n. " << i <<": " <<  detections[i].get_label() << std::endl;
    // If a detection has a teddy bear label the LEDPIN will be set to HIGH 
	if(detections[i].get_label() ==  "teddy bear" && gpioInitialise() >= 0){
		gpioSetMode(LEDPIN, PI_OUTPUT);
		gpioWrite(LEDPIN, 1);
	}
  }
}
```
The code to draw the bounding boxes has been taken from the *yolov5()* function in *yolo_postprocess.cpp*. It has been written in this custom function to make use of the *HailoDetection* objects (detections).

To make use of the GPIO, I used the ***pigpio*** library. 
<br>
<br>

## Building meson file
Next I have added the postprocess to the meson project so that it compiles.
**Meson** is an open source build system that puts an emphasis on speed and ease of use. **GStreamer** uses meson to generate build instructions to then be executed by **ninja**, another build system that requires a higher level build system (ie: meson) to generate its input files. Like GStreamer, Tappas also uses meson, and compiling new projects requires adjusting the meson.build files.
```shell
hailo@raspberrypi:/local/workspace/tappas/core/hailo/libs/postprocesses$ vim meson.build
```
I added this script at the end of the file:
```shell
###############################################
# MY POST SOURCES
###############################################
my_post_sources = [
  'yolo_teddybear.cpp',
]
my_post_lib = shared_library('yolo_teddybear',
 my_post_sources,
 include_directories: [hailo_general_inc] + xtensor_inc,
 dependencies : post_deps,
 gnu_symbol_visibility : 'default',
 install: true,
 install_dir: post_proc_install_dir,
 link_args : ['-lpigpio', '-lrt', '-lpthread'],
)
```
In short, this code is **providing paths** to cpp compilers, linked libraries, included directories, and dependencies. The _link_args : ['-lpigpio', '-lrt', '-lpthread']_ line has been added to link libraries to control the RaspberryPi GPIO.
<br>
<br>

## Compiling the .so
Now it's time to compile the postprocess. To do that the command to run is:
```shell
hailo@raspberrypi:~$ $TAPPAS_WORKSPACE/scripts/gstreamer/install_hailo_gstreamer.sh
```
After the compilation, the .so file should appear as
```shell
$TAPPAS_WORKSPACE/apps/gstreamer/libs/post_processes/libyolo_teddybear.so
```
<br>
<br>

## Running the inference
TAPPAS is a GStreamer based library of plug-ins. It enables using a Hailo devices within gstreamer pipelines to create inteliggent video processing applications. GStreamer’s development framework makes it possible to write any type of **streaming multimedia application**. The framework is based on plugins that will provide various codecs and other functionality and that can be linked and arranged in a pipeline, which defines the flow of the data.
Here is the GStreamer pipeline used:

```shell
gst-launch-1.0 v4l2src device=/dev/video0 name=src_0 ! \
videoflip video-direction=horiz ! \
queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
videoscale qos=false n-threads=2 ! \
video/x-raw, pixel-aspect-ratio=1/1 ! \
queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
videoconvert n-threads=2 qos=false ! \
queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
hailonet hef-path=/local/workspace/tappas/apps/gstreamer/general/detection/resources/yolov5m_wo_spp_60p.hef batch-size=1 ! \
queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
hailofilter \
function-name=my_function \
so-path=/local/workspace/tappas/apps/gstreamer/libs/post_processes/libyolo_teddybear.so \
config-path=/local/workspace/tappas/apps/gstreamer/general/detection/resources/configs/yolov5.json qos=false ! \
queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
hailooverlay qos=false ! \
queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
videoconvert n-threads=2 qos=false ! \
queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
fpsdisplaysink video-sink=ximagesink text-overlay=false name=hailo_display sync=false
```
Let's explain this pipeline section by section:
```shell
gst-launch-1.0 v4l2src device=/dev/video0 name=src_0 !
```
The pipeline starts with the **v4l2src** plugin, which reads video from a V4L2 device file */dev/video0* (which represents the USB Webcam). The name parameter sets a name for this element as src_0.
```shell
videoflip video-direction=horiz ! \
queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
```
The **videoflip** element flips the video horizontally, while the **queue** element is used to manage the buffering of the video frames (leaky=no means that it will drop frames if it reaches its maximum capacity).
```shell
videoscale qos=false n-threads=2 ! \
video/x-raw, pixel-aspect-ratio=1/1 !
```
The **videoscale** element scales the video according to the input needed by the pipeline, while **video/x-raw** sets the format of the video frames to raw video.
```shell
videoconvert n-threads=2 qos=false !
```
The **videoconvert** cap converts the video to feed the right representation to the **hailonet** element.
```shell
hailonet hef-path=/local/workspace/tappas/apps/gstreamer/general/detection/resources/yolov5m_wo_spp_60p.hef batch-size=1 !
```
The **hailonet** plugin performs the inference on the Hailo-8 device, by using the **yolov5** model, represented by the hef file provided.
```shell
hailofilter \
function-name=my_function \
so-path=/local/workspace/tappas/apps/gstreamer/libs/post_processes/libyolo_teddybear.so \
config-path=/local/workspace/tappas/apps/gstreamer/general/detection/resources/configs/yolov5.json qos=false !
```
**Hailofilter** performs the given postprocess, chosen with the *so-­path* property. The function called is set with the *function-name* parameter, while the *config-path* is necessary for the yolo inference to work, since it provides the json with the necessary information about the inference and a map containing the coco dataset.
```shell
hailooverlay qos=false !
```
**Hailooverlay** draws the postprocess results on the frame.
```shell
videoconvert n-threads=2 qos=false ! \
queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
fpsdisplaysink video-sink=ximagesink text-overlay=false name=hailo_display sync=false
```
The **videoconver** element is used again to convert the video frames to a format that can be displayed by **fpsdisplaysink**, which is a video sink that displays the video frames with a frames per second (FPS) counter. The fpsdisplaysink element is configured to use the **ximagesink** video sink for display, and the **text-overlay** property is set to false, which means that no text overlay will be shown on the video.