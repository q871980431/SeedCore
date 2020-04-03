#!/bin/bash
##在执行目录 创建目标目录的软连接
echo "create link path:$1"
ln -s $1/core `pwd`/core
ln -s $1/envir `pwd`/envir 
