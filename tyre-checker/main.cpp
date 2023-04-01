#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <cmath>
#include <chrono>
#include <tuple>
#include <fstream>
#include "YoloDetector.h"
#include "TyreChecker.h"


int main() {

    std::string textfields_cfg_path = "C:/Users/mateu/Desktop/projekty/tyre-checker/yolo_files/yolov4-tiny-text-fields-1216-192.cfg";
    std::string textfields_weights_path = "C:/Users/mateu/Desktop/projekty/tyre-checker/yolo_files/yolov4-tiny-text-fields-1216-192_10000.weights";
    std::string textfields_names_path = "C:/Users/mateu/Desktop/projekty/tyre-checker/yolo_files/obj_text_fields.names";

    std::string characters_cfg_path = "C:/Users/mateu/Desktop/projekty/tyre-checker/yolo_files/yolov4-tiny-characters-96-64.cfg";
    std::string characters_weights_path = "C:/Users/mateu/Desktop/projekty/tyre-checker/yolo_files/yolov4-tiny-characters-96-64_7000.weights";
    std::string characters_names_path = "C:/Users/mateu/Desktop/projekty/tyre-checker/yolo_files/obj_characters.names";

    YoloConfig textfields_detector_config;
    YoloConfig characters_detector_config;

    textfields_detector_config.cfg_path = textfields_cfg_path;
    textfields_detector_config.weights_path = textfields_weights_path;
    textfields_detector_config.names_path = textfields_names_path;
    textfields_detector_config.input_size = cv::Size(1216, 192);
    textfields_detector_config.conf_thresh = 0.5;
    textfields_detector_config.nms_thresh = 0.5;

    characters_detector_config.cfg_path = characters_cfg_path;
    characters_detector_config.weights_path = characters_weights_path;
    characters_detector_config.names_path = characters_names_path;
    characters_detector_config.input_size = cv::Size(96, 64);
    characters_detector_config.conf_thresh = 0.5;
    characters_detector_config.nms_thresh = 0.5;


    // YoloConfig textfields_detector_config(textfields_cfg_path, textfields_weights_path, textfields_names_path, cv::Size(1216, 192)); // this caused internal compiler error C1001
    // YoloConfig characters_detector_config(characters_cfg_path, characters_weights_path, characters_names_path, cv::Size(96, 64)); // this caused internal compiler error C1001

    TyreChecker tyre_checker(textfields_detector_config, characters_detector_config);

    std::string img_path = "C:/Users/mateu/Desktop/projekty/tyre-checker/data/images/107_3104.jpg";
    cv::Mat tyre_img = cv::imread(img_path);

    auto [tyre_info, ret_img, inference_times] = tyre_checker.checkTyre(tyre_img);

    cv::imwrite("C:/Users/mateu/Desktop/projekty/tyre-checker/data/images/107_3104_detections.jpg", ret_img);
    tyre_info.printInfo();
    inference_times.printTimes();
}
