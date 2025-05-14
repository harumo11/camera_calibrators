#include "camera_calibrators.hpp"
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

int main(void) {
    cv::VideoCapture cap(0);

    if (!cap.isOpened()) {
        std::cout << "No camera is opened" << std::endl;
        return -1;
    }

    cv::Mat frame_src;
    cv::Mat frame_output;

    camera_calibrators ccalibs(3, 4, 42.0, "chess");

    while (true) {
        cap >> frame_src;
        cv::imshow("input", frame_src);

        auto [found, corners, output_img] = ccalibs.detect_board(frame_src);
        cv::imshow("output", output_img);
        cv::waitKey(1);
    }
}
