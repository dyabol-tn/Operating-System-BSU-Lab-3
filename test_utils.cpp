#include "gtest/gtest.h"
#include "marker.h"

TEST(MarkerUtilsTest, CanMarkElement) {
    int array[5] = { 0, 1, 0, 2, 0 };

    EXPECT_TRUE(marker_utils::can_mark_element(array, 0));
    EXPECT_FALSE(marker_utils::can_mark_element(array, 1));
    EXPECT_TRUE(marker_utils::can_mark_element(array, 2));
    EXPECT_FALSE(marker_utils::can_mark_element(array, 3));
    EXPECT_TRUE(marker_utils::can_mark_element(array, 4));
}

TEST(MarkerUtilsTest, MarkElement) {
    int array[3] = { 0, 0, 0 };

    marker_utils::mark_element(array, 0, 1);
    EXPECT_EQ(array[0], 1);
    EXPECT_EQ(array[1], 0);
    EXPECT_EQ(array[2], 0);

    marker_utils::mark_element(array, 2, 2);
    EXPECT_EQ(array[0], 1);
    EXPECT_EQ(array[1], 0);
    EXPECT_EQ(array[2], 2);
}

TEST(MarkerUtilsTest, ShouldPauseThread) {
    int array[3] = { 0, 5, 0 };

    EXPECT_FALSE(marker_utils::should_pause_thread(array, 0));
    EXPECT_TRUE(marker_utils::should_pause_thread(array, 1));
    EXPECT_FALSE(marker_utils::should_pause_thread(array, 2));
}

TEST(MarkerUtilsTest, GetThreadInfo) {
    ThreadData data = {
        nullptr,
        10,
        3,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        7
    };

    std::string info = marker_utils::get_thread_info(&data, 5);
    EXPECT_EQ(info, "Thread 3 cannot mark element. Marked: 7, Failed index: 5");
}