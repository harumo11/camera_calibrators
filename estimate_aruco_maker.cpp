#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <utility>

std::tuple<cv::Mat, cv::Mat, cv::Size> read_calibration(const std::string &filename) {
    cv::Mat camera_matrix, dist_coeffs;
    cv::Size image_size;
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cout << "Failed to open calibration file. See you!" << std::endl;
        std::exit(-1);
    }
    fs["camera_matrix"] >> camera_matrix;
    fs["dist_coeffs"] >> dist_coeffs;
    fs["image_size"] >> image_size;
    fs.release();

    std::cout << "Camera matrix: " << camera_matrix << std::endl;
    std::cout << "Distortion coefficients: " << dist_coeffs << std::endl;
    std::cout << "Image size: " << image_size << std::endl;

    return {camera_matrix, dist_coeffs, image_size};
}

int main(void) {
    // read calibration parameters
    auto [camera_matrix, dist_coeffs, image_size] = read_calibration("asm_circle_camera_calibration_realsense.yml");

    auto dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    std::vector<int> marker_ids;
    std::vector<std::vector<cv::Point2f>> marker_corners;
    std::vector<std::vector<cv::Point2f>> rejected_candidates;
    cv::aruco::DetectorParameters parameters;
    parameters.cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX;
    cv::aruco::ArucoDetector detector(dictionary, parameters);
    double marker_length = 0.18; // Length of the marker in meters

    // open camera
    cv::VideoCapture cap(8);
    if (!cap.isOpened()) {
        std::cout << "No camera is opened" << std::endl;
        return -1;
    }

    cv::Mat frame_src;
    cv::Mat frame_output;
    while (true) {
        cap >> frame_src;
        frame_output = frame_src.clone();
        cv::imshow("Camera", frame_src);

        detector.detectMarkers(frame_src, marker_corners, marker_ids, rejected_candidates);

        if (marker_ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(frame_output, marker_corners, marker_ids);

            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(marker_corners, marker_length, camera_matrix, dist_coeffs, rvecs,
                                                 tvecs);
            std::cout << tvecs[0] << std::endl;

            for (size_t i = 0; i < marker_ids.size(); ++i) {
                cv::drawFrameAxes(frame_output, camera_matrix, dist_coeffs, rvecs[i], tvecs[i], 0.1);
            }
            cv::imshow("result", frame_output);
        }

        cv::waitKey(27);
    }

    return 0;
}
