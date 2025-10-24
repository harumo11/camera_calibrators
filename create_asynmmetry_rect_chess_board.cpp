// this code generates a chessboard and above of the chessboard image,
// the number is printed.
#include <iostream>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>
#include <string>

int main(void) {
    std::cout << "Creating a chessboard image using OpenCV" << std::endl;
    int output_width = 2100;                                    // A4 width in pixels (210mm), 10 pixels per mm
    int output_height = 2970;                                   // A4 height in pixels (297mm), 10 pixels per mm
    cv::Mat output_image(output_height, output_width, CV_8UC3); // A4 paper size
    cv::Size square_dim(5, 6);                                  // col x row
    int square_size_w = 200;                                    // size of each square in pixels, 10 pixels is 1 mm
    int square_size_h = 165;                                    // size of each square in pixels, 10 pixels is 1 mm
    std::string title = "";

    // output image size
    output_image.setTo(cv::Scalar(255, 255, 255)); // white background

    // A4 paper is splitted as 2:8. 2 is drawing number space, 8 is chaess board space.
    // |--------------|
    // |      2       |
    // | drawing num  |
    // |--------------|
    // |              |
    // |              |
    // |              |
    // |      8       |
    // | chess board  |
    // |              |
    // |--------------|
    int number_drawn_space_y_len = output_height * 0.2;      // 593 pixel
    int half_of_number_space = number_drawn_space_y_len / 2; // 297
    int chess_drawn_space_y_len = output_height * 0.8;
    int half_of_chess_space = chess_drawn_space_y_len / 2;

    // draw number
    cv::Size number_pixel_size = cv::getTextSize(title, cv::FONT_HERSHEY_DUPLEX, 15.0, 1, 0);
    cv::Point2f number_ankter(
        cv::Size((output_image.cols - number_pixel_size.width) / 2, number_pixel_size.height + 200));
    cv::putText(output_image, title, number_ankter, cv::FONT_HERSHEY_DUPLEX, 15, cv::Scalar(0, 0, 0), 20, cv::LINE_AA);

    // create chessboard image
    int board_height = square_dim.height * square_size_h;
    int board_width = square_dim.width * square_size_w;
    cv::Mat board_image(board_height, board_width, CV_8UC3); // rows, cols
    board_image.setTo(cv::Scalar(255, 255, 255));            // white background
    for (int i = 0; i < square_dim.height; i++) {
        for (int j = 0; j < square_dim.width; j++) {
            if ((i + j) % 2 == 0) {
                cv::rectangle(board_image, cv::Point(j * square_size_w, i * square_size_h),
                              cv::Point((j + 1) * square_size_w, (i + 1) * square_size_h), cv::Scalar(0, 0, 0), -1);
            }
        }
    }
    // show
    cv::namedWindow("Board Image", cv::WINDOW_NORMAL);
    cv::resizeWindow("Board Image", cv::Size(500, 500));
    cv::imshow("Board Image", board_image);
    cv::waitKey(0);
    // save
    cv::imwrite("chessboard_image_processing.png", board_image);

    // copy chessboard image into output image
    std::cout << "output image cols " << output_image.cols << " rows " << output_image.rows << std::endl;
    std::cout << "board  image cols " << board_image.cols << " rows " << board_image.rows << std::endl;
    std::cout << "number drawing area height " << number_drawn_space_y_len << std::endl;

    int chess_drawn_space_center_height = number_drawn_space_y_len + half_of_chess_space;
    int board_top_y_coord = chess_drawn_space_center_height - board_image.rows / 2;
    std::cout << "board top y coord " << board_top_y_coord << std::endl;
    cv::Rect roi((output_image.cols - board_image.cols) / 2, board_top_y_coord, board_image.cols, board_image.rows);
    board_image.copyTo(output_image(roi));

    // text
    std::stringstream text;
    auto square_dim_title = square_dim;
    square_dim_title.width -= 1;
    square_dim_title.height -= 1;
    text << square_dim_title << " Checker Size: " << "height : " << square_size_h * 0.1 << " mm"
         << " width : " << square_size_w * 0.1 << " mm";
    int padding = 175;
    cv::Point2f text_anker(cv::Size(padding, output_image.rows - padding + 100));
    cv::putText(output_image, text.str(), text_anker, cv::FONT_HERSHEY_DUPLEX, 2, cv::Scalar(0, 0, 0), 3);

    //// show image
    cv::namedWindow("Chessboard", cv::WINDOW_NORMAL);
    // cv::resize(board_image, board_image, cv::Size(500, 500));
    cv::resizeWindow("Chessboard", cv::Size(210 * 3, 297 * 3));
    cv::imshow("Chessboard", output_image);
    cv::waitKey(0);

    // save
    cv::imwrite("chess_board.png", output_image);
}
