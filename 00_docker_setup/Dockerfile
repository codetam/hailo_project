FROM hailo_tappas:v3.23.1
COPY source/rapidjson /local/workspace/tappas/core/hailo/libs/postprocesses/rapidjson
COPY source/pigpio-master /local/workspace/tappas/pigpio-master
COPY source/scripts/yolo_print_to_file.* /local/workspace/tappas/core/hailo/libs/postprocesses
COPY source/meson.build /local/workspace/tappas/core/hailo/libs/postprocesses/meson.build
COPY source/detection /local/workspace/tappas/core/hailo/libs/postprocesses/detection
RUN ["/bin/bash", "-c", "sudo apt update -y && sudo apt upgrade -y"]
RUN ["/bin/bash", "-c", "sudo apt install apache2 -y && sudo service apache2 start"]
RUN ["/bin/bash", "-c", "cd /local/workspace/tappas/pigpio-master && sudo make && sudo make install"]
RUN ["/bin/bash", "-c", "/local/workspace/tappas/scripts/gstreamer/install_hailo_gstreamer.sh"]
RUN ["/bin/bash", "-c", "export LD_PRELOAD=/usr/lib/aarch64­linux­gnu/libgomp.so.1"]
