cmake_minimum_required(VERSION 3.5)

project(uncrc VERSION 0.2)

find_package(Threads)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
    main.cpp)

target_sources(${PROJECT_NAME}
    PRIVATE
    crc32fast.cpp)
  
target_link_libraries(${PROJECT_NAME}
    PUBLIC 
    Threads::Threads)
