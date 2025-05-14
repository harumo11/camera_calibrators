#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect/aruco_board.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/objdetect/aruco_dictionary.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>
#include <opencv2/videoio.hpp>
#include <vector>

int main(void) {
    double squared_length = 200;
    double marker_length = 130;
    auto dictionary =
        cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    cv::aruco::CharucoBoard diamond_board(cv::Size(3, 3), squared_length,
                                          marker_length, dictionary);
    cv::aruco::DetectorParameters detect_parameters;
    detect_parameters.cornerRefinementMethod = 1;
    cv::aruco::CharucoDetector detector(
        diamond_board, cv::aruco::CharucoParameters(), detect_parameters);

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "No camera is opend" << std::endl;
        return -1;
    }

    while (true) {
        cv::UMat frame_src;
        cv::UMat frame_output;
        cap >> frame_src;

        std::vector<int> marker_ids;
        std::vector<cv::Vec4i> diamond_ids;
        std::vector<std::vector<cv::Point2f>> marker_corners, diamond_corners;
        std::vector<cv::Vec3d> rvecs, tvecs;

        detector.detectDiamonds(frame_src, diamond_corners, diamond_ids,
                                marker_corners, marker_ids);

        // Draw detected markers
        if (diamond_ids.size() > 0) {
            cv::aruco::drawDetectedDiamonds(frame_src, diamond_corners,
                                            diamond_ids);
        }

        cv::namedWindow("output", cv::WINDOW_FULLSCREEN);
        cv::imshow("output", frame_src);
        cv::waitKey(27);
    }

    return 0;
}
