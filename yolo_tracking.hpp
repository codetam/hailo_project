#pragma once
#include "hailo_objects.hpp"
#include "hailo_common.hpp"
#include "detection/yolo_postprocess.cpp"
#include "detection/yolo_output.cpp"
#include "tracking/hailo_tracker.hpp"
#include <iostream>

__BEGIN_DECLS
void my_function(HailoROIPtr roi, void *params_void_ptr);
__END_DECLS
