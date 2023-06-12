#!/bin/sh

gst-launch-1.0 rtspsrc location="rtsp://192.168.1.102:8554/cam" name=src_0 ! \
	rtph264depay ! h264parse ! avdec_h264 max_threads=2 ! \
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
        so-path=/local/workspace/tappas/apps/gstreamer/libs/post_processes/libyolo_print_to_file.so \
        config-path=/local/workspace/tappas/apps/gstreamer/general/detection/resources/configs/yolov5.json qos=false ! \
        queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
        hailooverlay qos=false ! \
        queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
        videoconvert n-threads=2 qos=false ! \
        queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
        fpsdisplaysink video-sink=ximagesink text-overlay=false name=hailo_display sync=false
