#include "yolo_print_to_file.hpp"

using namespace std;
char Yolov5::prev_date[30] = "Initialization text";

// Get the output layers from the hailo frame.
void my_function(HailoROIPtr roi, void *params_void_ptr)
{
  YoloParams *params = reinterpret_cast<YoloParams *>(params_void_ptr);
  auto post = Yolov5(roi, params);       	    // Gets the Yolo5 object
  auto detections = post.decode();      	    // Gets the detections (an object to represent the Yolo tensor)
  hailo_common::add_detections(roi, detections);    // Creates the bounding boxes
  
  string filename = "/local/workspace/tappas/yolo_contents.txt";

  time_t my_time = time(NULL);
  char* current_time = ctime(&my_time);
  current_time[strcspn(current_time, "\n")] = 0;	

  if(strcmp(Yolov5::prev_date,current_time) != 0){ // Only if the time is different
    ofstream outfile;
    outfile.open(filename, ios_base::app);	   //opens the log file
    outfile << current_time <<": ";		   //Prints current time
    for(int i=0; i < detections.size(); i++){
      outfile << detections[i].get_label() << " "; //Prints the labels from the detections
    }
    outfile << endl;
    outfile.close();
    memcpy(Yolov5::prev_date,current_time,30 * sizeof(char)); //Saves new time
  }
}
