set (PLUGINS_LIB ${PROJECT_SOURCE_DIR}/../lib)

set(Torch_DIR "${PROJECT_SOURCE_DIR}/libs/libtorch/share/cmake/Torch")
find_package(Torch REQUIRED)
message(NOTICE "Torch found: ${TORCH_LIBRARIES}")

find_package(BLAS REQUIRED)
message(NOTICE "BLAS found: ${BLAS_LIBRARIES}")

add_library(ASRModel STATIC
    asrmodel.cpp
    codes.cpp
    feat/feature-fbank.cc
    feat/mel-computations.cc
    feat/srfft.cc
    feat/utils.cpp
)

target_link_libraries(ASRModel PRIVATE ${TORCH_LIBRARIES} ${BLAS_LIBRARIES})
target_include_directories(ASRModel PRIVATE ${PLUGINS_LIB})
set_property(TARGET ASRModel PROPERTY POSITION_INDEPENDENT_CODE ON)


add_subdirectory(test)