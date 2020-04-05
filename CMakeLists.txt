cmake_minimum_required(VERSION 3.5)

project(ttcp LANGUAGES CXX)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

set(APP_EXE roundtrip)

set(TTCP_HEADERS common.h inet_address.h socket.h accept.h tcp_stream.h message.h)

set(TTCP_SRCS socket.cc inet_address.cc accept.cc tcp_stream.cc)

add_library(ttpclib STATIC  ${TTCP_SRCS} ${TTCP_HEADERS})

add_executable(sender sender.cc)

target_link_libraries(sender ttpclib pthread)

add_executable(${APP_EXE} roundtrip.cc)

target_link_libraries(${APP_EXE} ttpclib pthread)



# add_executable(inet_address_test inet_address_test.cc)

# target_link_libraries(inet_address_test ttpclib)

# tcp_stream_test
# add_executable(tcp_stream_test tcp_stream_test.cc)

# target_link_libraries(tcp_stream_test ttpclib)