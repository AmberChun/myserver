#!/bin/bash

echo "清理编译产物。。。。。。。。。。"
make clean

echo "开始编译"
make -j6
