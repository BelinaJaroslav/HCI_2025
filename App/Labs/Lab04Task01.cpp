#include "App/DefinesAndMacros.hpp"
#ifndef SKIP_LABS_COMPILATION

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

constexpr float MAX_PSNR = 50.0f;


/*
void App::grabber_thread() {
    while (capture.isOpened()) {
        cv::Mat frame_;
        capture >> frame_;

        if (frame_.empty()) {
            std::cerr << "Device closed (or video at the end)" << '\n';
            capture.release();
            break;
        }

        // Clone to ensure thread safety
        synced_deque_raw.push_back(frame_.clone());
    }

    // Signal end of stream with empty Mat
    synced_deque_raw.push_back({});
}


void App::encoder_thread(int quality) {
    while (true) {
        cv::Mat frame = synced_deque_raw.pop_front_wait();

        // Sentinel value to stop
        if (frame.empty()) break;

        std::string suff = ".jpg";
        if (!cv::haveImageWriter(suff)) {
            throw std::runtime_error("Cannot compress to format: " + suff);
        }

        std::vector<uchar> encoded;
        std::vector<int> compression_params;

        // JPEG quality (0-100), higher is better
        compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
        compression_params.push_back(quality);

        // Encode frame to JPEG
        bool success = cv::imencode(suff, frame, encoded, compression_params);
        if (!success) {
            std::cerr << "Encoding failed for frame" << std::endl;
            continue;
        }

        // Push encoded JPEG bytes to next queue
        synced_deque_encoded.push_back(encoded);
    }

    // Signal end of encoded stream
    synced_deque_encoded.push_back({});
}


void App::decoder_thread() {
    while (true) {
        std::vector<uchar> encoded = synced_deque_encoded.pop_front_wait();

        // Sentinel value to stop
        if (encoded.empty()) break;

        // Decode JPEG bytes into image
        cv::Mat decoded = cv::imdecode(encoded, cv::IMREAD_COLOR);
        if (decoded.empty()) {
            std::cerr << "Decoding failed for image" << std::endl;
            continue;
        }

        // Push decoded frame to next stage
        synced_deque_decoded.push_back(decoded);
    }

    // Signal end of decoded stream
    synced_deque_decoded.push_back({});
}
/**/


std::vector<uchar> App::lossy_bw_limit(cv::Mat& input_img, size_t size_limit) 
{
    std::string suff(".jpg"); // target format
    if (!cv::haveImageWriter(suff))
        throw std::runtime_error("Can not compress to format:" + suff);

    std::vector<uchar> bytes;
    std::vector<int> compression_params;

    // prepare parameters for JPEG compressor
    // we use only quality, but other parameters are available (progressive, optimization...)
    std::vector<int> compression_params_template;
    compression_params_template.push_back(cv::IMWRITE_JPEG_QUALITY);

    std::cout << '[';

    //try step-by-step to decrease quality by 5%, until it fits into limit
    for (auto i = 100; i > 0; i -= 5) {
        compression_params = compression_params_template; // reset parameters
        compression_params.push_back(i);                  // set desired quality
        std::cout << i << ',';

        // try to encode
        cv::imencode(suff, input_img, bytes, compression_params);

        // check the size limit
        if (bytes.size() <= size_limit)
            break; // ok, done 
    }
    std::cout << "]\n";

    return bytes;
}


std::vector<uchar> App::lossy_quality_limit(const cv::Mat& frame, const float target_coefficient)
{
    std::string suff(".jpg"); // target format
    if (!cv::haveImageWriter(suff))
        throw std::runtime_error("Can not compress to format:" + suff);

    std::vector<uchar> bytes;
    std::vector<int> compression_params;

    // prepare parameters for JPEG compressor
    // we use only quality, but other parameters are available (progressive, optimization...)
    std::vector<int> compression_params_template;
    compression_params_template.push_back(cv::IMWRITE_JPEG_QUALITY);

    std::cout << '[';

    //try step-by-step to decrease quality by 5%, until it fits into limit
    for (auto i = 100; i > 0; i -= 5) {
        compression_params = compression_params_template; // reset parameters
        compression_params.push_back(i);                  // set desired quality
        std::cout << i << ',';

        // try to encode
        cv::imencode(suff, frame, bytes, compression_params);

        // try to decode
        cv::Mat decoded = cv::imdecode(bytes, cv::IMREAD_COLOR);
        if (decoded.size() != frame.size()) {
            cv::resize(decoded, decoded, frame.size());
        }
        // check the quality with PSNR
        float min_psnr = target_coefficient * MAX_PSNR;
        if (CV2Tools::get_PSNR(decoded,frame) <= min_psnr) {
            break;
        }
    }
    std::cout << "]\n";

    return bytes;
}


int App::lab_compression() {
    cv::Mat frame;
    std::vector<uchar> bytes;
    float target_coefficient = 0.5f; // used as size-ratio, or quality-coefficient
    try {
        while (capture.isOpened())
        {
            capture >> frame;

            if (frame.empty()) {
                std::cerr << "device closed (or video at the end)" << '\n';
                capture.release();
                break;
            }

            // encode image with bandwidth limit
            auto size_uncompressed = frame.elemSize() * frame.total();
            auto size_compressed_limit = size_uncompressed * target_coefficient;

            //
            // Encode single image with limitation by bandwidth (encoded to original datasize ratio in 0.0 - 1.0)
            //
            //bytes = lossy_bw_limit(frame, size_compressed_limit); // returns JPG compressed stream for single image
            

            //
            // TASK 1: Replace function lossy_bw_limit() - limitation by bandwith - with limitation by quality.
            //         Implement the function:
            bytes = lossy_quality_limit(frame, target_coefficient);
            // 
            //         Use PSNR (Peak Signal to Noise Ratio)
            //         or  SSIM (Structural Similarity) 
            // (from https://docs.opencv.org/2.4/doc/tutorials/highgui/video-input-psnr-ssim/video-input-psnr-ssim.html#image-similarity-psnr-and-ssim ) 
            //         to estimate quality of the compressed image.
            //


            // display compression ratio
            auto size_compreessed = bytes.size();
            std::cout << "Size: uncompressed = " << size_uncompressed << ", compressed = " << size_compreessed << ", = " << size_compreessed / (size_uncompressed / 100.0) << " % \n";

            //
            // decode and display compressed and original data
            //  
            cv::Mat decoded_frame = cv::imdecode(bytes, cv::IMREAD_ANYCOLOR);

            cv::namedWindow("original");
            cv::imshow("original", frame);

            cv::namedWindow("decoded");
            cv::imshow("decoded", decoded_frame);

            // key handling
            int c = cv::pollKey();
            switch (c) {
            case 27:
                return EXIT_SUCCESS;
                break;
            case 'q':
                target_coefficient += 0.03f;
                break;
            case 'a':
                target_coefficient -= 0.03f;
                break;
            default:
                break;
            }

            target_coefficient = std::clamp(target_coefficient, 0.01f, 1.0f);
            std::cout << "Target PSNR: " << target_coefficient * MAX_PSNR << " dB\n";
        }
    }
    catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Finished OK...\n";
    return EXIT_SUCCESS;
}

#endif // !SKIP_LABS_COMPILATION
