#include "CppUTest/TestHarness.h"
#include "hal_adc.hpp"

TEST_GROUP(HalAdc){void setup(){} void teardown(){}};

TEST(HalAdc, ReturnsFakeAdcValue)
{
    float result = getAdcTemp();         // replace with your testable function
    CHECK_EQUAL(int(36.6), int(result)); // Expect 42, or whatever logic you test
}