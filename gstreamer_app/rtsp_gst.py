import sys
import threading
import gi
import os.path
import time

gi.require_version('Gtk', '3.0')
gi.require_version('Gst', '1.0')
gi.require_version('GstVideo', '1.0')
from gi.repository import Gtk, Gst, GstVideo, GdkX11, GLib

Gst.init(None)

flag = True     # flag required to stop second thread

def get_window_handle(widget):
    return widget.get_window().get_xid()


class VideoPlayer:
    def __init__(self, canvas):
        self._canvas = canvas
        self._setup_pipeline()
    
    def _setup_pipeline(self):
        # The element with the set_window_handle function will be stored here
        self._video_overlay = None
        # Sets up pipeline using argv[1] as the rtsp server
        self._pipeline = Gst.parse_launch(
        "rtspsrc location=rtsp://" + sys.argv[1] + " name=webcam_source ! \
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
        hailotracker name=hailo_tracker keep-past-metadata=true \
	class-id=1 kalman-dist-thr=.7 iou-thr=.8 keep-tracked-frames=2 keep-lost-frames=50 ! \
        hailooverlay qos=false ! \
        queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
        videoconvert n-threads=2 qos=false ! \
        queue leaky=no max-size-buffers=30 max-size-bytes=0 max-size-time=0 ! \
        fpsdisplaysink video-sink=ximagesink text-overlay=false name=hailo_display sync=false"
        )
        
        # getting the source of the pipeline so it can be stopped
        self.source = self._pipeline.get_by_name('webcam_source')

        self._setup_signal_handlers()
    
    def _setup_signal_handlers(self):
        self._canvas.connect('realize', self._on_canvas_realize)
        
        bus = self._pipeline.get_bus()
        bus.enable_sync_message_emission()
        bus.connect('sync-message::element', self._on_sync_element_message)

    def _on_sync_element_message(self, bus, message):
        if message.get_structure().get_name() == 'prepare-window-handle':
            self._video_overlay = message.src
            self._video_overlay.set_window_handle(self._canvas_window_handle)
    
    def _on_canvas_realize(self, canvas):
        self._canvas_window_handle = get_window_handle(canvas)
        
    def start(self):
        self._pipeline.set_state(Gst.State.PLAYING)
        
    def stop_stream(self):
        self._pipeline.set_state(Gst.State.NULL)
    
    def setup_buttons(self, builder):
        pause_button = builder.get_object("pause")
        pause_button.connect("clicked", self.on_pause)
        
        play_button = builder.get_object("resume")
        play_button.connect("clicked", self.on_play)
    
    def on_pause(self, button):
        self.source.set_state(Gst.State.PAUSED)
        # self._pipeline.set_state(Gst.State.PAUSED)
        pass

    def on_play(self, button):
        self.source.set_state(Gst.State.PLAYING)
        # self._pipeline.set_state(Gst.State.PLAYING)
        pass


# this function starts the GTK execution
def run_stream(builder):
    window = builder.get_object("windowMain")
    
    canvas_box = builder.get_object("canvas_box")
    canvas_webcam = Gtk.DrawingArea()
    canvas_webcam.set_size_request(640, 600)
    canvas_box.add(canvas_webcam)
    
    text_view.set_size_request(640, 600)
    
    player = VideoPlayer(canvas_webcam)
    player.setup_buttons(builder)
    
    canvas_webcam.connect('realize', lambda *_: player.start())
        
    window.connect('destroy', Gtk.main_quit)
    window.show_all()
    
    Gtk.main()
    player.stop_stream()

def insert_text(line, text_view, text_buffer):
    text_buffer.insert_at_cursor(line)
    text_mark_end = text_buffer.create_mark("", text_buffer.get_end_iter(), False)
    text_view.scroll_to_mark(text_mark_end, 0, False, 0, 0) 
    
def read_from_file(text_view, text_buffer):
    path = "/local/workspace/tappas/yolo_contents.txt"
    while not os.path.isfile(path):
        time.sleep(1)
    if os.path.isfile(path):
        file = open(path, "r")
        while flag:
            line = file.readline()
            if line != "":
                GLib.idle_add(insert_text, line, text_view, text_buffer)
        file.close()

if __name__ =="__main__":
    builder = Gtk.Builder()
    builder.add_from_file("gui1.glade")
    
    text_view = builder.get_object("text_view")
    text_buffer = text_view.get_buffer()
    # creating thread
    t1 = threading.Thread(target=run_stream, args=[builder])
    t2 = threading.Thread(target=read_from_file, args=[text_view, text_buffer])
    # starting thread 1
    t1.start()
    t2.start()
    # wait until thread 1 is completely executed
    t1.join()
    flag = False # flag makes second thread end
    t2.join()