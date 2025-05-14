#include "camera_calibrators.hpp"
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

camera_calibrators::camera_calibrators(const uint row_square_num, const uint col_square_num, const double length,
                                       std::string pattern_type) {
    if (pattern_type == "chess") {
        // call set_chess_config
        this->set_chess_config(row_square_num, col_square_num, length);
    } else if (pattern_type == "asymmetric_circle") {
        // call set_asymmetric_circle_config
    } else {
        std::cout << "||| Error Unsupported pattern type : " << pattern_type << " is specified." << std::endl;
        std::exit(-1);
    }
}

camera_calibrators::camera_calibrators(const uint row_square_num, const uint col_square_num, const double square_length,
                                       const double marker_length, std::string pattern_type) {
    // call set_charuco_config
}

void camera_calibrators::set_chess_config(const uint row_square_num, const uint col_square_num,
                                          const double square_length) {
    this->pattern_type = "chess";
    this->row_square_num = row_square_num;
    this->col_square_num = col_square_num;
    this->square_length = square_length;
}

void camera_calibrators::set_asymmetric_circle_config(const uint row_square_num, const uint square_num,
                                                      const double diagonal_spacing) {
    this->pattern_type = "asymmetric_circle";
}

void camera_calibrators::set_charuco_config(const uint row_square_num, const uint col_square_num,
                                            const double square_length, const double marker_length) {
    this->pattern_type = "charuco";
}

bool camera_calibrators::add_calibration_image(const cv::Mat &image) {
    this->calibration_images.push_back(image);
    return true;
}

std::tuple<bool, std::vector<cv::Point2f>, cv::Mat> camera_calibrators::detect_board(const cv::Mat &image) {
    if (this->pattern_type == "chess") {
        return this->detect_chess_board(image);
    } else if (this->pattern_type == "asymmetric_circle") {
        return this->detect_asymmetric_circle_board(image);
    } else if (this->pattern_type == "charuco") {
        return this->detect_charuco_board(image);
    } else {
        bool found = false;
        std::vector<cv::Point2f> none_corners;
        cv::Mat none_image;
        return {found, none_corners, none_image};
    }
}

std::tuple<bool, std::vector<cv::Point2f>, cv::Mat> camera_calibrators::detect_chess_board(const cv::Mat &image) {
    std::vector<cv::Point2f> board_corners;
    cv::Size board_size(this->row_square_num, col_square_num);

    bool found = cv::findChessboardCorners(image, board_size, board_corners,
                                           cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK |
                                               cv::CALIB_CB_NORMALIZE_IMAGE);
    if (found) {
        cv::Mat frame_gray;
        cv::cvtColor(image, frame_gray, cv::COLOR_BGR2GRAY);
        cv::cornerSubPix(frame_gray, board_corners, cv::Size(11, 11), cv::Size(-1, -1),
                         cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.001));
    }

    cv::Mat output_frame;
    image.copyTo(output_frame);
    cv::drawChessboardCorners(output_frame, board_size, board_corners, found);

    return {found, board_corners, output_frame};
}

std::tuple<bool, std::vector<cv::Point2f>, cv::Mat>
camera_calibrators::detect_asymmetric_circle_board(const cv::Mat &image) {
    std::vector<cv::Point2f> board_corners;
    cv::Size board_size(this->row_square_num, this->col_square_num);

    bool found =
        cv::findCirclesGrid(image, board_size, board_corners, cv::CALIB_CB_ASYMMETRIC_GRID | cv::CALIB_CB_CLUSTERING);

    cv::Mat output_frame;
    image.copyTo(output_frame);
    cv::drawChessboardCorners(image, board_size, board_corners, found);

    return {found, board_corners, output_frame};
}

std::tuple<bool, std::vector<cv::Point2f>, cv::Mat> camera_calibrators::detect_charuco_board(const cv::Mat &image) {
    cv::Mat output_frame;
    bool found = true;
    std::vector<cv::Point2f> corners;
    return {found, corners, output_frame};
}

std::tuple<double, cv::Mat, cv::Mat> camera_calibrators::calibrate() {
    // check saved image to find board corners
    for (auto &&image : this->calibration_images) {
        if (this->pattern_type == "chess") {
            auto [found, corners, output_image] = this->detect_chess_board(image);
            if (found) {
                this->detected_all_corners.push_back(corners);
            }

        } else if (this->pattern_type == "asymmetric_circle") {
            // get corners
        } else if (this->pattern_type == "charuco") {
            // get corners
        } else {
            // an error
        }
    }

    double error = 0;
    cv::Mat camera_matrix, dist_coeff;
    if (this->pattern_type == "chess") {
        cv::Size board_size(this->row_square_num, col_square_num);
        auto object_points =
            this->get_chessboard_corner_positions(board_size, this->square_length, this->detected_all_corners.size());

        std::vector<cv::Mat> rvec, tvec;
        error = cv::calibrateCamera(object_points, this->detected_all_corners, this->calibration_images.front().size(),
                                    camera_matrix, dist_coeff, rvec, tvec);
    } else if (this->pattern_type == "asymmetric_circle") {
        // do calibrate
    } else if (this->pattern_type == "charuco") {
        // do calibrate
    } else {
        // an errro
        error = -1;
    }

    return {error, camera_matrix, dist_coeff};
}

std::vector<std::vector<cv::Point3f>>
camera_calibrators::get_chessboard_corner_positions(const cv::Size &board_size, double square_size, int num_of_images) {

    // corners in one board
    std::vector<cv::Point3f> points;
    for (int i = 0; i < board_size.height; i++) {
        for (int j = 0; j < board_size.width; j++) {
            points.push_back(cv::Point3f(j * square_size, i * square_size, 0));
        }
    }

    // corners in all board
    std::vector<std::vector<cv::Point3f>> object_points;
    for (int i = 0; i < num_of_images; i++) {
        object_points.push_back(points);
    }

    return object_points;
}

std::vector<std::vector<cv::Point3f>>
camera_calibrators::get_asymmetric_circle_corner_positions(const cv::Size &board_size, double square_size,
                                                           int num_of_images) {
    // a vector of 3D points
    std::vector<cv::Point3f> points;
    for (int i = 0; i < board_size.height; i++) {
        for (int j = 0; j < board_size.width; j++) {
            points.push_back(cv::Point3f((2 * j + i % 2) * square_size, i * square_size, 0));
        }
    }

    std::vector<std::vector<cv::Point3f>> object_points;
    for (int i = 0; i < num_of_images; i++) {
        object_points.push_back(points);
    }

    return object_points;
}
