#include <iostream>
#include <vector>
#include <windows.h>
#include "marker.h"

std::vector<HANDLE> getActivePausedEvents(const std::vector<ThreadData*>& threadData) {
    std::vector<HANDLE> pausedEvents;
    for (const auto& data : threadData) {
        if (data != nullptr) {
            pausedEvents.push_back(data->threadPausedEvent);
        }
    }
    return pausedEvents;
}

void resetPausedEvents(const std::vector<ThreadData*>& threadData) {
    for (const auto& data : threadData) {
        if (data != nullptr) {
            // ResetEvent(data->threadPausedEvent);
        }
    }
}

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
    std::vector<int> array(arraySize, 0);

    int threadCount;
    std::cout << "Enter number of marker threads: ";
    std::cin >> threadCount;

    std::vector<HANDLE> threadHandles;
    std::vector<ThreadData*> threadData;

    for (int threadIndex = 0; threadIndex < threadCount; threadIndex++) {
        int currentThreadId = threadIndex + 1;

        ThreadData* data = new ThreadData{
            array.data(),
            arraySize,
            currentThreadId,
            CreateEvent(nullptr, TRUE, FALSE, nullptr),
            CreateEvent(nullptr, FALSE, FALSE, nullptr),
            CreateEvent(nullptr, FALSE, FALSE, nullptr),
            CreateEvent(nullptr, FALSE, FALSE, nullptr),
            0
        };

        threadData.push_back(data);

        HANDLE threadHandle = CreateThread(
            nullptr,
            0,
            marker_thread,
            data,
            0,
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

    std::cout << "\nStarting all threads..." << std::endl;
    for (auto& data : threadData) {
        SetEvent(data->startEvent);
    }

    int activeThreads = threadCount;

    while (activeThreads > 0) {
        std::vector<HANDLE> pausedEvents = getActivePausedEvents(threadData);

        if (!pausedEvents.empty()) {
            std::cout << "\n--- Waiting for all threads to pause ---" << std::endl;
            WaitForMultipleObjects(
                static_cast<DWORD>(pausedEvents.size()),
                pausedEvents.data(),
                TRUE,
                INFINITE
            );
        }

        std::cout << "\n=== All threads paused ===" << std::endl;
        printArray(array, "Current array: ");

        int threadToTerminate;
        std::cout << "\nEnter thread number to terminate (1-" << threadCount << "): ";
        std::cin >> threadToTerminate;

        bool threadFound = false;

        for (size_t i = 0; i < threadData.size(); i++) {
            if (threadData[i] != nullptr && threadData[i]->threadId == threadToTerminate) {
                threadFound = true;

                SetEvent(threadData[i]->terminateEvent);

                WaitForSingleObject(threadHandles[i], INFINITE);

                CloseHandle(threadHandles[i]);
                CloseHandle(threadData[i]->startEvent);
                CloseHandle(threadData[i]->threadPausedEvent);
                CloseHandle(threadData[i]->terminateEvent);
                CloseHandle(threadData[i]->continueEvent);

                delete threadData[i];
                threadData[i] = nullptr;

                activeThreads--;

                std::cout << "Thread " << threadToTerminate << " terminated successfully." << std::endl;
                break;
            }
        }

        if (!threadFound) {
            std::cout << "Thread " << threadToTerminate << " not found or already terminated." << std::endl;
        }

        printArray(array, "\nArray after termination: ");

        std::cout << "\n--- Resuming remaining threads ---" << std::endl;
        for (auto& data : threadData) {
            if (data != nullptr) {
                SetEvent(data->continueEvent);
            }
        }
    }

    threadHandles.clear();
    threadData.clear();

    std::cout << "\n=== All threads completed. Program finished. ===" << std::endl;
    return 0;
}