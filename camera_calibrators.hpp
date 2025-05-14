#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/types.hpp>
#include <sstream>
#include <vector>

class camera_calibrators {
  public:
    camera_calibrators() = default;
    ~camera_calibrators() = default;
    camera_calibrators(const uint row_square_num, const uint col_square_num, const double length,
                       std::string pattern_type = "chess");
    camera_calibrators(const uint row_square_num, const uint col_square_num, const double square_length,
                       const double marker_length, std::string pattern_type = "charuco");

    void set_chess_config(const uint row_square_num, const uint col_square_num, const double square_length);
    void set_asymmetric_circle_config(const uint row_square_num, const uint square_num, const double diagonal_spacing);
    void set_charuco_config(const uint row_square_num, const uint col_square_num, const double square_length,
                            const double marker_length);

    // store the image to calibrate
    bool add_calibration_image(const cv::Mat &image);
    std::tuple<double, cv::Mat, cv::Mat> calibrate();
    bool save(const std::string &filename);
    std::tuple<bool, std::vector<cv::Point2f>, cv::Mat> detect_board(const cv::Mat &image);

  private:
    double square_length;
    int row_square_num;
    int col_square_num;
    std::string pattern_type;
    std::vector<cv::Mat> calibration_images;
    std::vector<std::vector<cv::Point2f>> detected_all_corners;

    std::tuple<bool, std::vector<cv::Point2f>, cv::Mat> detect_chess_board(const cv::Mat &image);
    std::tuple<bool, std::vector<cv::Point2f>, cv::Mat> detect_asymmetric_circle_board(const cv::Mat &image);
    std::tuple<bool, std::vector<cv::Point2f>, cv::Mat> detect_charuco_board(const cv::Mat &image);

    std::vector<std::vector<cv::Point3f>> get_chessboard_corner_positions(const cv::Size &board_size,
                                                                          double square_size, int num_of_images);
    std::vector<std::vector<cv::Point3f>> get_asymmetric_circle_corner_positions(const cv::Size &board_size,
                                                                                 double square_size, int num_of_images);
};
