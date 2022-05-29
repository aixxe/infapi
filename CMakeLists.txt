cmake_minimum_required(VERSION 3.22)
project(infapi VERSION 0.1.0.0 LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(spdlog
    URL              https://github.com/gabime/spdlog/archive/refs/tags/v1.10.0.zip
    URL_HASH         SHA256=7be28ff05d32a8a11cfba94381e820dd2842835f7f319f843993101bcab44b66
)

FetchContent_Declare(minhook
    URL              https://github.com/TsudaKageyu/minhook/archive/4a455528f61b5a375b1f9d44e7d296d47f18bb18.zip
    URL_HASH         SHA256=aef7b67a907cc28d52e46fa8398ba3c6cb5f8318b50cae81a541cac40ea73c3e
)

FetchContent_Declare(restinio
    URL              https://github.com/Stiffstream/restinio/releases/download/v.0.6.15/restinio-0.6.15-full.zip
    URL_HASH         SHA256=91c1c10fe95f87bb7a024bbc70703230b331e05c7cb6dbc3e8714ad4a9e828c8
)

FetchContent_Declare(cppcodec
    URL              https://github.com/tplgy/cppcodec/archive/9838f9eaf077e42121cb42361e9a1613901fc5e8.zip
    URL_HASH         SHA256=1c8a0fd61a0f76eb1ff0b66a666b9225f6bdf8f58093a713401e17faa479c2dc
)

FetchContent_Declare(expected-lite
    URL              https://github.com/martinmoene/expected-lite/archive/refs/tags/v0.6.0.zip
    URL_HASH         SHA256=864837755ea8c6d34248ffd8da6ce0882e4ee0083510718850814ba83d77db2b
)

FetchContent_Declare(memory_signature
    URL              https://github.com/JustasMasiulis/memory_signature/archive/544b4bfe8c5fa4faf421a62b008ff883908fed9e.zip
    URL_HASH         SHA256=b62512f6d7667bf691ebbe9ee94b8d6fd0519821551837d57776dfa8b790f265
)

FetchContent_Populate(spdlog)
FetchContent_Populate(minhook)
FetchContent_Populate(restinio)
FetchContent_Populate(cppcodec)
FetchContent_Populate(expected-lite)
FetchContent_Populate(memory_signature)

add_subdirectory(${spdlog_SOURCE_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(${minhook_SOURCE_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(${restinio_SOURCE_DIR}/dev EXCLUDE_FROM_ALL)
add_subdirectory(${restinio_SOURCE_DIR}/dev/fmt EXCLUDE_FROM_ALL)
add_subdirectory(${restinio_SOURCE_DIR}/dev/json_dto EXCLUDE_FROM_ALL)
add_subdirectory(${restinio_SOURCE_DIR}/dev/nodejs/http_parser EXCLUDE_FROM_ALL)
add_subdirectory(${expected-lite_SOURCE_DIR} EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME}_launcher
    src/launcher.cpp
)

target_link_libraries(${PROJECT_NAME}_launcher spdlog)
target_compile_features(${PROJECT_NAME}_launcher PUBLIC cxx_std_23)
set_target_properties(${PROJECT_NAME}_launcher PROPERTIES OUTPUT_NAME "infapi")

add_library(${PROJECT_NAME} SHARED
    src/eacapi.cpp
    src/server.cpp
    src/imports/avs2.cpp
    src/imports/eacnet.cpp
    src/methods/decodeResponse.cpp
    src/methods/encodeRequest.cpp
    src/methods/setEacnetClock.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0601)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${cppcodec_SOURCE_DIR}
    ${memory_signature_SOURCE_DIR}/include
    ${restinio_SOURCE_DIR}/dev/asio/include
    ${restinio_SOURCE_DIR}/dev/rapidjson/include
)

target_link_directories(${PROJECT_NAME} PRIVATE lib)

target_link_libraries(${PROJECT_NAME}
    ws2_32
    mswsock
    minhook
    avs2-core
    json-dto::json-dto
    restinio::restinio
)

target_precompile_headers(${PROJECT_NAME} PRIVATE
    src/server.hpp
    src/imports/avs2.hpp
)

if (NOT MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)
    set_target_properties(${PROJECT_NAME}_launcher PROPERTIES LINK_FLAGS_RELEASE -s)
    target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ -static)
    target_link_options(${PROJECT_NAME}_launcher PRIVATE -static-libgcc -static-libstdc++ -static)
endif (NOT MSVC)