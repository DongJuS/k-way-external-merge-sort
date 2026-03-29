#include <vector>
#include <algorithm>
#include <execution>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <string>
#include "FixedRecord.h"

/*
* 텍스트 데이터를 고정 크기 레코드로 변환하는 함수
* 텍스트(lineitem.tbl) -> FixedRecord
*/
FixedRecord parseLine(const std::string& line) {
    std::stringstream ss(line);
    FixedRecord record{};
    std::string temp;

    // 정수형, 실수형 필드 처리
    std::getline(ss, temp, '|'); record.orderKey = std::stoll(temp);
    std::getline(ss, temp, '|'); record.partKey = std::stoll(temp);
    std::getline(ss, temp, '|'); record.suppKey = std::stoll(temp);
    std::getline(ss, temp, '|'); record.lineNumber = std::stoll(temp);
    std::getline(ss, temp, '|'); record.quantity = std::stod(temp);
    std::getline(ss, temp, '|'); record.extendedPrice = std::stod(temp);
    std::getline(ss, temp, '|'); record.discount = std::stod(temp);
    std::getline(ss, temp, '|'); record.tax = std::stod(temp);
    std::getline(ss, temp, '|'); record.returnFlag = temp.empty() ? '\0' : temp[0];
    std::getline(ss, temp, '|'); record.lineStatus = temp.empty() ? '\0' : temp[0];

    // 문자열 필드들: strncpy 후 널 종결 처리
    std::getline(ss, temp, '|'); 
    std::strncpy(record.shipDate, temp.c_str(), sizeof(record.shipDate)-1);
    record.shipDate[sizeof(record.shipDate)-1] = '\0';

    std::getline(ss, temp, '|'); 
    std::strncpy(record.commitDate, temp.c_str(), sizeof(record.commitDate)-1);
    record.commitDate[sizeof(record.commitDate)-1] = '\0';

    std::getline(ss, temp, '|'); 
    std::strncpy(record.receiptDate, temp.c_str(), sizeof(record.receiptDate)-1);
    record.receiptDate[sizeof(record.receiptDate)-1] = '\0';

    std::getline(ss, temp, '|'); 
    std::strncpy(record.shipInstr, temp.c_str(), sizeof(record.shipInstr)-1);
    record.shipInstr[sizeof(record.shipInstr)-1] = '\0';

    std::getline(ss, temp, '|'); 
    std::strncpy(record.shipMode, temp.c_str(), sizeof(record.shipMode)-1);
    record.shipMode[sizeof(record.shipMode)-1] = '\0';

    std::getline(ss, temp, '\n'); 
    std::strncpy(record.comment, temp.c_str(), sizeof(record.comment)-1);
    record.comment[sizeof(record.comment)-1] = '\0';

    return record;
}

// 특정 칼럼 기준 정렬
void sortRecords(std::vector<FixedRecord>& records, const std::string& column) {
    if (column == "orderKey") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return a.orderKey < b.orderKey;});
    } else if (column == "partKey") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return a.partKey < b.partKey;});
    } else if (column == "suppKey") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return a.suppKey < b.suppKey;});
    } else if (column == "lineNumber") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return a.lineNumber < b.lineNumber;});
    } else if (column == "quantity") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return a.quantity < b.quantity;});
    } else if (column == "extendedPrice") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return a.extendedPrice < b.extendedPrice;});
    } else if (column == "discount") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return a.discount < b.discount;});
    } else if (column == "tax") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return a.tax < b.tax;});
    } else if (column == "returnFlag") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return a.returnFlag < b.returnFlag;});
    } else if (column == "lineStatus") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return a.lineStatus < b.lineStatus;});
    } else if (column == "shipDate") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return std::string(a.shipDate)<std::string(b.shipDate);});
    } else if (column == "commitDate") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return std::string(a.commitDate)<std::string(b.commitDate);});
    } else if (column == "receiptDate") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return std::string(a.receiptDate)<std::string(b.receiptDate);});
    } else if (column == "shipInstr") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return std::string(a.shipInstr)<std::string(b.shipInstr);});
    } else if (column == "shipMode") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return std::string(a.shipMode)<std::string(b.shipMode);});
    } else if (column == "comment") {
        std::sort(std::execution::par, records.begin(), records.end(), [](auto &a,auto &b){return std::string(a.comment)<std::string(b.comment);});
    } else {
        throw std::invalid_argument("This column is not supported : " + column);
    }
}

