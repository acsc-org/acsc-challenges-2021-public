#!/bin/sh

./server &
sleep 1

./client 127.0.0.1:7878 flag.jpg
