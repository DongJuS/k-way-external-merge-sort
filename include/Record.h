#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <stdexcept>
#include "FixedRecord.h"

struct Record {
    FixedRecord fixed;
    int fileIndex;
    int sortColumnIndex;

    bool operator>(const Record& other) const {
        std::string myKey = extractSortKeyFromRecord(this->fixed, this->sortColumnIndex);
        std::string otherKey = extractSortKeyFromRecord(other.fixed, other.sortColumnIndex);

        if (sortColumnIndex >= 0 && sortColumnIndex <= 3) {
            return std::stoll(myKey) > std::stoll(otherKey);
        } else if (sortColumnIndex >= 4 && sortColumnIndex <= 7) {
            return std::stod(myKey) > std::stod(otherKey);
        } else {
            return myKey > otherKey;
        }
    }
};

#endif
