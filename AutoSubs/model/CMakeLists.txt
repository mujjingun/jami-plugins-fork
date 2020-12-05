set (PLUGINS_LIB ${PROJECT_SOURCE_DIR}/../lib)

cmake_policy(SET CMP0111 NEW)
set(Torch_DIR "${PROJECT_SOURCE_DIR}/libs/libtorch/share/cmake/Torch")
find_package(Torch REQUIRED)
message(NOTICE "Torch found: ${TORCH_LIBRARIES}")

cmake_policy(SET CMP0074 NEW)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(libfvad_ROOT "${PROJECT_SOURCE_DIR}/libs/libfvad")
find_package(libfvad REQUIRED)
message(NOTICE "libfvad found: ${libfvad_LIBRARIES}")
 
add_library(ASRModel STATIC
    asrmodel.cpp 
    codes.cpp
    feat/feature-fbank.cc
    feat/mel-computations.cc
    feat/srfft.cc
    feat/utils.cpp
    feat/online-feature.cc
    feat/feature-window.cc
)

target_link_libraries(ASRModel PRIVATE ${TORCH_LIBRARIES} ${libfvad_LIBRARIES})
target_include_directories(ASRModel PRIVATE ${PLUGINS_LIB} ${TORCH_INCLUDE_DIRS} ${libfvad_INCLUDE_DIRS})
set_property(TARGET ASRModel PROPERTY POSITION_INDEPENDENT_CODE ON)

target_compile_options(ASRModel PRIVATE)

if(UNIX)
    add_subdirectory(test)
endif()