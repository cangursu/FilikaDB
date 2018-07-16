#!/bin/sh

for ((number = 10; number < 10000001; number *= 10))
{
    ./GenTestData "$1TestData$number" $number
}
