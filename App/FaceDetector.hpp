#pragma once

#include <opencv2/opencv.hpp>

class FaceDetector {
public:
	FaceDetector() { }

	bool load_classifier(const cv::String& filename) {
		return m_classifier.load(filename);
	}

	std::vector<cv::Point2f> find_faces(cv::Mat& frame) {
        cv::Mat frame_grayscale;
        cv::cvtColor(frame, frame_grayscale, cv::COLOR_BGR2GRAY);

        std::vector<cv::Rect> detected_rectangles;
        m_classifier.detectMultiScale(frame_grayscale, detected_rectangles);

        std::vector<cv::Point2f> detected_centers;
        detected_centers.reserve(detected_rectangles.size());

        for (const auto& rect : detected_rectangles) {
            detected_centers.push_back(
                cv::Point2f(
                    (rect.x + rect.width / 2.0f) / frame.cols,
                    (rect.y + rect.height / 2.0f) / frame.rows
                )
            );
        }

        return detected_centers;
	}

private:
	cv::CascadeClassifier m_classifier;

};
