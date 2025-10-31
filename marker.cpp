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

DWORD WINAPI marker_thread(LPVOID param) {
    ThreadData* data = (ThreadData*)param;
    WaitForSingleObject(data->startEvent, INFINITE);
    srand(data->threadId);
    data->markedCount = 0;
    bool terminated = false;

    while (!terminated) {
        int random_number = rand();
        int index = random_number % data->arraySize;

        if (data->array[index] == 0) {
            Sleep(SLEEP_DURATION);
            data->array[index] = data->threadId;
            Sleep(SLEEP_DURATION);
            data->markedCount++;
        }
        else {
            std::cout << "Thread " << data->threadId << " cannot mark element. Marked: " << data->markedCount << ", Failed index: " << index << std::endl;
            SetEvent(data->suspendEvent);
            HANDLE waitEvents[EVENTS_COUNT];
            waitEvents[START_EVENT_INDEX] = data->startEvent;
            waitEvents[TERMINATE_EVENT_INDEX] = data->terminateEvent;
            DWORD waitResult = WaitForMultipleObjects(
                EVENTS_COUNT,
                waitEvents,
                FALSE,
                INFINITE
            );
            DWORD signaledEventIndex = waitResult - WAIT_OBJECT_0;

            if (signaledEventIndex == TERMINATE_EVENT_INDEX) {
                terminated = true;
            }
        }
    }

    const int ZERO_VALUE = 0;
    for (int i = 0; i < data->arraySize; i++) {
        if (data->array[i] == data->threadId) {
            data->array[i] = ZERO_VALUE;
        }
    }

    std::cout << "Thread " << data->threadId << " terminated." << std::endl;
    return 0;
}