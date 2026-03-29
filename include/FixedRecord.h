#ifndef FIXED_RECORD_H
#define FIXED_RECORD_H

#include "FixedRecordStruct.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <string>


FixedRecord parseLine(const std::string& line);
void sortRecords(std::vector<FixedRecord>& records, const std::string& column);
void saveToTxt(const std::string& filename, const std::vector<FixedRecord>& records);
void saveToBinary(const std::string& filename, const std::vector<FixedRecord>& records);
void convertTblToBinary(const std::string& tblFilename, const std::string& binFilename);
std::vector<FixedRecord> loadFromBinary(const std::string& filename);


std::string extractSortKeyFromRecord(const FixedRecord& record, int sortColumnIndex);

#endif
