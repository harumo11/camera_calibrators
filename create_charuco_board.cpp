#include <iostream>
#include <opencv4/opencv2/aruco.hpp>
#include <opencv4/opencv2/aruco/charuco.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/objdetect/aruco_board.hpp>
#include <opencv4/opencv2/objdetect/aruco_dictionary.hpp>
#include <opencv4/opencv2/objdetect/charuco_detector.hpp>

int main(void) {
    std::cout << "Creating a Charuco board" << std::endl;
    // config
    cv::Mat boardImage;
    cv::Size imageSize;
    int squaresX = 5;
    int squaresY = 7;
    float squareLength = 0.06;  // unit: meters
    double margins = 0.01;      // unit: meters
    double markerLength = 0.04; // unit: meters
    imageSize.width = squaresX * squareLength + 2 * margins;
    imageSize.height = squaresY * squareLength + 2 * margins;
    auto charucoboard = cv::aruco::CharucoBoard(
        cv::Size(squaresX, squaresY), squareLength, markerLength,
        cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50));
    charucoboard.generateImage(cv::Size(364 * 2, 257 * 2), boardImage);

    // display
    cv::imshow("Charuco board", boardImage);
    cv::waitKey(0);

    cv::imwrite("board.png", boardImage);
}
