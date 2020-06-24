#!/bin/bash


# If fail then exit
# Go library

dir_bin=bin
dir_build=build_linux

rm -rf $dir_bin
mkdir $dir_bin
cd library
rm -rf $dir_build
mkdir $dir_build
cd $dir_build
cmake -DCMAKE_BUILD_TYPE=Release  .. || exit 1
make  || exit 1
cd ..
rm -rf $dir_build


cd ..
cd executable
rm -rf $dir_build
mkdir $dir_build
cd $dir_build
cmake -DCMAKE_BUILD_TYPE=Release  .. || exit 1
make  || exit 1
cd ..
rm -rf $dir_build
cd ..
cd $dir_bin
./call || exit 1