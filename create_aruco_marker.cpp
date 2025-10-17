#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>

int main(void) {
    cv::Mat marker_image;
    int marker_id = 1;
    int marker_pix_size = 500; // pixel (500 x 500)
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    cv::aruco::generateImageMarker(dictionary, marker_id, marker_pix_size, marker_image);

    cv::imshow("marker", marker_image);
    cv::waitKey(0);
    cv::imwrite("marker.png", marker_image);
    cv::destroyAllWindows();

    return 0;
}
