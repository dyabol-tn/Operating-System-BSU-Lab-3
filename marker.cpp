#include <iostream>
#include <windows.h>
#include <cstdlib>

struct ThreadData {
    int* array;
    int arraySize;
    int threadId;
    HANDLE startEvent;
    HANDLE threadPausedEvent;
    HANDLE terminateEvent;
    HANDLE continueEvent;
    int markedCount;
};

const int SLEEP_DURATION_MS = 5;
const int EVENTS_COUNT = 2;
const int TERMINATE_EVENT_INDEX = 0;
const int CONTINUE_EVENT_INDEX = 1;
const int ZERO_VALUE = 0;

DWORD WINAPI marker_thread(LPVOID param) {
    ThreadData* data = (ThreadData*)param;

    WaitForSingleObject(data->startEvent, INFINITE);
    srand(data->threadId);

    data->markedCount = 0;
    bool terminated = false;

    while (!terminated) {
        int randomNumber = rand();
        int index = randomNumber % data->arraySize;

        if (data->array[index] == ZERO_VALUE) {
            Sleep(SLEEP_DURATION_MS);
            data->array[index] = data->threadId;
            Sleep(SLEEP_DURATION_MS);
            data->markedCount++;
        }
        else {
            SetEvent(data->threadPausedEvent);

            std::cout << "Thread " << data->threadId
                << " cannot mark element. Marked: " << data->markedCount
                << ", Failed index: " << index << std::endl;

            HANDLE waitEvents[EVENTS_COUNT] = {
                data->terminateEvent,
                data->continueEvent
            };

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

    for (int elementIndex = 0; elementIndex < data->arraySize; elementIndex++) {
        if (data->array[elementIndex] == data->threadId) {
            data->array[elementIndex] = ZERO_VALUE;
        }
    }

    std::cout << "Thread " << data->threadId << " terminated." << std::endl;
    return 0;
}