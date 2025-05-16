cmake_minimum_required(VERSION 3.18)

project(HDDLed)

set(SOURCE_HDDLED src/HDDLed.cpp)
set(SOURCE_CFGINI src/cfgini.cpp)
set(SOURCE_LOG src/Log.cpp)
set(SOURCE_SACT src/showActivity.cpp)

include_directories(include)

add_library(cfgini STATIC ${SOURCE_CFGINI})
add_library(log STATIC ${SOURCE_LOG})
add_library(showActivity STATIC ${SOURCE_SACT})

add_executable(hddled ${SOURCE_HDDLED})

target_compile_options(hddled PRIVATE -Wall -O2 -Ofast)
target_link_libraries(hddled cfgini log showActivity wiringPi rt)

install(TARGETS hddled DESTINATION bin)
install(FILES ${CMAKE_SOURCE_DIR}/config/hddled.ini.default DESTINATION /usr/local/etc)
configure_file(${CMAKE_SOURCE_DIR}/service/hddled.service.in ${CMAKE_BINARY_DIR}/hddled.service @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/hddled.service DESTINATION /etc/systemd/system)