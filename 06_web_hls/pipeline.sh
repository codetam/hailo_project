#!/bin/sh

gst-launch-1.0 -v v4l2src device=/dev/video0 name=webcam_source ! \
    videoflip video-direction=horiz ! \
    queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    clockoverlay ! \
    videoscale qos=false n-threads=2 ! \
    video/x-raw, pixel-aspect-ratio=1/1 ! \
    queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    videoconvert n-threads=2 qos=false ! \
    queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    hailonet hef-path=/local/workspace/tappas/apps/gstreamer/general/detection/resources/yolov5m_wo_spp_60p.hef batch-size=1 ! \
    queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    hailofilter \
    function-name=my_function \
    so-path=/local/workspace/tappas/apps/gstreamer/libs/post_processes/libyolo_print_to_file.so \
    config-path=/local/workspace/tappas/apps/gstreamer/general/detection/resources/configs/yolov5.json qos=false ! \
    queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    hailotracker name=hailo_tracker keep-past-metadata=true \
	class-id=1 kalman-dist-thr=.7 iou-thr=.8 keep-tracked-frames=2 keep-lost-frames=50 ! \
    hailooverlay qos=false ! \
    queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    videoconvert n-threads=2 qos=false ! \
    queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    video/x-raw, framerate=5/1 ! \
    x264enc tune=zerolatency ! mpegtsmux ! \
    queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    hlssink playlist-root=http://192.168.1.36:80 location=/var/www/html/segment_%05d.ts target-duration=1 max-files=4