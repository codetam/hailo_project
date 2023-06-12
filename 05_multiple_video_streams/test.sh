# First test with basic videotestsrc

gst-launch-1.0 -v videotestsrc pattern=snow ! video/x-raw,width=800,height=600 ! videobox left=-800 border-alpha=0 ! queue ! videomixer name=mix ! videoconvert ! autovideosink videotestsrc ! video/x-raw,width=800,height=600 ! videobox left=0 ! queue ! mix.