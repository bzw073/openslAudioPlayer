project(OpenSLPlayer)

cmake_minimum_required(VERSION 3.4.1)

include_directories(include)

add_library(OpenSLPlayer SHARED OpenSLPlayer.cpp
        AudioPlayer.cpp)

find_library(log-lib log .)

#链接OpenSLES库和log库
target_link_libraries(OpenSLPlayer OpenSLES log)