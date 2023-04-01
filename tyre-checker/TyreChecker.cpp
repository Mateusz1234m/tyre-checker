#include "TyreChecker.h"


TyreChecker::TyreChecker(YoloConfig textfields_detector_config, YoloConfig characters_detector_config) {
    this->textfields_detector = new YoloDetector(textfields_detector_config);
    this->characters_detector = new YoloDetector(characters_detector_config);
};

std::tuple<TyreInfo, cv::Mat, InferenceTimes> TyreChecker::checkTyre(cv::Mat img) {

    InferenceTimes inference_times;
    TyreInfo tyre_info;

    auto total_start = std::chrono::high_resolution_clock::now();

    // convert an image from cartesian coordinates to polar coordinates
    auto [img_polar, convertion_time] = this->cartesianToPolar(img);
    inference_times.cartesian_to_polar_time = convertion_time;

    // copy the image, detections will be drawn on the copy to preserve the original image
    cv::Mat img_polar_copy;
    img_polar.copyTo(img_polar_copy);


    // detect textfields
    auto textfields_detection_start = std::chrono::high_resolution_clock::now();
    std::vector<Detection> textfields = textfields_detector->detect(img_polar);
    auto textfields_detection_stop = std::chrono::high_resolution_clock::now();
    inference_times.textfields_inference_time = std::chrono::duration_cast<std::chrono::milliseconds>(textfields_detection_stop - textfields_detection_start);

    // draw detected textfields
    textfields_detector->drawDetections(img_polar_copy, textfields, img_polar.size(), cv::Scalar(255, 0, 255), Position::top, true);

    // detect characters on every detected textfield
    auto characters_detection_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < textfields.size(); i++) {

        // get an image of the textfield from the original high-resolution image
        cv::Rect pix_bbox = textfields_detector->yoloToBbox(textfields[i].yolo, img_polar.size());
        Detection temp_textfield = textfields[i];
        temp_textfield.bbox = this->expandBbox(pix_bbox, 15, img_polar_copy.size().width, img_polar_copy.size().height);
        temp_textfield.yolo = this->bboxToYolo(temp_textfield.bbox, img_polar.size());
        cv::Mat textfield_image = img_polar(temp_textfield.bbox);


        // detect characters on the current textfield
        std::vector<Detection> characters = characters_detector->detect(textfield_image);

        // add info based on detections
        tyre_info.addInfo(textfields[i], characters);

        // draw detections on the high-resulution image
        std::vector<Detection> characters_global = this->calculateGlobalDetections(characters, temp_textfield, img_polar_copy.size());
        characters_detector->drawDetections(img_polar_copy, characters_global, img_polar_copy.size(), cv::Scalar(0, 255, 0), Position::bottom, false);

        //characters_detector->drawDetections(textfield_image, characters, textfield_image.size(), cv::Scalar(0, 255, 0), Position::bottom, false);
        //std::string i_str = std::to_string(i);
        //std::string path = "C:/Users/mateu/Desktop/projekty/tyre-checker-cpp/data/images_out/107_3104_textfield_" + i_str + ".jpg";
        //cv::imwrite(path, textfield_image);

    }
    auto characters_detection_stop = std::chrono::high_resolution_clock::now();
    inference_times.characteres_inference_time = std::chrono::duration_cast<std::chrono::milliseconds>(characters_detection_stop - characters_detection_start);

    auto total_stop = std::chrono::high_resolution_clock::now();
    inference_times.total_time = std::chrono::duration_cast<std::chrono::milliseconds>(total_stop - total_start);

    return { tyre_info, img_polar_copy, inference_times };
};

