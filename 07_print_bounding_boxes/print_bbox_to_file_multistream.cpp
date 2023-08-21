#include "print_bbox_to_file.hpp"

using namespace std;

char prev_date[30] = "";
int num_streams = 2;
int current_stream_num = 0;
bool second_has_passed = false;
ofstream outfile;

// Get the output layers from the hailo frame.
void my_function(HailoROIPtr roi, void *params_void_ptr)
{
    YoloParams *params = reinterpret_cast<YoloParams *>(params_void_ptr);
    auto post = Yolov5(roi, params);       	    // Gets the Yolo5 object
    auto detections = post.decode();      	    // Gets the detections (an object to represent the Yolo tensor)
    hailo_common::add_detections(roi, detections);    // Creates the bounding boxes

    time_t my_time = time(NULL);
    char* current_time = ctime(&my_time);
    current_time[strcspn(current_time, "\n")] = 0;	

    // Opens the file if it isn't open already
    if (!outfile.is_open())
    {
        string filename = "/local/workspace/tappas/yolo_contents.txt";
        outfile.open(filename, ios_base::app);
    }
    // When a second has passed, the flag is set to true
    if(strcmp(prev_date,current_time) != 0){  
        second_has_passed = true; 
    }
    // If the flag is true and we're at the first stream, we start printing
    if(second_has_passed && current_stream_num % num_streams == 0){
        second_has_passed = false;
        outfile << current_time <<": " << endl;     //Prints current time
        current_stream_num = 0; 
    }
    
    if(current_stream_num < num_streams){
        outfile << "  Stream n. " << current_stream_num << endl;
        for(int i=0; i < detections.size(); i++){
            outfile << "    Detection n. " << i << ": " << detections[i].get_label() << endl;
            outfile << "      Confidence: " << detections[i].get_confidence() << endl;
            HailoBBox detection_bbox = detections[i].get_bbox();
            outfile << "      Bbox: xmin=" << detection_bbox.xmin() << " ymin=" << detection_bbox.ymin() << " width:" << detection_bbox.width() << " height:" << detection_bbox.height() << endl;
        }
        outfile << endl;
    }
    current_stream_num++;
    memcpy(prev_date,current_time,30 * sizeof(char)); //Saves new time
}

void two_streams(HailoROIPtr roi, void *params_void_ptr){
    num_streams = 2;
    my_function(roi, params_void_ptr);
}
