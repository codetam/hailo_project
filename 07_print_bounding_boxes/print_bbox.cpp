#include <iostream>
#include <typeinfo>
#include "print_bbox.hpp"

// Get the output layers from the hailo frame.
void my_function(HailoROIPtr roi, void *params_void_ptr)
{
    YoloParams *params = reinterpret_cast<YoloParams *>(params_void_ptr);
    auto post = Yolov5(roi, params);
    auto detections = post.decode();
    hailo_common::add_detections(roi, detections);
    for (int i = 0; i < detections.size(); i++)
    {
        std::cout << "Detection n. " << i << ": " << detections[i].get_label() << std::endl;
        std::cout << "  Confidence: " << detections[i].get_confidence() << std::endl;
        HailoBBox detection_bbox = detections[i].get_bbox();
        std::cout << "  Bbox: xmin=" << detection_bbox.xmin() << " ymin=" << detection_bbox.ymin() << " width:" << detection_bbox.width() << " height:" << detection_bbox.height() << std::endl;
    }   
}