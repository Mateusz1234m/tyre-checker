#include "TyreInfo.h"

void TyreInfo::addInfo(Detection textfield, std::vector<Detection> characters) {

    // check which textfield was detected
    Field textfield_id = Field(textfield.class_id);

    // add information to a specific field that was detected
    // if more than one field of the same type is detected, compare confidence
    switch (textfield_id) {

        case Field::dot:
            if (textfield.conf > this->dot.conf) {
                this->dot.addInfo("visible", textfield.conf);
            }
            break;

        case Field::m_s:
            if (textfield.conf > this->m_s.conf) {
                this->dot.addInfo("Mud+Snow", textfield.conf);
            }
            break;

        case Field::manufact_code:
            if (textfield.conf > this->manufact.conf) {
                this->manufact.addInfo(characters, textfield.conf);
            }
            break;

        case Field::size_code:
            if (textfield.conf > this->size.conf) {
                this->size.addInfo(characters, textfield.conf);
            }
            break;

        case Field::date:
            if (textfield.conf > this->date.conf) {
                this->date.addInfo(characters, textfield.conf);
            }
            break;

        case Field::width:
            if (textfield.conf > this->width.conf) {
                this->width.addInfo(characters, textfield.conf);
            }
            break;

        case Field::height:
            if (textfield.conf > this->height.conf) {
                this->height.addInfo(characters, textfield.conf);
            }
            break;

        case Field::diameter:
            if (textfield.conf > this->diameter.conf) {
                this->diameter.addInfo(characters, textfield.conf);
            }
            break;

        case Field::index:
            if (textfield.conf > this->index.conf) {
                this->index.addInfo(characters, textfield.conf);
            }
            break;

        default:
            throw("Unrecognized class ID");
    }

}

void TyreInfo::printInfo() {
    std::cout << std::endl;
    std::cout << "#####   TYRE INFO    ####" << std::endl;
    std::cout << std::format("DOT: {}", this->dot.info) << std::endl;
    std::cout << std::format("M+S: {}", this->m_s.info) << std::endl;
    std::cout << std::format("Manufacturer code: {}", this->manufact.info) << std::endl;
    std::cout << std::format("Size code: {}", this->size.info) << std::endl;
    std::cout << std::format("Production date: {}", this->convertProductionDate(this->date.info)) << std::endl;
    std::cout << std::format("Width: {}", this->width.info) << std::endl;
    std::cout << std::format("Height: {}", this->height.info) << std::endl;
    std::cout << std::format("Diameter: {}", this->diameter.info) << std::endl;
    std::cout << std::format("Index: {}", this->index.info) << std::endl;
    std::cout << "#########################" << std::endl;
}

std::string TyreInfo::convertProductionDate(std::string digits) {

    // production date should contain 4 digits
    if (digits.length() != 4) {
        return digits + ": invalid production date format";
    }
    else {

        std::string week = digits.substr(0, 2);
        std::string year = digits.substr(2, 2);

        int week_int = std::stoi(week); // TODO: replace with std::from_chars
        int year_int = std::stoi(year); // TODO: replace with std::from_chars

        // convert production year to 19xx or 20xx
        if (year_int > 50) {
            year_int += 1900;
        }
        else {
            year_int += 2000;
        }

        year = std::format("{}", year_int);

        return std::format("{}th week of {} (code: {})", week_int, year_int, digits);
    }
}
