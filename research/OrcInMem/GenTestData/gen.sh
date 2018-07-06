#!/bin/sh

for ((number = 10; number < 10001; number *= 10))
{
    ./GenTestData "TestData$number" $number
}
