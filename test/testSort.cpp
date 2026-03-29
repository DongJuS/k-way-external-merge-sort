#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <cctype>
#include <sstream> 

/*
* 필드명과 인덱스를 매핑하는 함수
*/
std::unordered_map<std::string, int> getFieldIndexMap() {
    return {
        {"orderKey", 0},
        {"partKey", 1},
        {"suppKey", 2},
        {"lineNumber", 3},
        {"quantity", 4},
        {"extendedPrice", 5},
        {"discount", 6},
        {"tax", 7},
        {"returnFlag", 8},
        {"lineStatus", 9},
        {"shipDate", 10},
        {"commitDate", 11},
        {"receiptDate", 12},
        {"shipInstr", 13},
        {"shipMode", 14},
        {"comment", 15}
    };
}

/*
* 필드 값 추출 함수
*/
std::string extractFieldValue(const std::string& line, int fieldIndex) {
    size_t startPos = 0;
    size_t endPos = 0;

    for (int i = 0; i <= fieldIndex; ++i) {
        if (i == 0) {
            endPos = line.find('|');
        } else {
            startPos = endPos;
            endPos = line.find('|', startPos + 1);
        }

        if (endPos == std::string::npos) {
            if (i < fieldIndex) {
                throw std::runtime_error("필드 인덱스가 레코드의 필드 수를 초과했습니다.");
            }
            endPos = line.length(); 
        }
    }

    return line.substr((fieldIndex == 0 ? 0 : startPos + 1), endPos - (fieldIndex == 0 ? 0 : startPos + 1));
}

/*
* 정렬 여부를 확인하는 함수
*/
bool isSorted(const std::string& filename, const std::string& column, int sortColumnIndex) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        throw std::runtime_error("파일을 열 수 없습니다: " + filename);
    }

    auto fieldIndexMap = getFieldIndexMap();
    if (fieldIndexMap.find(column) == fieldIndexMap.end()) {
        throw std::invalid_argument("지원되지 않는 필드입니다: " + column);
    }

    int fieldIndex = fieldIndexMap[column]; 
    std::string prevLine, currLine;

    int cnt = 0;

    if (!std::getline(inFile, prevLine)) {
        return true;
    }

    // 파일을 순회하며 각 레코드를 비교
    while (std::getline(inFile, currLine)) {
        std::string prevValue = extractFieldValue(prevLine, fieldIndex);
        std::string currValue = extractFieldValue(currLine, fieldIndex);

        try {
            if (sortColumnIndex >= 0 && sortColumnIndex <= 3) {
                long long prevNum = std::stoll(prevValue);
                long long currNum = std::stoll(currValue);
                if (currNum < prevNum) {
                    std::cout << "정렬 오류 발견 : " << cnt << " = " << prevLine << " -> " << currLine << std::endl;
                    return false;
                }
            } else if (sortColumnIndex >= 4 && sortColumnIndex <= 7) {
                double prevNum = std::stod(prevValue);
                double currNum = std::stod(currValue);
                if (currNum < prevNum) {
                    std::cout << "정렬 오류 발견 : " << cnt << " = " << prevLine << " -> " << currLine << std::endl;
                    return false;
                }
            } else {
                if (currValue < prevValue) {
                    std::cout << "정렬 오류 발견 : " << cnt << " = " << prevLine << " -> " << currLine << std::endl;
                    return false;
                }
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "변환 오류: " << cnt << " = " << e.what() << std::endl;
            return false;
        } catch (const std::out_of_range& e) {
            std::cerr << "범위 오류: " << cnt << " = " << e.what() << std::endl;
            return false;
        }

        prevLine = currLine;
        cnt++;
    }

    std::cout << cnt << "개의 레코드를 확인했습니다." << std::endl;
    inFile.close();
    return true;
}

/*
* 테스트 메인 함수
*/
int main() {
    const std::string sortedFilename = "../assets/sorted_output.txt"; 
    const std::string sortColumn = "quantity";           
    auto indexMap = getFieldIndexMap();

    try {
        bool sorted = isSorted(sortedFilename, sortColumn, indexMap[sortColumn]);
        if (sorted) {
            std::cout << "파일이 정렬되어 있습니다." << std::endl;
        } else {
            std::cout << "파일이 정렬되어 있지 않습니다!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "오류 발생: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
