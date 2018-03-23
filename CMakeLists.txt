cmake_minimum_required(VERSION 2.8)
project( imgdiff )

find_package( OpenCV REQUIRED )
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

add_executable( imgdiff main.cpp)

target_link_libraries( imgdiff ${OpenCV_LIBS} )
target_link_libraries(imgdiff ${JSONCPP_LIBRARIES})