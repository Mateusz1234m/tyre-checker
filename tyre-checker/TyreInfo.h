#pragma once
#include "TextfieldInfo.h"

// Types of detected textfields
enum class Field {
    dot,                // DOT sign is visible
    m_s,                // M+S (Mud + Snow) sign is visible
    manufact_code,      // Code that identifies manufacturer
    size_code,          // Manufacturer's identification code
    date,               // Production date, 4 digits, first 2 digits - week, last 2 digits - year 
    width,              // Tyre's width in mm
    height,             // Tyre's height relative to width (in %)
    diameter,           // Tyre's inner diameter in inches
    index               // Speed and load index
};


/*
    Class storing tyre information
*/
class TyreInfo
{
public:

    TextfieldInfo dot;              // DOT sign is visible
    TextfieldInfo m_s;              // M+S (Mud + Snow) sign is visible
    TextfieldInfo manufact;         // Code that identifies manufacturer
    TextfieldInfo size;             // Manufacturer's identification code
    TextfieldInfo date;             // Production date, 4 digits, first 2 digits - week, last 2 digits - year 
    TextfieldInfo width;            // Tyre's width in mm
    TextfieldInfo height;           // Tyre's height relative to width (in %)
    TextfieldInfo diameter;         // Tyre's inner diameter in inches
    TextfieldInfo index;            // Speed and load index


    // adds information to textfield based on detected characters
    void addInfo(Detection textfield, std::vector<Detection> characters);

    // prints all information about tyre
    void printInfo();

private:

    // decodes the production code and returns as a string,
    // e.g. 2422 -> 24th week of 2022
    std::string convertProductionDate(std::string digits);
};

