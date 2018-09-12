cmake_minimum_required(VERSION 3.8)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(RESTINIO_USE_BOOST_ASIO "none")
SET(RESTINIO_STAND_ALONE_ASIO_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/asio/include )
SET(RESTINIO_STAND_ALONE_ASIO_DEFINES -DASIO_STANDALONE -DASIO_HAS_STD_CHRONO -DASIO_DISABLE_STD_STRING_VIEW)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/link_threads_if_necessary.cmake")

# Add necessary dependecies.
# HTTP parser
add_subdirectory(nodejs/http_parser)

# fmtlib
add_subdirectory(fmt)

# RESTinio
add_subdirectory(restinio)

# The main example code
add_subdirectory(websocket_chat)

