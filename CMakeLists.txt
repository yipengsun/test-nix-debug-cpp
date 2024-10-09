cmake_minimum_required(VERSION 3.27...3.29)

project(test-nix-debug-cpp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_executable(main "main.cpp")
