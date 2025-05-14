#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/persistence.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

std::vector<std::vector<cv::Point3f>> calc_board_corner_positions(const cv::Size &board_size, float square_size,
                                                                  int num_of_images) {
    // a vector of 3D points
    std::vector<cv::Point3f> points;
    for (int i = 0; i < board_size.height; i++) {
        for (int j = 0; j < board_size.width; j++) {
            points.push_back(cv::Point3f(j * square_size, i * square_size, 0));
        }
    }

    std::vector<std::vector<cv::Point3f>> object_points;
    for (int i = 0; i < num_of_images; i++) {
        object_points.push_back(points);
    }

    return object_points;
}

int main() {
    // config
    cv::Size board_size(3, 4);
    int square_size = 42; // mm

    // open camera
    cv::VideoCapture cap(0);

    if (!cap.isOpened()) {
        std::cout << "No camera is opened" << std::endl;
        return -1;
    }
    cv::Mat frame_src;
    cv::Mat frame_output;
    std::vector<std::vector<cv::Point2f>> all_image_corners;

    while (true) {
        cap >> frame_src;
        frame_output = frame_src.clone();

        // find chessboard corners
        std::vector<cv::Point2f> board_corners;
        bool found = cv::findChessboardCorners(frame_src, board_size, board_corners,
                                               cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK |
                                                   cv::CALIB_CB_NORMALIZE_IMAGE);
        if (found) {
            cv::Mat frame_gray;
            cv::cvtColor(frame_src, frame_gray, cv::COLOR_BGR2GRAY);
            cv::cornerSubPix(frame_gray, board_corners, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.001));
            cv::drawChessboardCorners(frame_output, board_size, board_corners, found);
            cv::putText(frame_output, "Press 'c' to save corners", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7,
                        cv::Scalar(0, 255, 0), 2);

            // if c key is pressed, save the corners
            if (cv::waitKey(1) == 'c') {
                all_image_corners.push_back(board_corners);
                std::cout << "Corners saved" << std::endl;
            }
        }

        char key = (char)cv::waitKey(1);
        if (key == 27) {
            std::cout << "ESC key is pressed. Starting to calculate camera matrix." << std::endl;
            break;
        }

        cv::imshow("Camera", frame_src);
        cv::imshow("output", frame_output);
    }

    // calculating camera matrix
    cv::Mat camera_matrix, dist_coeff;
    std::vector<cv::Mat> rvec, tvec;
    auto object_points = calc_board_corner_positions(board_size, square_size, all_image_corners.size());
    double rms =
        cv::calibrateCamera(object_points, all_image_corners, frame_src.size(), camera_matrix, dist_coeff, rvec, tvec);

    std::cout << "camera matrix : " << camera_matrix << std::endl;
    std::cout << "dist_coeff : " << dist_coeff << std::endl;
    std::cout << "error : " << rms << std::endl;

    // save the camera matrix
    cv::FileStorage fs("chess_camera_calibration.yml", cv::FileStorage::WRITE);
    if (fs.isOpened()) {
        cv::write(fs, "camera_matrix", camera_matrix);
        cv::write(fs, "dist_coeffs", dist_coeff);
        cv::write(fs, "image_size", frame_src.size());
        std::cout << "Calibration results is saved to chess_camera_calibration.yml" << std::endl;
    }

    return 0;
}
