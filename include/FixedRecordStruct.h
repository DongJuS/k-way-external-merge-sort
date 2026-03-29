#ifndef FIXED_RECORD_STRUCT_H
#define FIXED_RECORD_STRUCT_H

#include <string>

// 고정 크기 레코드 구조체 정의
struct FixedRecord {
    long long orderKey;         // 8 bytes
    long long partKey;          // 8 bytes
    long long suppKey;          // 8 bytes
    long long lineNumber;       // 8 bytes
    double quantity;            // 8 bytes
    double extendedPrice;       // 8 bytes
    double discount;            // 8 bytes
    double tax;                 // 8 bytes
    char returnFlag;            // 1 byte
    char lineStatus;            // 1 byte
    char shipDate[11];          // 10 + 1 (null-terminated)
    char commitDate[11];        // 10 + 1 (null-terminated)
    char receiptDate[11];       // 10 + 1 (null-terminated)
    char shipInstr[25];         // 24 + 1 (null-terminated)
    char shipMode[10];          // 9 + 1 (null-terminated)
    char comment[44];           // 43 + 1 (null-terminated)
};

#endif
