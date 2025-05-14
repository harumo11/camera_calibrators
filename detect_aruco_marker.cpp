#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>
#include <vector>

int main(void) {
    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FPS, 60);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    if (!cap.isOpened()) {
        std::cout << "No camera is opened" << std::endl;
        return -1;
    }

    cv::Mat frame_src;

    while (true) {
        cap >> frame_src;

        std::vector<int> found_marker_ids;
        std::vector<std::vector<cv::Point2f>> marker_corners;
        auto dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
        cv::aruco::DetectorParameters detect_parameters;
        detect_parameters.cornerRefinementMethod = 2;
        cv::aruco::ArucoDetector detector(dict, detect_parameters);

        // detect markers
        detector.detectMarkers(frame_src, marker_corners, found_marker_ids);
        if (found_marker_ids.size() > 0) {
            std::cout << "Detected markers: ";
            for (size_t i = 0; i < found_marker_ids.size(); ++i) {
                std::cout << found_marker_ids[i] << " ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "No markers detected" << std::endl;
        }

        // draw detected markers
        cv::aruco::drawDetectedMarkers(frame_src, marker_corners,
                                       found_marker_ids);
        cv::namedWindow("Detected_Markers", cv::WINDOW_FREERATIO);
        cv::imshow("Detected_Markers", frame_src);
        cv::waitKey(27);
    }
    return 0;
}