std::tuple<cv::Mat, std::chrono::milliseconds> TyreChecker::cartesianToPolar(cv::Mat& img_cartesian) {

    auto start = std::chrono::high_resolution_clock::now();

    unsigned int width = img_cartesian.cols;
    unsigned int height = img_cartesian.rows;

    // calculate radius - height of the resut image
    unsigned int radius = height / 2;

    // calculate alpha - width of the resut image
    unsigned int alpha = (2.0 * PI * (float)radius);

    cv::Mat img_polar = cv::Mat::zeros(cv::Size(alpha, radius), CV_8UC3);

    // width and height of the input image should be equal
    if (width != height) {
        throw std::invalid_argument("Width and height are not equal");
    }

    for (int i = 0; i < alpha; i++) {

        // calculate sine and cosine value,
        // depend only on alpha, not radius, so can be done her for optimization
        float angle = 2.0 * PI * ((float)i / (float)alpha);
        float sine_value = sin(angle);
        float cosine_value = cos(angle);

        for (int j = 0; j < radius; j++) {

            // calculate the cartesian coordinates corresponding to the current polar coordinates
            unsigned int x = (unsigned int)((float)j * sine_value) + (width / 2);
            unsigned int y = (unsigned int)((float)j * cosine_value) + (height / 2);

            // get the pixel value from the cartesian image
            cv::Vec3b pixel_value = img_cartesian.at<cv::Vec3b>(x, y);

            // set the pixel value in the polar image
            unsigned int temp_alpha = i;
            unsigned int temp_radius = radius - j - 1;
            img_polar.at<cv::Vec3b>(temp_radius, temp_alpha) = pixel_value;

        }
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds > (stop - start);

    return { img_polar, duration };

}

cv::Rect TyreChecker::expandBbox(cv::Rect bbox, int amount, int max_x, int max_y) {
    cv::Rect temp_bbox;

    temp_bbox.x = std::max(bbox.x - amount, 0);
    temp_bbox.y = std::max(bbox.y - amount, 0);

    temp_bbox.width = std::min(bbox.width + 2 * amount, max_x - temp_bbox.x);
    temp_bbox.height = std::min(bbox.height + 2 * amount, max_y - temp_bbox.y);

    return temp_bbox;
}

std::vector<Detection> TyreChecker::calculateGlobalDetections(std::vector<Detection> dets, Detection textfield, cv::Size global_size) {

    std::vector<Detection> global_detections;

    Detection global_textfield;

    // calculate the ratio of height and width between the textfield and the image
    float x_ratio = (float)global_size.width / (float)this->textfields_detector->input_size.width;
    float y_ratio = (float)global_size.height / (float)this->textfields_detector->input_size.height;

    // rewrite values that don't change
    global_textfield.conf = textfield.conf;
    global_textfield.class_id = textfield.class_id;
    global_textfield.class_name = textfield.class_name;

    // recalculate bounding box of the textfield
    global_textfield.bbox = textfields_detector->yoloToBbox(textfield.yolo, global_size);
    global_textfield.yolo = this->bboxToYolo(global_textfield.bbox, global_size);

    // iterate over characters in the textfield
    for (int i = 0; i < dets.size(); i++) {

        Detection temp_detection;

        // rewrite values that don't change
        temp_detection.conf = dets[i].conf;
        temp_detection.class_id = dets[i].class_id;
        temp_detection.class_name = dets[i].class_name;

        cv::Size temp_size;
        temp_size.width = global_textfield.bbox.width;
        temp_size.height = global_textfield.bbox.height;

        // recalculate bounding box of the character
        cv::Rect temp_bbox = characters_detector->yoloToBbox(dets[i].yolo, temp_size);
        temp_detection.bbox.x = global_textfield.bbox.x + temp_bbox.x;
        temp_detection.bbox.y = global_textfield.bbox.y + temp_bbox.y;
        temp_detection.bbox.width = temp_bbox.width;
        temp_detection.bbox.height = temp_bbox.height;
        temp_detection.yolo = this->bboxToYolo(temp_detection.bbox, global_size);

        global_detections.push_back(temp_detection);
    }

    return global_detections;
}

YoloCoords TyreChecker::bboxToYolo(cv::Rect bbox, cv::Size img_size) {

    YoloCoords yolo_coords;

    yolo_coords.w = (float)bbox.width / (float)img_size.width;
    yolo_coords.h = (float)bbox.height / (float)img_size.height;

    yolo_coords.x = ((float)bbox.x / (float)img_size.width) + (yolo_coords.w / 2);
    yolo_coords.y = ((float)bbox.y / (float)img_size.height) + (yolo_coords.h / 2);


    return yolo_coords;
}

void InferenceTimes::printTimes(){
    std::cout << std::endl;
    std::cout << "####  INFERENCE TIME  ####" << std::endl;
    std::cout << "Cartesian to polar conversion: " << this->cartesian_to_polar_time << std::endl;
    std::cout << "Textfields detection: " << this->textfields_inference_time << std::endl;
    std::cout << "Characters detection: " << this->characteres_inference_time << std::endl;
    std::cout << "TOTAL: " << this->total_time << std::endl;
    std::cout << "##########################" << std::endl;
}
