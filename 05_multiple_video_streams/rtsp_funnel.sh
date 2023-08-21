#!/bin/bash
set -e

sources=""
streamrouter_input_streams=""

function create_sources() {
    start_index=0
    identity=""

    sources+="rtspsrc location=rtsp://192.168.1.102:8554/cam \
            name=source_0 ! rtph264depay ! h264parse ! avdec_h264 max_threads=2 ! \
            clockoverlay ! \
    	    queue name=hailo_preprocess_q_0 leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    	    videoscale qos=false n-threads=2 ! \
    	    video/x-raw,width=640,height=640,pixel-aspect-ratio=1/1 ! \
            queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
            videoconvert n-threads=2 qos=false ! \
    	    fun.sink_0 sid.src_0 ! \
    	    queue name=comp_q_0 leaky=downstream max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    	    comp.sink_0 "
     streamrouter_input_streams+=" src_0::input-streams=\"<sink_0>\""

    sources+="rtspsrc location=rtsp://192.168.1.16:8554/cam \
            name=source_1 ! rtph264depay ! h264parse ! avdec_h264 max_threads=2 ! \
            queue name=hailo_preprocess_q_1 leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
            videoscale qos=false n-threads=2 ! \
            video/x-raw,width=640,height=640,pixel-aspect-ratio=1/1 ! \
            queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
            videoconvert n-threads=2 qos=false ! \
            fun.sink_1 sid.src_1 ! \
	        queue name=comp_q_1 leaky=downstream max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
            comp.sink_1 "
     streamrouter_input_streams+=" src_1::input-streams=\"<sink_1>\""
}


function main() {
    create_sources


    pipeline="gst-launch-1.0 \
    hailoroundrobin name=fun ! \
    queue name=hailo_pre_infer_q_0 leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    hailonet hef-path=/local/workspace/tappas/apps/gstreamer/general/multistream_detection/resources/yolov5m_wo_spp_60p.hef ! \
    queue name=hailo_postprocess0 leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    hailofilter \
    function-name=my_function \
    so-path=/local/workspace/tappas/apps/gstreamer/libs/post_processes/libprint_bbox_to_file_multistream.so \
    config-path=/local/workspace/tappas/apps/gstreamer/general/detection/resources/configs/yolov5.json qos=false ! \
    queue name=hailo_draw0 leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    hailooverlay ! \
    hailostreamrouter name=sid $streamrouter_input_streams compositor name=comp start-time-selection=0 \
    sink_0::xpos=0 sink_0::ypos=0 \
    sink_1::xpos=640 sink_1::ypos=0 ! \
    queue name=hailo_video_q_0 leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    videoconvert ! \
    queue name=hailo_display_q_0 leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
    fpsdisplaysink video-sink=ximagesink name=hailo_display sync=false text-overlay=false \
    $sources"

    eval "${pipeline}"
}

main $@