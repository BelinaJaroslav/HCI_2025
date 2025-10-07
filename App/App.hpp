#pragma once

#include <opencv2/opencv.hpp>

#include "CV2Tools.hpp"
#include "FaceDetector.hpp"
#include "FPSMeter.hpp"

class App {
public:
    App();
    bool init();
    void run();    
    ~App();

private:
    // == MEMBERS ==
    FaceDetector face_detector;
    FPSMeter fps_meter;

    cv::VideoCapture capture;             

    // == METHODS ==
    // Lab 01
    void lab_identify_object_by_luminance() const;
    void lab_find_red_object_in_image() const;
    void lab_find_red_object_in_video();
    void lab_find_face_in_video();    
    // Lab 02
    void lab_complex_behaviour();
    // Lab 03
    // Lab 04
    // Lab 05
    // Lab 06
    // Lab 07
    // Lab 08
    // Lab 09
    // Lab 10
};
