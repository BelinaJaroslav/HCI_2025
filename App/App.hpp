#pragma once

#include <atomic>
#include <tuple>

#include <opencv2/opencv.hpp>

#include "CV2Tools.hpp"
#include "FaceDetector.hpp"
#include "FPSMeter.hpp"
#include "SyncedDeque.hpp"
#include "ThreadPool.hpp"



class App {
public:
    App();
    bool init();
    void run();    
    ~App();

    struct ProcessedFrame {
        int id;
        cv::Mat processed_image;  // e.g., decoded after compression
        cv::Mat original_image;   // the original captured frame
    };

    SyncedDeque<ProcessedFrame> result_queue;


private:


    // == MEMBERS ==
    FaceDetector face_detector;
    FPSMeter fps_meter_main;
    SyncedDeque<std::tuple<cv::Mat, std::vector<cv::Point2f>>> synced_deque;


    cv::VideoCapture capture;

    std::atomic<bool> do_terminate_worker_threads;

    // == METHODS ==
    // Lab 01
    void lab_identify_object_by_luminance() const;
    void lab_find_red_object_in_image() const;
    void lab_find_red_object_in_video();
    void lab_find_face_in_video();    
    
    // Lab 02
    void lab_complex_behaviour();
    
    // Lab 03
    void lab_multithread();
    
    void tracker_thread();
    void render_thread();

    // Lab 04
    int lab_compression(); 

    void grabber_thread();

    void process_frame(const cv::Mat& original, int id, int threshold, int quality, SyncedDeque<ProcessedFrame>& result_queue);


    std::vector<uchar> lossy_bw_limit(cv::Mat& input_img, size_t size_limit);
    std::vector<uchar> lossy_quality_limit(const cv::Mat& frame, const float target_coefficient);
    int lab_compression_pool();
    
    // Lab 05
    // Lab 06
    // Lab 07
    // Lab 08
    // Lab 09
    // Lab 10
};
