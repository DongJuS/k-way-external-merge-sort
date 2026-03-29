#include "MergeSort.h"
#include <queue>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <cstdio>
#include <cerrno>
#include <string>
#include "FixedRecord.h"
#include "Record.h"  

// mergeFilesBinary 호출 시 로깅 메시지를 출력하도록 수정
void mergeFilesBinary(const std::vector<std::string>& inputFiles, const std::string& outputFile,
                      int& readCount, int& writeCount, int sortColumnIndex) {

    constexpr size_t memoryLimit =  512ULL * 1024 * 1024;  // 512MB 제한 (외부정렬 메모리 제한 - 한 배치 내 동적으로 레코드 조절)

    auto mergeStartTime = std::chrono::high_resolution_clock::now();

    size_t recordSize = sizeof(FixedRecord); // 고정된 레코드 크기
    size_t maxTotalRecordsInMemory = memoryLimit / recordSize; // 메모리 제한 내에서 처리 가능한 최대 레코드 수
    size_t bufferRecordsPerFile = maxTotalRecordsInMemory / inputFiles.size(); // 파일당 버퍼 크기 (레코드 수)

    // 파일 스트림 초기화 및 버퍼 설정
    std::priority_queue<Record, std::vector<Record>, std::greater<>> minHeap;
    std::vector<std::ifstream> fileStreams(inputFiles.size());
    std::vector<std::vector<FixedRecord>> buffers(inputFiles.size());
    std::vector<size_t> bufferIndices(inputFiles.size(), 0);
    std::vector<size_t> bufferSizes(inputFiles.size(), 0);

    for (size_t i = 0; i < inputFiles.size(); ++i) {
        fileStreams[i].open(inputFiles[i], std::ios::binary);
        if (!fileStreams[i].is_open()) {
            throw std::runtime_error("Failed to open input file: " + inputFiles[i]);
        }

        buffers[i].resize(bufferRecordsPerFile); // 파일당 버퍼 크기 설정
        fileStreams[i].read(reinterpret_cast<char*>(buffers[i].data()), bufferRecordsPerFile * recordSize);
        bufferSizes[i] = fileStreams[i].gcount() / recordSize; // 실제 읽은 레코드 수 계산
        if (bufferSizes[i] > 0) {
            Record r{ buffers[i][0], (int)i, sortColumnIndex };
            minHeap.push(r);
            readCount++;
        }
    }

    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to create output file: " + outputFile);
    }

    while (!minHeap.empty()) {
        Record smallest = minHeap.top();
        minHeap.pop();

        outFile.write(reinterpret_cast<const char*>(&smallest.fixed), recordSize);
        writeCount++;

        size_t fileIndex = smallest.fileIndex;
        bufferIndices[fileIndex]++;
        if (bufferIndices[fileIndex] < bufferSizes[fileIndex]) {
            Record next{ buffers[fileIndex][bufferIndices[fileIndex]], (int)fileIndex, sortColumnIndex };
            minHeap.push(next);
            readCount++;
        } else {
            // 버퍼를 다시 채우기
            fileStreams[fileIndex].read(reinterpret_cast<char*>(buffers[fileIndex].data()), bufferRecordsPerFile * recordSize);
            bufferSizes[fileIndex] = fileStreams[fileIndex].gcount() / recordSize; // 실제 읽은 레코드 수 계산
            bufferIndices[fileIndex] = 0;
            if (bufferSizes[fileIndex] > 0) {
                Record next{ buffers[fileIndex][0], (int)fileIndex, sortColumnIndex };
                minHeap.push(next);
                readCount++;
            }
        }
    }

    for (auto& fs : fileStreams) {
        fs.close();
    }
    outFile.close();

    auto mergeEndTime = std::chrono::high_resolution_clock::now();
    std::cout << "[mergesort] Merge total time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(mergeEndTime - mergeStartTime).count()
              << " ms" << std::endl;
}




void externalMergeSortBinary(const std::string& folderPath, int numFiles, const std::string& outputFilename, int sortColumnIndex) {
    const int maxOpenFiles = 500;
    int readCount = 0, writeCount = 0;

    auto totalStartTime = std::chrono::high_resolution_clock::now();

    int iteration = 0;
    int currentNumFiles = numFiles;

    while (currentNumFiles > 1) {
        auto iterationStart = std::chrono::high_resolution_clock::now();

        int nextFileIndex = 0;

        for (int batchStart = 0; batchStart < currentNumFiles; batchStart += maxOpenFiles) {
            int batchEnd = std::min(batchStart + maxOpenFiles, currentNumFiles);

            std::ostringstream tempFileName;
            tempFileName << folderPath << "/temp_iter" << iteration << "_" << nextFileIndex++ << ".bin"; // 확장자 .txt 예시, 필요 시 .bin
            // 병합 파일들 목록 준비
            std::vector<std::string> batchFiles;
            for (int i = batchStart; i < batchEnd; ++i) {
                std::ostringstream filename;
                if (iteration == 0) {
                    filename << folderPath << "/" << std::setw(4) << std::setfill('0') << (i + 1) << ".bin";
                } else {
                    filename << folderPath << "/temp_iter" << (iteration - 1) << "_" << i << ".bin";
                }
                batchFiles.push_back(filename.str());
            }

            mergeFilesBinary(batchFiles, tempFileName.str(), readCount, writeCount, sortColumnIndex);
        }

        auto iterationEnd = std::chrono::high_resolution_clock::now();
        std::cout << "[mergesort] Iteration " << iteration << " time: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(iterationEnd - iterationStart).count()
                  << " ms" << std::endl;

        currentNumFiles = (currentNumFiles + maxOpenFiles - 1) / maxOpenFiles;
        iteration++;
    }

    // 최종 파일 이동 시간 측정
    auto finalMoveStart = std::chrono::high_resolution_clock::now();
    std::ostringstream finalFileName;
    finalFileName << folderPath << "/temp_iter" << (iteration - 1) << "_0.bin";

    if (std::remove(outputFilename.c_str()) != 0 && errno != ENOENT) {
        perror("Error removing existing output file");
    }

    if (std::rename(finalFileName.str().c_str(), outputFilename.c_str()) != 0) {
        throw std::runtime_error("Failed to rename final file");
    }
    auto finalMoveEnd = std::chrono::high_resolution_clock::now();

    std::cout << "[mergesort] Final file move time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(finalMoveEnd - finalMoveStart).count()
              << " ms" << std::endl;

    auto totalEndTime = std::chrono::high_resolution_clock::now();
    std::cout << "[mergesort] Total external merge sort time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(totalEndTime - totalStartTime).count()
              << " ms" << std::endl;

    std::cout << "Reads: " << readCount << std::endl;
    std::cout << "Writes: " << writeCount << std::endl;
}
