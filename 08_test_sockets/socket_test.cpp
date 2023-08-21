#include "socket_test.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <signal.h>

const int PORT = 8080;  // Choose a port number
int serverSocket = -1;
int clientSocket = -1;

int i = 0;

void setup_socket() {
    if (serverSocket == -1) {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            perror("Error creating socket");
            return;
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("Error binding");
            close(serverSocket);
            serverSocket = -1;
            return;
        }

        if (listen(serverSocket, 1) == -1) {
            perror("Error listening");
            close(serverSocket);
            serverSocket = -1;
            return;
        }
        // Set the server socket to non-blocking
        fcntl(serverSocket, F_SETFL, O_NONBLOCK);
    }
}

void my_function(HailoROIPtr roi, void *params_void_ptr) {
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa, 0) == -1) {
        perror(0);
        exit(1);
    }
    
    setup_socket();
    
    if (clientSocket == -1) {
        clientSocket = accept(serverSocket, NULL, NULL);
    }

    int i = 0;
    if (clientSocket != -1) 
    {
        std::string output;
        for (auto obj : roi->get_objects())
        {
            if(obj->get_type() == HAILO_DETECTION)
            {
                i++;
                HailoDetectionPtr detection = std::dynamic_pointer_cast<HailoDetection>(obj);
                output += "Detection n. " + std::to_string(i) + ": " + detection->get_label() + "\n";
                // ID number
                for (auto obj_detection : detection->get_objects())
                {
                    if(obj_detection->get_type() == HAILO_UNIQUE_ID){
                    HailoUniqueIDPtr id = std::dynamic_pointer_cast<HailoUniqueID>(obj_detection);
                        if (id->get_mode() == GLOBAL_ID || id->get_mode() == TRACKING_ID)
                        {
                            output += "  ID Number: " + std::to_string(id->get_id()) + "\n"; 
                        }
                    }
                }
                // Detection info
                output += "  Confidence: " + std::to_string(detection->get_confidence()) + "\n";
                HailoBBox detection_bbox = detection->get_bbox();
                output += "  Bbox: xmin=" + std::to_string(detection_bbox.xmin()) 
                        + " ymin=" + std::to_string(detection_bbox.ymin()) 
                        + " width:" + std::to_string(detection_bbox.width()) 
                        + " height:" + std::to_string(detection_bbox.height()) 
                        + "\n";
            }
        }
        ssize_t bytesSent = send(clientSocket, output.c_str(), output.size(), 0);
        if (bytesSent == -1) {
            printf("Client disconnected\n");
            close(clientSocket);
            clientSocket = -1;
        }
    }
}
