cmake_minimum_required(VERSION 3.5)

project(Curlse LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Curlse
    main.cpp
    http.cpp
    json.cpp
    )
target_include_directories(Curlse PRIVATE .)

install(TARGETS Curlse
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
#find_library(LIBZIP_LIBRARY NAMES libzip libzip_static)
#find_library(LIBJSONCPP_LIBRARY NAMES libjsoncpp libjsoncpp_static )
#find_package(LIBZIP MODULE REQUIRED)
#find_package(LIBJSONCPP MODULE REQUIRED)

#target_link_libraries( Curlse "${LIBZIP_LIBRARY}" "${LIBJSONCPP_LIBRARY}")
find_library( ZIP_LIB zip)
find_library( JSONCPP_LIB jsoncpp)
find_library( CURL_LIB curl)
target_link_libraries( Curlse PRIVATE "${ZIP_LIB}" "${JSONCPP_LIB}" "${CURL_LIB}")
