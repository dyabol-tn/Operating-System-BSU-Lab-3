#pragma once
#include <windows.h>
#include <string>

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

DWORD WINAPI marker_thread(LPVOID param);

namespace marker_utils {
    bool can_mark_element(int* array, int index);
    void mark_element(int* array, int index, int threadId);
    bool should_pause_thread(int* array, int index);
    void reset_events(ThreadData* data);
    std::string get_thread_info(const ThreadData* data, int failed_index);
}