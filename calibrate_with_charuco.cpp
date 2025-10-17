// This program do calibration with a ChArUco board.
// The program uses OpenCV library for image processing and camera calibration.

#include <chrono>
#include <iomanip>
#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/objdetect/aruco_dictionary.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>
#include <string>
#include <vector>

void log(std::string message) {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << std::put_time(std::localtime(&now_c), "%F %T") << " " << message << std::endl;
}

int main(void) {
    // Create charuco board object and charuco detector
    cv::Size image_size;
    double square_length = 0.044; // Length of the square in meters
    double marker_length = 0.029; // Length of the marker in meters
    std::vector<cv::Mat> all_images;
    std::vector<cv::Mat> all_charuco_ids;
    std::vector<cv::Mat> all_charuco_corners;
    cv::aruco::CharucoParameters charuco_parameters;
    cv::aruco::DetectorParameters detect_parameters;
    std::vector<std::vector<cv::Point2f>> all_image_points;
    std::vector<std::vector<cv::Point3f>> all_object_points;

    // aruco dictionary
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    // charuco board
    cv::aruco::CharucoBoard board(cv::Size(4, 5), square_length, marker_length, dictionary);
    // charuco board detector
    charuco_parameters.tryRefineMarkers = true;
    detect_parameters.cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX;
    cv::aruco::CharucoDetector detector(board, charuco_parameters, detect_parameters);

    // Open camera
    // cv::VideoCapture cap(0);
    cv::VideoCapture cap(6);
    if (!cap.isOpened()) {
        std::cout << "No camera is opened" << std::endl;
        return -1;
    }

    while (true) {
        cv::Mat frame_src;
        cv::Mat frame_output;
        cap >> frame_src;

        cv::imshow("Camera", frame_src);

        cv::Mat marker_ids;
        cv::Mat current_charuco_ids;
        cv::Mat current_charuco_corners;
        std::vector<cv::Point3f> current_object_points;
        std::vector<cv::Point2f> current_image_points;
        std::vector<std::vector<cv::Point2f>> marker_corners;

        // Detect charuco corners
        detector.detectBoard(frame_src, current_charuco_corners, current_charuco_ids, marker_corners, marker_ids);

        frame_src.copyTo(frame_output);
        if (!current_charuco_ids.empty()) {
            cv::aruco::drawDetectedMarkers(frame_output, marker_corners, marker_ids);
        }
        if (current_charuco_corners.total() > 3) {
            cv::aruco::drawDetectedCornersCharuco(frame_output, current_charuco_corners, current_charuco_ids);
        }

        cv::putText(frame_output, "Press 'c' to capture or ESC to finish", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX,
                    0.5, cv::Scalar(0, 255, 0), 1);

        cv::imshow("Charuco corners", frame_output);
        char key = (char)cv::waitKey(1);
        if (key == 27) {
            log("ESC key pressed, start to calculating camera matrix");
            break;
        }

        if (key == 'c' && current_charuco_corners.total() > 3) {
            board.matchImagePoints(current_charuco_corners, current_charuco_ids, current_object_points,
                                   current_image_points);
            if (current_image_points.empty() || current_object_points.empty()) {
                log("Points matching failed, try again");
                continue;
            }

            log("Captured image points");
            all_charuco_corners.push_back(current_charuco_corners);
            all_charuco_ids.push_back(current_charuco_ids);
            all_image_points.push_back(current_image_points);
            all_object_points.push_back(current_object_points);
            all_images.push_back(frame_src);

            image_size = frame_src.size();
        } else {
            log("current charuco corners: " + std::to_string(current_charuco_corners.total()));
        }
    }

    if (all_charuco_corners.size() < 5) {
        log("Not enough images captured, exiting");
        log("Captured images: " + std::to_string(all_charuco_corners.size()));
        return -1;
    }

    cv::Mat camera_matrix, dist_coeffs;
    cv::Mat rvecs, tvecs;
    double reprojection_errror =
        cv::calibrateCamera(all_object_points, all_image_points, image_size, camera_matrix, dist_coeffs, rvecs, tvecs);
    log("Calibration done");
    std::cout << "Reprojection error: " << reprojection_errror << std::endl;
    std::cout << "Camera matrix: " << camera_matrix << std::endl;
    std::cout << "Distortion coefficients: " << dist_coeffs << std::endl;

    // Save calibration results
    cv::FileStorage fs("charuco_camera_calibration.yml", cv::FileStorage::WRITE);
    fs << "camera_matrix" << camera_matrix;
    fs << "dist_coeffs" << dist_coeffs;
    fs << "image_size" << image_size;
    fs.release();

    log("Calibration results saved to calibration.yml");

    return 0;
}
