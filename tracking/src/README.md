HailoTracker Source

https://github.com/hailo-ai/tappas/blob/3c2b49d62aa928529574736dc11377eb32577a50/core/hailo/tracking/hailo_tracker.cpp

https://github.com/hailo-ai/tappas/blob/3c2b49d62aa928529574736dc11377eb32577a50/core/hailo/tracking/hailo_tracker.hpp

Samples

https://github.com/hailo-ai/tappas/blob/3c2b49d62aa928529574736dc11377eb32577a50/core/hailo/libs/postprocesses/recognition/arcface.cpp

https://github.com/hailo-ai/tappas/blob/3c2b49d62aa928529574736dc11377eb32577a50/core/hailo/libs/postprocesses/classification/person_attributes.cpp

https://github.com/hailo-ai/tappas/blob/3c2b49d62aa928529574736dc11377eb32577a50/core/hailo/libs/postprocesses/classification/face_attributes.cpp

JDE tracker

https://github.com/Zhongdao/Towards-Realtime-MOT

How SORT and DeepSORT work

source(https://learnopencv.com/understanding-multiple-object-tracking-using-deepsort/#:~:text=DeepSORT%20can%20be%20defined%20as,the%20appearance%20of%20the%20object).

3.1 Simple Online Realtime Tracking (SORT)
SORT is an approach to Object tracking where rudimentary approaches like Kalman filters and Hungarian algorithms are used to track objects and claim to be better than many online trackers. SORT is made of 4 key components which are as follows:

Detection: This is the first step in the tracking module. In this step, an object detector detects the objects in the frame that are to be tracked. These detections are then passed on to the next step. Detectors like FrRCNN, YOLO, and more are most frequently used.
Estimation: In this step, we propagate the detections from the current frame to the next which is estimating the position of the target in the next frame using a constant velocity model. When a detection is associated with a target, the detected bounding box is used to update the target state where the velocity components are optimally solved via the Kalman filter framework
Data association: We now have the target bounding box and the detected bounding box. So, a cost matrix is computed as the intersection-over-union (IOU) distance between each detection and all predicted bounding boxes from the existing targets. The assignment is solved optimally using the Hungarian algorithm. If the IOU of detection and target is less than a certain threshold value called IOUmin then that assignment is rejected. This technique solves the occlusion problem and helps maintain the IDs.
Creation and Deletion of Track Identities: This module is responsible for the creation and deletion of IDs. Unique identities are created and destroyed according to the IOUmin. If the overlap of detection and target is less than  IOUmin then it signifies the untracked object. Tracks are terminated if they are not detected for TLost frames, you can specify what the amount of frame should be for TLost. Should an object reappear, tracking will implicitly resume under a new identity.
The objects can be successfully tracked using SORT algorithms beating many State-of-the-art algorithms. The detector gives us detections, Kalman filters give us tracks and the Hungarian algorithm performs data association. So, Why do we even need DeepSORT?

SORT performs very well in terms of tracking precision and accuracy. But SORT returns tracks with a high number of ID switches and fails in case of occlusion. This is because of the association matrix used. DeepSORT uses a better association metric that combines both motion and appearance descriptors. DeepSORT can be defined as the tracking algorithm which tracks objects not only based on the velocity and motion of the object but also the appearance of the object.

Mahalanobis Distance

https://www.machinelearningplus.com/statistics/mahalanobis-distance/

## What I did

    hailo@raspberrypi:/local/workspace/tappas/core/hailo$ sudo cp tracking/ libs/postprocesses/ -r

    HAD TO UPDATE ENTIRELY HAILO_OBJECTS AND HAILO_COMMON!
    for get_stream_id()!