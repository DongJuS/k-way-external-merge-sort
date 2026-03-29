# K-way External Merge Sort

고정길이 레코드의 대용량 외부 정렬 알고리즘 (C++)

## 개요

메모리 제한 환경에서 대규모 데이터를 정렬하는 K-way 외부 병합정렬 알고리즘을 구현합니다.
TPC-H LINEITEM 테이블의 고정길이 바이너리 레코드를 대상으로 합니다.

## 구조

```
include/          # 헤더 파일
├── FixedRecord.h         # 고정길이 레코드 읽기/쓰기
├── FixedRecordStruct.h   # 레코드 구조체 정의
├── MergeSort.h           # K-way 병합정렬 알고리즘
├── ProcessBlocks.h       # 블록 단위 처리
└── Record.h              # 레코드 인터페이스

src/              # 소스 파일
├── main.cpp              # 진입점
├── FixedRecord.cpp       # 레코드 구현
├── MergeSort.cpp         # 병합정렬 구현
└── ProcessBlocks.cpp     # 블록 처리 구현

test/             # 테스트
└── testSort.cpp          # 정렬 결과 검증
```

## 빌드 및 실행

```bash
# 빌드
cd src
g++ -I"../include" main.cpp ProcessBlocks.cpp MergeSort.cpp FixedRecord.cpp -o ../bin/main

# 실행
cd ../bin
./main
```

## 핵심 알고리즘

1. 대규모 데이터를 메모리에 적재 가능한 크기의 청크로 분할
2. 각 청크를 인메모리 정렬 후 임시 파일로 저장
3. K개의 정렬된 청크를 동시에 읽으며 병합 (min-heap 활용)
4. 최종 정렬된 결과를 출력 파일에 기록

## 제약사항

- Windows 환경 기준 동시 파일 핸들 제한: 512개
- 고정길이 바이너리 레코드 전용

## 기간

2024.07 ~ 2024.11 (한국기술교육대학교 데이터베이스 시스템 과목)
