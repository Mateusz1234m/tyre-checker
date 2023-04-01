#include "YoloDetector.h"

YoloDetector::YoloDetector(YoloConfig config) {

    this->input_size = config.input_size;
    this->net = cv::dnn::readNetFromDarknet(config.cfg_path, config.weights_path);
    this->net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    this->net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

    this->output_layers = net.getUnconnectedOutLayersNames();
    this->class_names = this->loadClassNames(config.names_path);

    this->conf_thresh = config.conf_thresh;
    this->nms_thresh = config.nms_thresh;
}


std::vector<Detection> YoloDetector::detect(cv::Mat& img) {

    // rescale, normalize and swap B and R channels
    cv::Mat blob = cv::dnn::blobFromImage(img, 1 / 255.0, this->input_size, cv::Scalar(0, 0, 0), true, false);

    // perform forward pass through the neural network
    this->net.setInput(blob);
    std::vector<cv::Mat> outputs;
    this->net.forward(outputs, this->net.getUnconnectedOutLayersNames());

    // filter detections
    std::vector<Detection> detections = this->filterDetections(outputs, this->conf_thresh, this->nms_thresh);

    return detections;
}


void YoloDetector::drawDetections(cv::Mat& img, std::vector<Detection> detections, cv::Size img_size, cv::Scalar color, Position position, bool show_conf) {
    for (int i = 0; i < detections.size(); i++) {

        cv::Rect bbox = yoloToBbox(detections[i].yolo, img_size);
        rectangle(img, bbox, color, 2, 8, 0);

        std::string text = detections[i].class_name;

        // add confidence to text
        if (show_conf) {
            text += std::format(": {:.2f}", detections[i].conf);
        }

        int x = bbox.x;
        int y;

        // calculate y coordinate of the text
        if (position == Position::top) {
            y = std::max(bbox.y - 5, 20); // min 20, so that text is always visible
        }
        else {
            y = std::min(bbox.y + (int)(detections[i].yolo.h * img_size.height + 30), img_size.height);
        }


        cv::putText(img, text, cv::Point(x, y), 0, 1.2, color, 2, cv::LINE_8, false);
    }
}


cv::Rect YoloDetector::yoloToBbox(YoloCoords yolo, cv::Size img_size) {

    int x = (int)((yolo.x - yolo.w / 2) * img_size.width);
    int y = (int)((yolo.y - yolo.h / 2) * img_size.height);

    int width = (int)(yolo.w * img_size.width);
    int height = (int)(yolo.h * img_size.height);

    return cv::Rect(x, y, width, height);
}


std::vector<Detection> YoloDetector::filterDetections(const std::vector<cv::Mat>& outputs, float conf_thresh, float nms_thresh) {

    std::vector<int> class_ids;
    std::vector<float> confs;
    std::vector<cv::Rect> bboxes;
    std::vector<YoloCoords> coords;

    // iterate over output layers
    for (size_t i = 0; i < outputs.size(); i++) {

        // iterate over detection in current output layer
        float* data = (float*)outputs[i].data;
        for (int j = 0; j < outputs[i].rows; j++, data += outputs[i].cols) {

            // find maximum score
            cv::Mat scores = outputs[i].row(j).colRange(5, outputs[i].cols);
            cv::Point class_id_point;
            double confidence;
            cv::minMaxLoc(scores, 0, &confidence, 0, &class_id_point);

            // filter detections using confidence threshold
            if (confidence > conf_thresh)
            {
                float center_x = data[0] * this->input_size.width;
                float center_y = data[1] * this->input_size.height;
                float width = data[2] * this->input_size.width;
                float height = data[3] * this->input_size.height;

                float topX = center_x - width / 2;
                float topY = center_y - height / 2;


                class_ids.push_back(class_id_point.x);
                confs.push_back((float)confidence);
                bboxes.push_back(cv::Rect(topX, topY, width, height));
                coords.push_back(YoloCoords(data[0], data[1], data[2], data[3]));
            }
        }
    }


    // perform non-maximum suppresion
    std::vector<int> indices;
    cv::dnn::NMSBoxes(bboxes, confs, conf_thresh, nms_thresh, indices);

    // reformat detections to Detection struct
    std::vector<Detection> detections;
    for (int i = 0; i < indices.size(); i++) {

        Detection det;
        det.class_id = class_ids[indices[i]];
        det.class_name = this->class_names[det.class_id];
        det.conf = confs[indices[i]];
        det.bbox = bboxes[indices[i]];
        det.yolo = coords[indices[i]];

        detections.push_back(det);
    }

    return detections;
}


std::vector<std::string> YoloDetector::loadClassNames(std::string names_path) {

    std::vector<std::string> class_names;

    std::ifstream names_file(names_path);

    for (std::string line; std::getline(names_file, line);) {
        class_names.push_back(line);
    }

    return class_names;
}