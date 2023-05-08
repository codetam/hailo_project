#include "yolo_tracking.hpp"


// Get the output layers from the hailo frame.
void my_function(HailoROIPtr roi, void *params_void_ptr)
{
    if (!roi->has_tensors())
    {
        return;
    }
  YoloParams *params = reinterpret_cast<YoloParams *>(params_void_ptr);
  auto post = Yolov5(roi, params);
  auto detections = post.decode();
  hailo_common::add_detections(roi, detections);
  for(int i=0; i < detections.size(); i++){
	std::cout << "Detection n. " << i <<": " <<  detections[i].get_label() << std::endl;
  }
  auto unique_ids = hailo_common::get_hailo_track_id(roi);
  if(unique_ids.empty()){
    roi->remove_objects_typed(HAILO_MATRIX);
    std::cour << "Matrix removed" << std::endl;
  }
  else{
    std::cout << "Unique id: " << unique_ids[0]->get_id() << std::endl;
  }
    
}
