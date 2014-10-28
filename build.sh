#!/bin/bash
#this will remove any partial builds, if there was a failure


if [ "$1" == "clean" ]
then
    rm *.so
    exit
fi


python setup.py build_ext --inplace
#rm -r build/
echo "~~You made it!~~"