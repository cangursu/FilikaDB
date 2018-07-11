#!/bin/sh

for ((number = 10; number < 1000000001; number *= 10))
{
    ./GenTestData "$1TestData$number" $number
}