// 텍스트로 저장
void saveToTxt(const std::string& filename, const std::vector<FixedRecord>& records) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    for (auto &r : records) {
        outFile << r.orderKey << "|" << r.partKey << "|" << r.suppKey << "|"
                << r.lineNumber << "|" << r.quantity << "|" << r.extendedPrice << "|"
                << r.discount << "|" << r.tax << "|" << r.returnFlag << "|"
                << r.lineStatus << "|" << r.shipDate << "|" << r.commitDate << "|"
                << r.receiptDate << "|" << r.shipInstr << "|" << r.shipMode << "|"
                << r.comment << "\n";
    }
}

// 바이너리로 저장
void saveToBinary(const std::string& filename, const std::vector<FixedRecord>& records) {
    std::ofstream outFile(filename, std::ios::binary);
    if(!outFile.is_open()){
        throw std::runtime_error("Unable to open file: " + filename);
    }
    outFile.write(reinterpret_cast<const char*>(records.data()), records.size()*sizeof(FixedRecord));
}

// 바이너리에서 불러오기
std::vector<FixedRecord> loadFromBinary(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if(!inFile.is_open()){
        throw std::runtime_error("Unable to open file: " + filename);
    }
    std::vector<FixedRecord> records;
    FixedRecord rec;
    while(inFile.read(reinterpret_cast<char*>(&rec), sizeof(FixedRecord))) {
        records.push_back(rec);
    }
    return records;
}

// tbl -> binary 변환 함수(전처리용)
void convertTblToBinary(const std::string& tblFilename, const std::string& binFilename) {
    std::ifstream inFile(tblFilename);
    if(!inFile.is_open()) {
        throw std::runtime_error("Unable to open input tbl: " + tblFilename);
    }
    std::ofstream outFile(binFilename, std::ios::binary);
    if(!outFile.is_open()) {
        throw std::runtime_error("Unable to open output bin: " + binFilename);
    }

    std::string line;
    std::vector<FixedRecord> buffer;
    buffer.reserve(100000); // 큰 버퍼(원하는 크기로 조정)
    while(std::getline(inFile, line)) {
        buffer.push_back(parseLine(line));
        if(buffer.size() == 100000) {
            outFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size()*sizeof(FixedRecord));
            buffer.clear();
        }
    }
    if(!buffer.empty()) {
        outFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size()*sizeof(FixedRecord));
    }
}

std::string extractSortKeyFromRecord(const FixedRecord& record, int sortColumnIndex) {
    switch (sortColumnIndex) {
        case 0: return std::to_string(record.orderKey);
        case 1: return std::to_string(record.partKey);
        case 2: return std::to_string(record.suppKey);
        case 3: return std::to_string(record.lineNumber);
        case 4: return std::to_string(record.quantity);
        case 5: return std::to_string(record.extendedPrice);
        case 6: return std::to_string(record.discount);
        case 7: return std::to_string(record.tax);
        case 8: return std::string(1, record.returnFlag);
        case 9: return std::string(1, record.lineStatus);
        case 10: return std::string(record.shipDate);
        case 11: return std::string(record.commitDate);
        case 12: return std::string(record.receiptDate);
        case 13: return std::string(record.shipInstr);
        case 14: return std::string(record.shipMode);
        case 15: return std::string(record.comment);
        default: throw std::out_of_range("Invalid column index");
    }
}