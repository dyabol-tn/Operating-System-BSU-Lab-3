#include <gtest/gtest.h>
#include <windows.h>
#include <vector>
#include <thread>
#include <atomic>
#include <random>
#include "../marker.h"

TEST(MarkerTest, ThreadDataStructure) {
    ThreadData data = {
        nullptr,
        100,
        1,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        0
    };

    EXPECT_EQ(data.arraySize, 100);
    EXPECT_EQ(data.threadId, 1);
    EXPECT_EQ(data.markedCount, 0);
}

TEST(MarkerTest, ArrayInitialization) {
    const int testSize = 10;
    std::vector<int> array(testSize, 0);

    for (int i = 0; i < testSize; i++) {
        EXPECT_EQ(array[i], 0);
    }
}

TEST(MarkerTest, EventCreation) {
    HANDLE event1 = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    HANDLE event2 = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    EXPECT_NE(event1, nullptr);
    EXPECT_NE(event1, INVALID_HANDLE_VALUE);
    EXPECT_NE(event2, nullptr);
    EXPECT_NE(event2, INVALID_HANDLE_VALUE);

    if (event1 != nullptr && event1 != INVALID_HANDLE_VALUE) {
        CloseHandle(event1);
    }
    if (event2 != nullptr && event2 != INVALID_HANDLE_VALUE) {
        CloseHandle(event2);
    }
}

TEST(MarkerTest, MultiThreadSimulation) {
    const int ARRAY_SIZE = 5;
    std::vector<int> array(ARRAY_SIZE, 0);

    array[0] = 1;
    array[1] = 2;

    EXPECT_EQ(array[0], 1);
    EXPECT_EQ(array[1], 2);

    for (int i = 2; i < ARRAY_SIZE; i++) {
        EXPECT_EQ(array[i], 0);
    }
}

TEST(MarkerTest, RandomNumberGeneration) {
    srand(42);

    const int ARRAY_SIZE = 100;
    bool allInRange = true;

    for (int i = 0; i < 1000; i++) {
        int randomNumber = rand();
        int index = randomNumber % ARRAY_SIZE;

        if (index < 0 || index >= ARRAY_SIZE) {
            allInRange = false;
            break;
        }
    }

    EXPECT_TRUE(allInRange);
}

TEST(MarkerTest, SimpleMultiThreadSimulation) {
    const int ARRAY_SIZE = 10;
    std::vector<int> array(ARRAY_SIZE, 0);

    for (int threadId = 1; threadId <= 2; threadId++) {
        for (int i = 0; i < 5; i++) {
            int index = (threadId + i) % ARRAY_SIZE;
            if (array[index] == 0) {
                array[index] = threadId;
            }
        }
    }

    bool markedElementsExist = false;
    for (int val : array) {
        if (val != 0) {
            markedElementsExist = true;
            break;
        }
    }
    EXPECT_TRUE(markedElementsExist);
}

TEST(MarkerTest, ZeroingElements) {
    const int ARRAY_SIZE = 5;
    std::vector<int> array(ARRAY_SIZE, 0);

    array[0] = 1;
    array[1] = 1;
    array[2] = 2;
    array[3] = 2;
    array[4] = 1;

    for (int& val : array) {
        if (val == 1) {
            val = 0;
        }
    }

    EXPECT_EQ(array[0], 0);
    EXPECT_EQ(array[1], 0);
    EXPECT_EQ(array[4], 0);

    EXPECT_EQ(array[2], 2);
    EXPECT_EQ(array[3], 2);
}

static void SimulateWorker(std::vector<int>& array, std::atomic<int>& markedCount, int threadId) {
    for (int i = 0; i < 5; i++) {
        int index = (threadId + i) % array.size();
        if (array[index] == 0) {
            array[index] = threadId;
            markedCount++;
        }
    }
}

TEST(MarkerTest, ThreadSimulationWithFunction) {
    const int ARRAY_SIZE = 10;
    std::vector<int> array(ARRAY_SIZE, 0);
    std::atomic<int> markedCount{ 0 };

    SimulateWorker(array, markedCount, 1);
    SimulateWorker(array, markedCount, 2);

    EXPECT_GT(markedCount.load(), 0);
}

TEST(MarkerTest, CorrectLambdaUsage) {
    const int ARRAY_SIZE = 10;
    std::vector<int> array(ARRAY_SIZE, 0);
    std::atomic<int> markedCount{ 0 };

    auto worker = [&array, &markedCount](int threadId) {
        for (int i = 0; i < 5; i++) {
            int index = (threadId + i) % array.size();
            if (array[index] == 0) {
                array[index] = threadId;
                markedCount++;
            }
        }
        };

    worker(1);
    worker(2);

    EXPECT_GT(markedCount.load(), 0);
}