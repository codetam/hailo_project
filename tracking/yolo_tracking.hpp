#pragma once
#include "hailo_objects.hpp"
#include "hailo_common.hpp"
#include "detection/yolo_postprocess.cpp"
#include "detection/yolo_output.cpp"
#include <iostream>

__BEGIN_DECLS
void my_function(HailoROIPtr roi, void *params_void_ptr);
HailoUniqueIDPtr get_tracking_id(HailoDetectionPtr detection);
__END_DECLS
