#include "ProcessBlocks.h"
#include "FixedRecord.h" // sortRecords, saveToBinary 함수 선언 인식
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <filesystem>
#include <sstream>
#include <iomanip>

const int numThreads = 6;  // Consumer 스레드 개수

int createFixedInitRuns(const std::string& inputBinaryFilename, const std::string& folderPath, size_t blockMemorySize, const std::string& sortColumn) {
    std::ifstream inFile(inputBinaryFilename, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Unable to open input file: " + inputBinaryFilename);
    }

    // 출력 폴더 생성
    if (!std::filesystem::exists(folderPath)) {
        std::filesystem::create_directories(folderPath);
        std::cout << "Created directory: " << folderPath << std::endl;
    }

    // 레코드 크기 기반 블록 크기 계산
    size_t recordSize = sizeof(FixedRecord);
    size_t blockSize = blockMemorySize / recordSize; // 메모리 크기를 기준으로 블록 크기 계산

    // Producer-Consumer용 자료구조
    std::queue<std::pair<int, std::vector<FixedRecord>>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    bool doneReading = false;

    int blockIndex = 1;
    size_t totalRecords = 0;

    auto totalStartTime = std::chrono::high_resolution_clock::now();

    // Producer 스레드
    auto producer = [&]() {
        std::vector<FixedRecord> block(blockSize);

        while (true) {
            inFile.read(reinterpret_cast<char*>(block.data()), blockSize * recordSize);
            std::streamsize count = inFile.gcount() / recordSize;
            if (count == 0) break;

            totalRecords += static_cast<size_t>(count);
            block.resize(static_cast<size_t>(count));

            {
                std::unique_lock<std::mutex> lock(queueMutex);
                taskQueue.emplace(blockIndex++, std::move(block));
                block = std::vector<FixedRecord>(blockSize); // 새로운 블록 할당
            }
            cv.notify_one();

            if (count < blockSize) {
                // 더 이상 읽을 레코드 없음
                break;
            }
        }

        doneReading = true;
        cv.notify_all();
    };

    // Consumer 스레드
    auto consumer = [&](int threadId) {
        while (true) {
            std::pair<int, std::vector<FixedRecord>> task;

            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, [&] { return !taskQueue.empty() || doneReading; });

                if (taskQueue.empty() && doneReading) {
                    break;
                }

                task = std::move(taskQueue.front());
                taskQueue.pop();
            }

            auto blockStartTime = std::chrono::high_resolution_clock::now();

            // 정렬
            sortRecords(task.second, sortColumn);

            // 바이너리 파일로 run 저장
            std::ostringstream filename;
            filename << folderPath << "/" << std::setw(4) << std::setfill('0') << task.first << ".bin";
            saveToBinary(filename.str(), task.second);

            auto blockEndTime = std::chrono::high_resolution_clock::now();
            auto blockDuration = std::chrono::duration_cast<std::chrono::milliseconds>(blockEndTime - blockStartTime).count();

            std::cout << "Thread " << threadId << " processed block " << task.first
                      << " | Time: " << blockDuration << " ms" << std::endl;
        }
    };

    std::thread producerThread(producer);
    std::vector<std::thread> consumerThreads;
    for (int i = 0; i < numThreads; ++i) {
        consumerThreads.emplace_back(consumer, i + 1);
    }

    producerThread.join();
    for (auto& t : consumerThreads) {
        t.join();
    }

    inFile.close();

    auto totalEndTime = std::chrono::high_resolution_clock::now();
    auto totalElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(totalEndTime - totalStartTime).count();
    std::cout << "All blocks processed and saved to folder: " << folderPath << std::endl;
    std::cout << "[STATS] Total Execution Time: " << totalElapsed << " ms" << std::endl;

    return blockIndex - 1; // 생성된 run 파일 수
}
