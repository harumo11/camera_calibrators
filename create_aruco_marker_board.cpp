#include <iostream>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>
#include <sstream>
#include <string>

int main(void) {
    cv::Mat marker_image;
    cv::Mat board_image;

    int marker_id = 2;
    int padding = 150;
    cv::Size A4(2100, 2970);
    int marker_pix_size = A4.width - 2 * padding;
    std::cout << A4.width << std::endl;
    std::cout << marker_pix_size << std::endl;

    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    cv::aruco::generateImageMarker(dictionary, marker_id, marker_pix_size, marker_image);

    board_image.create(cv::Size(2100, 2970), CV_8UC1);
    board_image.setTo(cv::Scalar(255));

    cv::Rect roi((board_image.cols - marker_image.cols) / 2, (board_image.rows - padding - marker_pix_size),
                 marker_image.cols, marker_image.rows);

    marker_image.copyTo(board_image(roi));

    std::stringstream text;
    text << "DICT_4X4_50 | Size: " << marker_pix_size * 0.1 << " mm";
    std::cout << board_image.rows << " " << padding << std::endl;
    cv::Point2f text_anker(cv::Size(padding, board_image.rows - padding + 100));
    cv::putText(board_image, text.str(), text_anker, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 0), cv::LINE_4);

    cv::Size number_size = cv::getTextSize(std::to_string(marker_id), cv::FONT_HERSHEY_DUPLEX, 15, 1, 0);
    cv::Point2f number_anker(cv::Size((board_image.cols - number_size.width) / 2, number_size.height + 200));
    cv::putText(board_image, std::to_string(marker_id), number_anker, cv::FONT_HERSHEY_DUPLEX, 15, cv::Scalar(0, 0, 0),
                20, cv::LINE_AA);

    cv::namedWindow("board", cv::WINDOW_NORMAL);
    cv::resizeWindow("board", cv::Size(640, 480));
    cv::imshow("board", board_image);
    cv::waitKey(0);
    cv::imwrite("aruco_marker_board_1.png", board_image);
    cv::destroyAllWindows();

    std::cout << "You need to convert to PDF from PNG, otherwise, the board image little bit shrink. Use "
                 "https://png2pdf.com/ja/."
              << std::endl;

    return 0;
}
