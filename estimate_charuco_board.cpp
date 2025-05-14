#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>
#include <utility>

// read camera paramters from yaml
std::tuple<cv::Mat, cv::Mat>
read_calibration_file(const std::string &filename) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    cv::Mat camera_matrix, dist_coeffs;

    fs["camera_matrix"] >> camera_matrix;
    fs["dist_coeffs"] >> dist_coeffs;

    return {camera_matrix, dist_coeffs};
}

int main() {

    // load camera parameters
    std::cout << "Loading camera parameters..." << std::endl;
    auto [camera_matrix, dist_coeffs] =
        read_calibration_file("chess_camera_calibration.yml");

    // config
    cv::aruco::DetectorParameters detector_parmas;
    detector_parmas.cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX;
    cv::aruco::Dictionary dictionary =
        cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    cv::aruco::CharucoParameters charuco_parameters;
    charuco_parameters.tryRefineMarkers = true;
    charuco_parameters.cameraMatrix = camera_matrix;
    charuco_parameters.distCoeffs = dist_coeffs;

    // create charuco board
    int squares_x = 4;
    int squares_y = 5;
    float square_length = 0.044; // 4.4cm
    float marker_length = 0.029; // 2.9cm

    cv::aruco::CharucoBoard board(cv::Size(squares_x, squares_y), square_length,
                                  marker_length, dictionary);

    // create charuco detector
    cv::aruco::CharucoDetector detector(board, charuco_parameters,
                                        detector_parmas);

    // open camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "No camera is opened" << std::endl;
        return -1;
    }
    cv::Mat frame_src;
    cv::Mat frame_output;

    while (true) {
        cap >> frame_src;

        std::vector<int> marker_ids;
        std::vector<int> charuco_ids;
        std::vector<std::vector<cv::Point2f>> marker_corners;
        std::vector<cv::Point2f> charuco_corners;
        cv::Vec3d rvec;
        cv::Vec3d tvec;

        detector.detectBoard(frame_src, charuco_corners, charuco_ids,
                             marker_corners, marker_ids);

        bool valid_pose = false;
        if (camera_matrix.total() != 0 && dist_coeffs.total() != 0 &&
            charuco_ids.size() >= 4) {
            cv::Mat object_points;
            cv::Mat image_points;
            board.matchImagePoints(charuco_corners, charuco_ids, object_points,
                                   image_points);
            try {
                valid_pose =
                    cv::solvePnP(object_points, image_points, camera_matrix,
                                 dist_coeffs, rvec, tvec);
            } catch (const std::exception &e) {
                // std::cerr << e.what() << '\n';
                std::cout << "Could not solve PnP" << std::endl;
            }
        }

        // draw detected markers
        frame_src.copyTo(frame_output);
        if (!marker_ids.empty()) {
            cv::aruco::drawDetectedMarkers(frame_output, marker_corners,
                                           marker_ids);
        }

        if (!charuco_ids.empty()) {
            cv::aruco::drawDetectedCornersCharuco(frame_output, charuco_corners,
                                                  charuco_ids,
                                                  cv::Scalar(255, 0, 0));
        }

        if (valid_pose) {
            cv::drawFrameAxes(frame_output, camera_matrix, dist_coeffs, rvec,
                              tvec, 0.1);
        }

        cv::imshow("Camera", frame_output);
        cv::waitKey(27);
    }
    return 0;
}
