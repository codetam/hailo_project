gst-launch-1.0 v4l2src device=/dev/video0 name=src_0 ! \
videoflip video-direction=horiz ! \
queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
videoscale qos=false n-threads=2 ! \
videoconvert n-threads=1 qos=false ! \
video/x-raw,format=RGB ! \
queue name=hailo_pre_cropper1_q leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
hailocropper so-path=/local/workspace/tappas/apps/gstreamer/libs/post_processes//cropping_algorithms/libwhole_buffer.so \
function-name=create_crops use-letterbox=true resize-method=inter-area internal-offset=true name=cropper1 \
hailoaggregator name=agg1 cropper1. ! \
queue name=bypess1_q leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
agg1. cropper1. ! \
queue name=hailo_pre_detector_q leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
hailonet hef-path=/local/workspace/tappas/apps/gstreamer/general/multi_person_multi_camera_tracking/resources/yolov5s_personface.hef \
scheduling-algorithm=1 vdevice-key=1 ! \
queue name=detector_post_q leaky=no max-size-buffers=1000 max-size-bytes=0 max-size-time=0 ! \
hailofilter so-path=/local/workspace/tappas/apps/gstreamer/libs/post_processes//libyolo_post.so qos=false \
function_name=yolov5_personface_letterbox \
config-path=/local/workspace/tappas/apps/gstreamer/general/multi_person_multi_camera_tracking/resources/configs/yolov5_personface.json ! \
queue name=detector_pre_agg_q leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
agg1. agg1. ! \
queue name=hailo_pre_tracker leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
hailotracker name=hailo_tracker class-id=1 kalman-dist-thr=0.7 iou-thr=0.7 init-iou-thr=0.8 keep-new-frames=2 keep-tracked-frames=4 keep-lost-frames=8 qos=false ! \
queue name=hailo_pre_cropper2_q leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
hailocropper so-path=/local/workspace/tappas/apps/gstreamer/libs/post_processes//cropping_algorithms/libre_id.so \
function-name=create_crops internal-offset=true name=cropper2 \
hailoaggregator name=agg2 cropper2. ! \
queue name=bypess2_q leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
agg2. cropper2. ! \
queue name=pre_reid_q leaky=no max-size-buffers=10 max-size-bytes=0 max-size-time=0 ! \
hailonet hef-path=/local/workspace/tappas/apps/gstreamer/general/multi_person_multi_camera_tracking/resources/repvgg_a0_person_reid_2048.hef \
scheduling-algorithm=1 vdevice-key=1 ! \
queue name=reid_post_q leaky=no max-size-buffers=10 max-size-bytes=0 max-size-time=0 ! \
hailofilter so-path=/local/workspace/tappas/apps/gstreamer/libs/post_processes//libre_id.so qos=false ! \
queue name=reid_pre_agg_q leaky=no max-size-buffers=10 max-size-bytes=0 max-size-time=0 ! \
agg2. agg2. ! \
queue name=hailo_pre_gallery leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
hailogallery similarity-thr=.4 gallery-queue-size=100 class-id=1 ! \
queue name=hailo_post_gallery leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
videoscale n-threads=2 add-borders=false qos=false ! video/x-raw, width=800, height=450, pixel-aspect-ratio=1/1 ! \
queue name=hailo_pre_draw leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
hailofilter use-gst-buffer=true so-path=/local/workspace/tappas/apps/gstreamer/libs/apps/re_id//libre_id_overlay.so qos=false ! \
queue name=hailo_post_draw leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
hailooverlay qos=false ! \
queue name=hailo_video_q_0 leaky=no max_size_buffers=30 max-size-bytes=0 max-size-time=0 ! \
videoconvert n-threads=2 qos=false ! \
queue name=hailo_display_q_0 leaky=no max_size_buffers=300 max-size-bytes=0 max-size-time=0 ! \
fpsdisplaysink video-sink=ximagesink name=hailo_display sync=false text-overlay=false