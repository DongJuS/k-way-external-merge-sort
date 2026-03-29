#include <iostream>
#include <string>
#include <unordered_map>
#include "ProcessBlocks.h"
#include "MergeSort.h"
#include "FixedRecord.h"

/*
 * FixedRecord 필드명과 인덱스를 매핑
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

int main() {
    // 경로 설정
    const std::string inputTblFilename = "../assets/lineitem.tbl";
    const std::string inputBinaryFilename = "../assets/lineitem.bin"; // 전처리 후 사용
    const std::string folderPath = "./runs";
    const std::string binaryOutputFilename = "../assets/sorted_output.bin";
    const std::string textOutputFilename = "../assets/sorted_output.txt";
    const size_t blockMemorySize = 150 * 1024 * 1024; //  블록 단위 메모리 사용량 제한 
    auto indexMap = getFieldIndexMap();
    std::string sortColumn;
    int runFileCount;

    try {
        std::cout << "Enter the column name [ex) quantity, partKey] : ";
        std::cin >> sortColumn;
        std::cout << std::endl;

        if (indexMap.find(sortColumn) == indexMap.end()) {
            std::cerr << "Invalid column name: " << sortColumn << std::endl;
            return 1;
        }

        std::cout << "start make init run" << std::endl;
        // tbl -> binary 변환 (전처리)
        // convertTblToBinary(inputTblFilename, inputBinaryFilename); 

        // 블록 단위 정렬 및 바이너리 런 생성
        runFileCount = createFixedInitRuns(inputBinaryFilename, folderPath, blockMemorySize, sortColumn);

        std::cout << "start merge sort : " << sortColumn << " and : " << indexMap[sortColumn] << std::endl;
        // 외부 병합 정렬 (바이너리)
        externalMergeSortBinary(folderPath, runFileCount, binaryOutputFilename, indexMap[sortColumn]);

        // 디버깅용 텍스트 출력
        auto finalRecords = loadFromBinary(binaryOutputFilename);
        saveToTxt(textOutputFilename, finalRecords);

        std::cout << "end merge sort" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error : " << e.what() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.ignore();
        std::cin.get();
        return 1;
    }

    std::cout << "Press Enter to exit the program..." << std::endl;
    std::cin.ignore();
    std::cin.get();

    return 0;
}
// 빌드 과정
// cd C:\데이터베이스시스템\project1207_editing\src
// g++ -I"../include" main.cpp ProcessBlocks.cpp MergeSort.cpp FixedRecord.cpp -o main.exe
