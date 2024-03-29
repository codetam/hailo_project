## Python libraries:

```console
$ sudo apt-get update
$ sudo apt-get upgrade
$ sudo apt-get install gstreamer1.0-plugins-good
# Downloading pyGObject
$ pip3 install pycairo
$ pip3 install PyGObject
```

## PyGObject
PyGObject is a Python package which provides bindings for GObject based libraries such as GTK, GStreamer, WebKitGTK, GLib, GIO and many more.

## Sources
https://gstreamer.freedesktop.org/documentation/tutorials/basic/hello-world.html?gi-language=python

https://github.com/gkralik/python-gst-tutorial

https://discourse.gnome.org/t/reliable-way-to-pause-play-a-pipeline-with-webcam-streaming-and-tee/3355

https://stackoverflow.com/questions/70517084/how-to-change-a-videooverlays-window-handle-after-it-has-already-been-set

https://python-gtk-3-tutorial.readthedocs.io/en/latest/textview.html

https://pygobject.readthedocs.io/en/latest/guide/threading.html (Threading)

## Function

This GStreamer GTK app allows the user to run inference in a GTK Window. A separate window will print the detections on screen after reading them from a text file. If the second argument is "tracking", additionally, it will print to the terminal whenever a new detection is found.

## Usage

~~~bash
$ python3 rtsp_gst.py [IP_ADDRESS] [detection / tracking]

$ python3 usb_gst.py [detection / tracking] 
~~~
