#include <iostream>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/objdetect/aruco_dictionary.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>
#include <sstream>

// refered to
// https://github.com/opencv/opencv/blob/4.x/samples/cpp/tutorial_code/objectDetection/create_diamond.cpp
int main(void) {
    double squared_length = 200; // square side length (in pixels)
    double marker_length = 130;  // marker side length (in pixels)
    int margin = 0;              // margin size (in pixels)
    int border_bits = 1;         // number of bits in marker boarders
    std::string titile = "res.png";
    std::vector<int> ids = {0, 1, 2, 3};
    auto dictionary =
        cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    cv::aruco::CharucoBoard board(cv::Size(3, 3), squared_length, marker_length,
                                  dictionary, ids);
    cv::Size image_size;
    image_size.height = 3 * squared_length + 2 * margin;
    image_size.width = 3 * squared_length + 2 * margin;

    cv::Mat board_image;
    board.generateImage(image_size, board_image, margin, border_bits);

    cv::imshow("board", board_image);
    cv::waitKey(0);

    cv::imwrite("diamond.png", board_image);
}
