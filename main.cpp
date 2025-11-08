#include <iostream>
#include <vector>
#include <windows.h>
#include "marker.h"

struct ThreadData {
    int* array;
    int arraySize;
    int threadId;
    HANDLE startEvent;
    HANDLE suspendEvent;
    HANDLE terminateEvent;
    int markedCount;
};

const int INITIAL_ARRAY_VALUE = 0;
const int FIRST_THREAD_ID = 1;
const int THREAD_ID_INCREMENT = 1;
const int MINIMUM_ACTIVE_THREADS = 0;
const int WAIT_INFINITE = INFINITE;
const int EVENT_MANUAL_RESET = TRUE;
const int EVENT_AUTO_RESET = FALSE;
const int INITIAL_EVENT_STATE = FALSE;
const int DEFAULT_STACK_SIZE = 0;
const int DEFAULT_CREATION_FLAGS = 0;
const BOOL WAIT_ALL_OBJECTS = TRUE;

void printArray(const std::vector<int>& array, const std::string& message) {
    std::cout << message;
    for (int value : array) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}

int main() {
    int arraySize;
    std::cout << "Enter array size: ";
    std::cin >> arraySize;
    std::vector<int> array(arraySize, INITIAL_ARRAY_VALUE);
    int threadCount;
    std::cout << "Enter number of marker threads: ";
    std::cin >> threadCount;
    std::vector<HANDLE> threadHandles;
    std::vector<ThreadData*> threadData;
    std::vector<HANDLE> suspendEvents;

    for (int threadIndex = 0; threadIndex < threadCount; threadIndex++) {
        int currentThreadId = FIRST_THREAD_ID + threadIndex * THREAD_ID_INCREMENT;

        ThreadData* data = new ThreadData{
            array.data(),
            arraySize,
            currentThreadId,
            CreateEvent(
                nullptr,
                EVENT_MANUAL_RESET,
                INITIAL_EVENT_STATE,
                nullptr
            ),
            CreateEvent(
                nullptr,
                EVENT_AUTO_RESET,
                INITIAL_EVENT_STATE,
                nullptr
            ), 
            CreateEvent(
                nullptr,
                EVENT_AUTO_RESET,
                INITIAL_EVENT_STATE,
                nullptr
            ), 0 
        };

        threadData.push_back(data);
        suspendEvents.push_back(data->suspendEvent);

        HANDLE threadHandle = CreateThread(
            nullptr,
            DEFAULT_STACK_SIZE,
            marker_thread,
            data,
            DEFAULT_CREATION_FLAGS,
            nullptr 
        );

        if (threadHandle == nullptr) {
            std::cerr << "Failed to create thread " << currentThreadId << std::endl;
            delete data;
            continue;
        }

        threadHandles.push_back(threadHandle);
        std::cout << "Thread " << currentThreadId << " created." << std::endl;
    }

    std::cout << "Starting all threads..." << std::endl;
    for (auto& data : threadData) {
        SetEvent(data->startEvent);
    }

    int activeThreads = threadCount;

    while (activeThreads > MINIMUM_ACTIVE_THREADS) {
        if (!suspendEvents.empty()) {
            WaitForMultipleObjects(
                static_cast<DWORD>(suspendEvents.size()),
                suspendEvents.data(),
                WAIT_ALL_OBJECTS,
                WAIT_INFINITE
            );
        }

        printArray(array, "Current array: ");

        int threadToTerminate;
        std::cout << "Enter thread number to terminate: ";
        std::cin >> threadToTerminate;

        bool threadFound = false;

        for (size_t threadIndex = 0; threadIndex < threadData.size(); threadIndex++) {
            if (threadData[threadIndex] && threadData[threadIndex]->threadId == threadToTerminate) {
                threadFound = true;

                SetEvent(threadData[threadIndex]->terminateEvent);

                WaitForSingleObject(threadHandles[threadIndex], WAIT_INFINITE);

                CloseHandle(threadHandles[threadIndex]);
                CloseHandle(threadData[threadIndex]->startEvent);
                CloseHandle(threadData[threadIndex]->suspendEvent);
                CloseHandle(threadData[threadIndex]->terminateEvent);

                delete threadData[threadIndex];
                threadData[threadIndex] = nullptr;

                suspendEvents.erase(suspendEvents.begin() + threadIndex);
                activeThreads--;

                std::cout << "Thread " << threadToTerminate << " terminated successfully." << std::endl;
                break;
            }
        }

        if (!threadFound) {
            std::cout << "Thread " << threadToTerminate << " not found or already terminated." << std::endl;
        }

        printArray(array, "Array after termination: ");

        for (auto& data : threadData) {
            if (data) {
                SetEvent(data->startEvent);
            }
        }
    }

    std::cout << "All threads completed. Program finished." << std::endl;

    return 0;
}