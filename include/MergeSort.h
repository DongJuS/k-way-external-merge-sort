#ifndef MERGE_SORT_H
#define MERGE_SORT_H

#include "Record.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <queue>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <cerrno>

std::string extractSortKey(const std::string& line, int sortColumnIndex);
void mergeFiles(const std::vector<std::string>& inputFiles, const std::string& outputFile, int& readCount, int& writeCount, int sortColumnIndex);
void externalMergeSort(const std::string& folderPath, int numFiles, const std::string& outputFilename, int sortColumnIndex);
void externalMergeSortBinary(const std::string& folderPath, int numFiles, const std::string& outputFilename, int sortColumnIndex);
std::string extractSortKeyFromRecord(const FixedRecord& record, int sortColumnIndex);
#endif
