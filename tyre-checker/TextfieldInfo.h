#pragma once
#include<stdio.h>
#include <iostream>
#include"YoloDetector.h"

/*
    Class storing textfield data    
*/
class TextfieldInfo
{
public:

    std::string info = "";      // textfield data
    float conf = 0.0;           // confidence of textfield detection

    // adds data based on the characters detected in the textfield
    void addInfo(std::vector<Detection>& characters, float conf);

    // overload function that adds string as a data to the textfield
    void addInfo(std::string info, float conf);
};

