#include <App/App.hpp>

#include <fmt/core.h>

#include <thread>
#include <condition_variable>

#include <iostream>
#include <numeric>
#include <vector>
#include <array>
#include <opencv2/opencv.hpp>
#include <cmath>


std::atomic<int> global_frame_id{ 0 };

const int MAX_QUALITY = 95.0f;
const int MIN_QUALITY = 10.0f;
int PSNR_threshold = 30.0f;

ThreadPool threadPool(6);


struct ProcessedFrame {
    int id;
    cv::Mat processed_image;  // e.g., decoded after compression
    cv::Mat original_image;   // the original captured frame
};


void App::grabber_thread() {

    int display_id = 1;

    cv::Mat frame;

    while (capture.isOpened() && !do_terminate_worker_threads)
    {
        // loads next captured frame
        capture >> frame;

        if (frame.empty()) {
            std::cerr << "device closed (or video at the end)" << '\n';
            capture.release();
            break;
        }


        int id = global_frame_id++;
        // push frames into pool
        threadPool.enqueue([frame, id, this]() {
            process_frame(frame.clone(), id, PSNR_threshold, MAX_QUALITY, this->result_queue);
            });

        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
}


void App::process_frame(const cv::Mat& original,
    int id,
    int threshold,
    int quality,
    SyncedDeque<ProcessedFrame>& result_queue
)

{
    std::vector<uchar> encoded;
    std::vector<int> params = {
        cv::IMWRITE_JPEG_QUALITY, quality
    };

    // Encode
    if (!cv::imencode(".jpg", original, encoded, params))
        return;

    // Decode
    cv::Mat decoded = cv::imdecode(encoded, cv::IMREAD_COLOR);
    if (decoded.empty()) return;

    // Evaluate
    double psnr = CV2Tools::getPSNR(original, decoded);

    // Recursively reduce quality if PSNR is too high
    if (psnr > threshold && quality > MIN_QUALITY) {
        this->process_frame(original, id, threshold, quality - 5, result_queue);
    }
    else {

        // Send to queue for main thread to display, expecting ordered return of processed frames :)
        result_queue.push_back(ProcessedFrame{ id, decoded, original.clone() });
    }
}


int App::lab_compression_pool() {

    std::jthread grabber(&App::grabber_thread, this);
    std::map<int, ProcessedFrame> buffer;

    int expected_frame_id = 0;  // Local frame tracker for display order

    try {
        cv::namedWindow("original", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("decoded", cv::WINDOW_AUTOSIZE);

        while (capture.isOpened() && !do_terminate_worker_threads)
        {
            ProcessedFrame compressed_frame = result_queue.pop_front_wait();

            if (compressed_frame.id != expected_frame_id) {
                buffer[compressed_frame.id] = std::move(compressed_frame);
                continue;
            }

            // Process and look ahead for next frames
            while (true) {
                // --- Display Frame ---
                auto size_uncompressed = compressed_frame.original_image.elemSize() * compressed_frame.original_image.total();
                auto size_compressed = compressed_frame.processed_image.elemSize() * compressed_frame.processed_image.total();

                std::cout << "Frame id: " << compressed_frame.id << "\n";

                cv::imshow("original", compressed_frame.original_image);
                cv::imshow("decoded", compressed_frame.processed_image);

                // Handle key press
                int c = cv::pollKey();
                switch (c) {
                case 27:
                    do_terminate_worker_threads = true;
                    return EXIT_SUCCESS;
                case 'q':
                    PSNR_threshold += 5;
                    break;
                case 'a':
                    PSNR_threshold -= 5;
                    break;
                default:
                    break;
                }

                PSNR_threshold = std::clamp(PSNR_threshold, 0, 50);

                // Move to next expected frame
                ++expected_frame_id;

                // Check buffer for the next frame
                auto it = buffer.find(expected_frame_id);
                if (it == buffer.end()) break;

                compressed_frame = std::move(it->second);
                buffer.erase(it);
            }
        }
    }
    catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    cv::destroyAllWindows();
    std::cout << "Finished OK...\n";
    return EXIT_SUCCESS;
}
