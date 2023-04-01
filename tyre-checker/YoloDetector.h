#pragma once
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <cmath>
#include <chrono>
#include <tuple>
#include <fstream>


enum class Position {
    top,
    bottom
};

/*
    Struct used to store the bounding box in yolo notation
*/
struct YoloCoords {
    float x;            // x of the center relative to image width
    float y;            // y of the center relative to image height
    float w;            // width of the bounding box relative to image width
    float h;            // height of the bounding box relative to image height
};

/*
    Struct used to store neural network configuration data
*/
struct YoloConfig {
    std::string cfg_path;       // path to .cfg file
    std::string weights_path;   // path to .weights file
    std::string names_path;     // path to .names file
    cv::Size input_size;        // size of neural network input
    float conf_thresh;          // confidence threshold
    float nms_thresh;           // nms threshold
};

/*
    Struct used to store detection data
*/
struct Detection {
    unsigned int class_id;
    std::string class_name;
    float conf;                 // detection confidence
    YoloCoords yolo;            // bounding box in yolo notation
    cv::Rect bbox;              // bounding box in pixels
};

/*
    A class that implements the functionality related to inference of a neural network used to detect objects.
*/
class YoloDetector
{
public:

    cv::dnn::Net net;                           // neural network instance
    cv::Size input_size;
    std::vector<std::string> output_layers;
    std::vector<std::string> class_names;
    float conf_thresh;                          // confidence threshold
    float nms_thresh;                           // non-maximum suppresion threshold


    // constructor, initializes neural network with given configuration struct
    YoloDetector(YoloConfig config);

    // performs object detection
    std::vector<Detection>detect(cv::Mat& img);

    // draws bounding boxes of detected objects
    void drawDetections(cv::Mat& img, std::vector<Detection> detections, cv::Size img_size, cv::Scalar color, Position position, bool show_conf);

    // converts bounding box from yolo notation to pixels, 
    cv::Rect yoloToBbox(YoloCoords yolo, cv::Size img_size);

private:

    // filters detection using confidence threshold and non-maximum suppresion
    std::vector<Detection> filterDetections(const std::vector<cv::Mat>& outputs, float conf_thresh, float nms_thresh);

    // loads class names from a text file
    std::vector<std::string> loadClassNames(std::string names_path);

};

