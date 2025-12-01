#include <iostream>
#include <windows.h>
#include <cstdlib>

struct ThreadData {
    int* array;
    int arraySize;
    int threadId;
    HANDLE startEvent;
    HANDLE suspendEvent;
    HANDLE terminateEvent;
    int markedCount;
};

const int SLEEP_DURATION = 5;
const int EVENTS_COUNT = 2;
const int START_EVENT_INDEX = 0;
const int TERMINATE_EVENT_INDEX = 1;
const int ZERO_VALUE = 0;
const int FAILED_WAIT_RESULT = WAIT_FAILED;
const int WAIT_INFINITE = INFINITE;

DWORD WINAPI marker_thread(LPVOID param) {
    ThreadData* data = (ThreadData*)param;
    WaitForSingleObject(data->startEvent, WAIT_INFINITE);
    srand(data->threadId);
    data->markedCount = 0;
    bool terminated = false;

    while (!terminated) {
        int randomNumber = rand();
        int index = randomNumber % data->arraySize;

        if (data->array[index] == ZERO_VALUE) {
            Sleep(SLEEP_DURATION);
            data->array[index] = data->threadId;
            Sleep(SLEEP_DURATION);
            data->markedCount++;
        }
        else {
            std::cout << "Thread " << data->threadId << " cannot mark element. Marked: " << data->markedCount << ", Failed index: " << index << std::endl;

            SetEvent(data->suspendEvent);
            HANDLE waitEvents[EVENTS_COUNT] = {
                data->startEvent,
                data->terminateEvent
            };

            DWORD waitResult = WaitForMultipleObjects(
                EVENTS_COUNT,
                waitEvents,
                FALSE,
                WAIT_INFINITE
            );

            if (waitResult == FAILED_WAIT_RESULT) {
                std::cerr << "Thread " << data->threadId
                    << " failed to wait for events." << std::endl;
                break;
            }

            DWORD signaledEventIndex = waitResult - WAIT_OBJECT_0;

            if (signaledEventIndex == TERMINATE_EVENT_INDEX) {
                terminated = true;
            }
        }
    }

    for (int elementIndex = 0; elementIndex < data->arraySize; elementIndex++) {
        if (data->array[elementIndex] == data->threadId) {
            data->array[elementIndex] = ZERO_VALUE;
        }
    }
    return 0;
}