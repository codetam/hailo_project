#include <iostream>
#include <typeinfo>
#include "check_track_id.hpp"

void my_function(HailoROIPtr roi, void *params_void_ptr) {
    int i = 0;
    for (auto obj : roi->get_objects())
    {
        if(obj->get_type() == HAILO_DETECTION)
        {
            i++;
            HailoDetectionPtr detection = std::dynamic_pointer_cast<HailoDetection>(obj);
            std::cout << std::endl << "Detection n. " << i << ": " << detection->get_label() << std::endl;
            // ID number
            for (auto obj_detection : detection->get_objects())
            {
                if(obj_detection->get_type() == HAILO_UNIQUE_ID){
                   HailoUniqueIDPtr id = std::dynamic_pointer_cast<HailoUniqueID>(obj_detection);
                    if (id->get_mode() == GLOBAL_ID || id->get_mode() == TRACKING_ID)
                    {
                        std::string id_text = std::to_string(id->get_id());
                        std::cout << "  ID Number: " << id_text << std::endl; 
                    }
                }
            }
            // Detection info
            std::cout << "  Confidence: " << detection->get_confidence() << std::endl;
            HailoBBox detection_bbox = detection->get_bbox();
            std::cout << "  Bbox: xmin=" << detection_bbox.xmin() << " ymin=" << detection_bbox.ymin() << " width:" << detection_bbox.width() << " height:" << detection_bbox.height() << std::endl;
        }
    }
}
