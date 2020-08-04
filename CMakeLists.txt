cmake_minimum_required(VERSION 2.8.4)
project("DataHelper")

set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC")
set(CMKAE_C_FLAGS "-std=c++11 -fPIC")

find_package(Threads)
set(BASELIB_HEAD "${PROJECT_SOURCE_DIR}/../BaseLib/" CACHE INTERNAL "BaseLib Header_file localtion")
set(BASELIB_LIBRARY "${PROJECT_SOURCE_DIR}/../BaseLib/lib" CACHE INTERNAL "BaseLib Header_file localtion")

IF(WIN32)
set(BASELIB_LIBRARY "${PROJECT_SOURCE_DIR}/../BaseLib/lib" CACHE INTERNAL "BaseLib Header_file localtion")
ENDIF()

include_directories(${BASELIB_HEAD} ./mysql/include)
link_directories(${BASELIB_LIBRARY} ./mysql/lib)


include_directories(. Src)
aux_source_directory(Src DIR_SRCS)

add_executable(DataHelper main.cpp ${DIR_SRCS})

target_link_libraries(DataHelper hrRobotPub)
target_link_libraries(DataHelper BaseLib)
target_link_libraries(DataHelper mysql)
target_link_libraries(DataHelper ${CMAKE_THREAD_LIBS_INIT})


