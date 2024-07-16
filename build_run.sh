#!/bin/bash

#
# @Author Zulqarnain Z.
# @HireMe https://www.fiverr.com/zulqar
#

g++ -std=gnu++17 -o zulqarDOTnet web_server.cpp -lboost_system -lboost_thread -lboost_iostreams -lpthread -lz -lzstd -lbrotlienc -lbrotlicommon

./zulqarDOTnet "127.0.0.1" "::1"
