#include "TextfieldInfo.h"

void TextfieldInfo::addInfo(std::vector<Detection>& characters, float conf) {

    std::string text = "";

    std::vector<int> x_coords;      // stores x coordinates of precious detections
    int num_greater;                // number of x coordinates greater than x coordinate of the current detection

    
    for (int i = 0; i < characters.size(); i++) {

        // calculate how many of the previous detections had an
        // x coordinate greater than the current detection
        num_greater = 0;
        for (int j = 0; j < x_coords.size(); j++) {
            if (x_coords[j] > characters[i].bbox.x) {
                num_greater++;
            }
        }

        // insert character in appropriate place based on x coordinates
        text.insert(x_coords.size() - num_greater, characters[i].class_name);

        x_coords.push_back(characters[i].bbox.x);
    }

    this->info = text;
    this->conf = conf;
}

void TextfieldInfo::addInfo(std::string info, float conf) {
    this->info = info;
    this->conf = conf;
}