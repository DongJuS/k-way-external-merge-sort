#ifndef PROCESS_BLOCKS_H
#define PROCESS_BLOCKS_H

#include "FixedRecord.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <filesystem>

int createFixedInitRuns(const std::string& inputBinaryFilename, const std::string& folderPath, size_t blockMemorySize, const std::string& sortColumn);



#endif
