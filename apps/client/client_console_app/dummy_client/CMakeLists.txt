cmake_minimum_required(VERSION 3.5)

set(MODULE_NAME dummy_client)

add_library(${MODULE_NAME}
    ${MODULE_NAME}.cpp
    ${MODULE_NAME}.hpp
)
add_library(dummy::client ALIAS ${MODULE_NAME})

target_link_libraries(${MODULE_NAME}
    PRIVATE
        easylogging::easylogging
        modules::network
)
target_include_directories(${MODULE_NAME}
    PRIVATE 
        ${CMAKE_BINARY_DIR}/third_party/nlohmann_json/
        ../../../../modules/network_module
)