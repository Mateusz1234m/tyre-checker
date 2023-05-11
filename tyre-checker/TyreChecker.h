#pragma once
#include"YoloDetector.h"
#include <cmath>
#include <chrono>
#include <tuple>
#include <fstream>
#include"TyreInfo.h"

#define PI 3.14159265358979323846

/*
    A class used to store inference times.
*/
class InferenceTimes {
public:

    std::chrono::milliseconds cartesian_to_polar_time = std::chrono::milliseconds(0);
    std::chrono::milliseconds textfields_inference_time = std::chrono::milliseconds(0);
    std::chrono::milliseconds characteres_inference_time = std::chrono::milliseconds(0);
    std::chrono::milliseconds total_time = std::chrono::milliseconds(0);

    // prints out inference times
    void printTimes();
};

/*
    A class that implements the functionality of checking tyre markings, it consists of 2 detectors, 
    one that detects textfields and their type and another that detects characters on text fields.
*/
class TyreChecker
{

public:

    YoloDetector* textfields_detector;
    YoloDetector* characters_detector;


    // constructor, initializes detectors based on configuration structs
    TyreChecker(YoloConfig textfields_detector_config, YoloConfig characters_detector_config);

    // destructor, deletes detectors
    ~TyreChecker();

    // returns tyre information based on given image
    std::tuple<TyreInfo, cv::Mat, InferenceTimes> checkTyre(cv::Mat img);

    // converts an image from cartesian coordinates to polar coordinates
    std::tuple<cv::Mat, std::chrono::milliseconds> cartesianToPolar(cv::Mat& img_cartesian);

private:

    // adds an equal (if possible) margin on each side of the bounding box
    cv::Rect expandBbox(cv::Rect bbox, int amount, int max_x, int max_y);

    // converts the bounding box specified for a text field to a global bounding box specified for the entire image
    std::vector<Detection> calculateGlobalDetections(std::vector<Detection> dets, Detection textfield, cv::Size global_size);

    // converts bounding box from pixels to yolo notation, 
    YoloCoords bboxToYolo(cv::Rect bbox, cv::Size img_size);

};

