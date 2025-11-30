#include <iostream>
#include <vector>
#include <windows.h>
#include "marker.h"

int main() {
    int arraySize;
    std::cout << "Enter array size: ";
    std::cin >> arraySize;
    std::vector<int> array(arraySize, 0);
    int threadCount;
    std::cout << "Enter number of marker threads: ";
    std::cin >> threadCount;
    std::vector<HANDLE> threadHandles;
    std::vector<int> threadParams;

    for (int i = 0; i < threadCount; i++) {
        threadParams.push_back(i + 1);
    }
    return 0;
}