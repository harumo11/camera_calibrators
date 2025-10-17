#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

int main(void) {
    for (int i = 0; i < 20; i++) {
        cv::VideoCapture cap(i);
        if (!cap.isOpened()) {
            std::cout << "id " << i << " is not avalilable." << std::endl;
        } else {
            std::cout << "-------------------------------" << std::endl;
            std::cout << "id " << i << " is available." << std::endl;
            std::cout << "-------------------------------" << std::endl;
            cv::Mat frame;
            cap >> frame;
            cv::imshow("image", frame);
            cv::waitKey(1000);
            cv::destroyAllWindows();
        }
    }
}
