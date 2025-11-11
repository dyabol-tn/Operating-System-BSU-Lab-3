#include <iostream>
#include <windows.h>
#include <cstdlib>
#include "marker.h"

const int SLEEP_DURATION_MS = 5;
const int EVENTS_COUNT = 2;
const int TERMINATE_EVENT_INDEX = 0;
const int CONTINUE_EVENT_INDEX = 1;
const int ZERO_VALUE = 0;

namespace marker_utils {
    bool can_mark_element(int* array, int index) {
        return array[index] == ZERO_VALUE;
    }

    void mark_element(int* array, int index, int threadId) {
        array[index] = threadId;
    }

    bool should_pause_thread(int* array, int index) {
        return !can_mark_element(array, index);
    }

    void reset_events(ThreadData* data) {
    }

    std::string get_thread_info(const ThreadData* data, int failed_index) {
        return "Thread " + std::to_string(data->threadId) +
            " cannot mark element. Marked: " + std::to_string(data->markedCount) +
            ", Failed index: " + std::to_string(failed_index);
    }
}

DWORD WINAPI marker_thread(LPVOID param) {
    ThreadData* data = (ThreadData*)param;
    WaitForSingleObject(data->startEvent, INFINITE);
    srand(data->threadId);

    data->markedCount = 0;
    bool terminated = false;

    while (!terminated) {
        int random_number = rand();
        int index = random_number % data->arraySize;

        if (marker_utils::can_mark_element(data->array, index)) {
            Sleep(SLEEP_DURATION_MS);
            marker_utils::mark_element(data->array, index, data->threadId);
            Sleep(SLEEP_DURATION_MS);
            data->markedCount++;
        }
        else {
            SetEvent(data->threadPausedEvent);

#ifndef TESTING_MODE
            std::cout << marker_utils::get_thread_info(data, index) << std::endl;
#endif
            HANDLE wait_events[EVENTS_COUNT] = {
                data->terminateEvent,
                data->continueEvent
            };

            DWORD wait_result = WaitForMultipleObjects(
                EVENTS_COUNT,
                wait_events,
                FALSE,
                INFINITE
            );

            DWORD signaled_event = wait_result - WAIT_OBJECT_0;

            if (signaled_event == TERMINATE_EVENT_INDEX) {
                terminated = true;
            }
        }
    }

    for (int i = 0; i < data->arraySize; i++) {
        if (data->array[i] == data->threadId) {
            data->array[i] = ZERO_VALUE;
        }
    }

#ifndef TESTING_MODE
    std::cout << "Thread " << data->threadId << " terminated." << std::endl;
#endif

    return 0;
}