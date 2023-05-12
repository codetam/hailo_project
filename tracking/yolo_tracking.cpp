#include "yolo_tracking.hpp"

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
    }
    std::vector<HailoDetectionPtr> detections_ptrs = hailo_common::get_hailo_detections(roi);
    
    for (HailoDetectionPtr &detection : detections_ptrs)
    {
        int tracking_id = get_tracking_id(detection)->get_id();
        std::cout << "Tracking ID: " << tracking_id << std::endl;
    }
}

HailoUniqueIDPtr get_global_id(HailoDetectionPtr detection)
{
    for (auto obj : detection->get_objects_typed(HAILO_UNIQUE_ID))
    {
        HailoUniqueIDPtr id = std::dynamic_pointer_cast<HailoUniqueID>(obj);
        if (id->get_mode() == GLOBAL_ID)
        {
            return id;
        }
    }
    return nullptr;
}

https://github.com/hailo-ai/tappas/blob/3c2b49d62aa928529574736dc11377eb32577a50/core/hailo/apps/x86/re_id/re_id_overlay.cpp