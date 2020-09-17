#! /bin/bash
# Build the plugin for the project
export OSTYPE
ARCH=$(arch)

# Flags:

  # -p: number of processors to use
  # -c: Runtime plugin cpu/gpu setting.
  # -t: target platform.


if [ -z "${DAEMON}" ]; then
    DAEMON="./../../daemon"
    echo "DAEMON not provided, building with ${DAEMON}"
fi

PLUGIN_NAME="GreenScreen"
JPL_FILE_NAME="${PLUGIN_NAME}.jpl"
SO_FILE_NAME="lib${PLUGIN_NAME}.so"
DAEMON_SRC="${DAEMON}/src"
CONTRIB_PATH="${DAEMON}/contrib"
PLUGINS_LIB="../lib"
LIBS_DIR="./../contrib/Libs"

if [ -z "${TF_LIBS_DIR}" ]; then
    TF_LIBS_DIR="./../../../Libs"
fi
echo "Building with ${TF_LIBS_DIR}"

PLATFORM="linux-gnu"
PROCESSOR='GPU'

while getopts t:c:p OPT; do
  case "$OPT" in
    t)
      PLATFORM="${OPTARG}"
      if [ -z "${TF}" ]; then
          if [ "$PLATFORM" = 'linux-gnu' ]; then
              TF="_tensorflow_cc"
          elif [ "$PLATFORM" = 'android' ]; then
              TF="_tensorflowLite"
          fi
      fi
    ;;
    c)
      PROCESSOR="${OPTARG}"
    ;;
    p)
    ;;
    \?)
      exit 1
    ;;
  esac
done


if [ -z "${TF}" ]; then
    TF="_tensorflow_cc"
fi
echo "Building with ${TF}"
mkdir ./data/models


if [[ "${TF}" = "_tensorflow_cc" ]] && [[ "${PLATFORM}" = "linux-gnu" ]]
then
    if [ -z "$CUDALIBS" ]; then
        rm -r ./data/models
        echo "CUDALIBS must point to CUDA 10.1!"
        exit
    fi
    if [ -z "$CUDNN" ]; then
        rm -r ./data/models
        echo "CUDNN must point to libcudnn.so 7!"
        exit
    fi

    echo "Building for ${PROCESSOR}"

    CONTRIB_PLATFORM_CURT=${ARCH}
    CONTRIB_PLATFORM=${CONTRIB_PLATFORM_CURT}-${PLATFORM}
    DESTINATION_PATH="./../build/${CONTRIB_PLATFORM}/${TF}"
    mkdir -p "lib/${CONTRIB_PLATFORM}"
    mkdir -p "${DESTINATION_PATH}"


    # Compile
    clang++ -std=c++17 -shared -fPIC \
    -Wl,-Bsymbolic,-rpath,"\${ORIGIN}" \
    -Wall -Wextra \
    -Wno-unused-variable \
    -Wno-unused-function \
    -Wno-unused-parameter \
    -D"${PROCESSOR}" \
    -I"." \
    -I"${DAEMON_SRC}" \
    -I"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/include" \
    -I"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/include/opencv4" \
    -I"${LIBS_DIR}/${TF}/include" \
    -I"${LIBS_DIR}/${TF}/include/third_party/eigen3" \
    -I"${PLUGINS_LIB}" \
    main.cpp \
    videoSubscriber.cpp \
    pluginProcessor.cpp \
    pluginMediaHandler.cpp \
    TFInference.cpp \
    pluginInference.cpp \
    pluginParameters.cpp \
    -L"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/lib/" \
    -L"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/lib/opencv4/3rdparty/" \
    -L"${TF_LIBS_DIR}/${TF}/lib/${CONTRIB_PLATFORM}-gpu61/" \
    -lswscale \
    -lavutil \
    -lopencv_imgcodecs \
    -lopencv_imgproc \
    -lopencv_core \
    -ltensorflow_cc \
    -lpng \
    -o "lib/${CONTRIB_PLATFORM}/${SO_FILE_NAME}"

    cp "${TF_LIBS_DIR}/${TF}/lib/${CONTRIB_PLATFORM}-gpu61/libtensorflow_cc.so" "lib/$CONTRIB_PLATFORM/libtensorflow_cc.so.2"
    cp "/usr/lib/${CONTRIB_PLATFORM}/libswscale.so.4" "lib/$CONTRIB_PLATFORM"
    cp "/usr/lib/${CONTRIB_PLATFORM}/libavutil.so.55" "lib/$CONTRIB_PLATFORM"
    cp "/usr/lib/${CONTRIB_PLATFORM}/libpng16.so.16" "lib/$CONTRIB_PLATFORM"
    cp "${CUDALIBS}/libcudart.so" "lib/$CONTRIB_PLATFORM/libcudart.so.10.0"
    cp "${CUDNN}/libcublas.so.10" "lib/$CONTRIB_PLATFORM/libcublas.so.10.0"
    cp "${CUDALIBS}/libcufft.so.10" "lib/$CONTRIB_PLATFORM/libcufft.so.10.0"
    cp "${CUDALIBS}/libcurand.so.10" "lib/$CONTRIB_PLATFORM/libcurand.so.10.0"
    cp "${CUDALIBS}/libcusolver.so.10" "lib/$CONTRIB_PLATFORM/libcusolver.so.10.0"
    cp "${CUDALIBS}/libcusparse.so.10" "lib/$CONTRIB_PLATFORM/libcusparse.so.10.0"
    cp "${CUDNN}/libcudnn.so.7" "lib/$CONTRIB_PLATFORM"

    cp ./modelsSRC/mModel-resnet50float.pb ./data/models/mModel.pb
    cp ./preferences-tfcc.json ./data/preferences.json
elif [ "${TF}" = "_tensorflowLite" ]
then
    if [ "${PLATFORM}" = "linux-gnu" ]
    then
        CONTRIB_PLATFORM_CURT=${ARCH}
        CONTRIB_PLATFORM=${CONTRIB_PLATFORM_CURT}-${PLATFORM}
        DESTINATION_PATH="./../build/${CONTRIB_PLATFORM}/${TF}"
        mkdir -p "lib/${CONTRIB_PLATFORM}"
        mkdir -p "${DESTINATION_PATH}"

        # Compile
        clang++ -std=c++17 -shared -fPIC \
        -Wl,-Bsymbolic,-rpath,"\${ORIGIN}" \
        -Wall -Wextra \
        -Wno-unused-variable \
        -Wno-unused-function \
        -Wno-unused-parameter \
        -DTFLITE \
        -I"." \
        -I"${DAEMON_SRC}" \
        -I"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/include" \
        -I"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/include/opencv4" \
        -I"${LIBS_DIR}/${TF}/include/flatbuffers" \
        -I"${LIBS_DIR}/${TF}/include" \
        -I"${PLUGINS_LIB}" \
        main.cpp \
        videoSubscriber.cpp \
        pluginProcessor.cpp \
        pluginMediaHandler.cpp \
        TFInference.cpp \
        pluginInference.cpp \
        pluginParameters.cpp \
        -L"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/lib/" \
        -L"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/lib/opencv4/3rdparty/" \
        -L"${TF_LIBS_DIR}/${TF}/lib/${CONTRIB_PLATFORM}/" \
        -lswscale \
        -lavutil \
        -lopencv_imgcodecs \
        -lopencv_imgproc \
        -lopencv_core \
        -ltensorflowlite \
        -lpng \
        -o "lib/${CONTRIB_PLATFORM}/${SO_FILE_NAME}"

        cp "${TF_LIBS_DIR}/${TF}/lib/${CONTRIB_PLATFORM}/libtensorflowlite.so" "lib/$CONTRIB_PLATFORM"
        cp "/usr/lib/${CONTRIB_PLATFORM}/libswscale.so.4" "lib/$CONTRIB_PLATFORM"
        cp "/usr/lib/${CONTRIB_PLATFORM}/libavutil.so.55" "lib/$CONTRIB_PLATFORM"
        cp "/usr/lib/${CONTRIB_PLATFORM}/libpng16.so.16" "lib/$CONTRIB_PLATFORM"

    elif [ "${PLATFORM}" = "android" ]
    then
        DESTINATION_PATH="./../build/android"
        mkdir -p "${DESTINATION_PATH}"

        if [ -z "$ANDROID_NDK" ]; then
             ANDROID_NDK="/home/${USER}/Android/Sdk/ndk/21.1.6352462"
            echo "ANDROID_NDK not provided, building with ${ANDROID_NDK}"
        fi

        #=========================================================
        #    Check if the ANDROID_ABI was provided
        #    if not, set default
        #=========================================================
        if [ -z "$ANDROID_ABI" ]; then
            ANDROID_ABI="armeabi-v7a arm64-v8a"
            echo "ANDROID_ABI not provided, building for ${ANDROID_ABI}"
        fi

        buildlib() {
            echo "$CURRENT_ABI"
            mkdir -p "lib/$CURRENT_ABI"

            #=========================================================
            #    ANDROID TOOLS
            #=========================================================
            export HOST_TAG=linux-x86_64
            export TOOLCHAIN=$ANDROID_NDK/toolchains/llvm/prebuilt/$HOST_TAG

            if [ "$CURRENT_ABI" = armeabi-v7a ]
            then
            export AR=$TOOLCHAIN/bin/arm-linux-android-ar
            export AS=$TOOLCHAIN/bin/arm-linux-android-as
            export CC=$TOOLCHAIN/bin/armv7a-linux-androideabi21-clang
            export CXX=$TOOLCHAIN/bin/armv7a-linux-androideabi21-clang++
            export LD=$TOOLCHAIN/bin/arm-linux-android-ld
            export RANLIB=$TOOLCHAIN/bin/arm-linux-android-ranlib
            export STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip
            export ANDROID_SYSROOT=${DAEMON}/../client-android/android-toolchain-21-arm/sysroot

            elif [ "$CURRENT_ABI" = arm64-v8a ]
            then
            export AR=$TOOLCHAIN/bin/aarch64-linux-android-ar
            export AS=$TOOLCHAIN/bin/aarch64-linux-android-as
            export CC=$TOOLCHAIN/bin/aarch64-linux-android21-clang
            export CXX=$TOOLCHAIN/bin/aarch64-linux-android21-clang++
            export LD=$TOOLCHAIN/bin/aarch64-linux-android-ld
            export RANLIB=$TOOLCHAIN/bin/aarch64-linux-android-ranlib
            export STRIP=$TOOLCHAIN/bin/aarch64-linux-android-strip
            export ANDROID_SYSROOT=${DAEMON}/../client-android/android-toolchain-21-arm64/sysroot

            elif [ "$CURRENT_ABI" = x86_64 ]
            then
            export AR=$TOOLCHAIN/bin/x86_64-linux-android-ar
            export AS=$TOOLCHAIN/bin/x86_64-linux-android-as
            export CC=$TOOLCHAIN/bin/x86_64-linux-android21-clang
            export CXX=$TOOLCHAIN/bin/x86_64-linux-android21-clang++
            export LD=$TOOLCHAIN/bin/x86_64-linux-android-ld
            export RANLIB=$TOOLCHAIN/bin/x86_64-linux-android-ranlib
            export STRIP=$TOOLCHAIN/bin/x86_64-linux-android-strip
            export ANDROID_SYSROOT=${DAEMON}/../client-android/android-toolchain-21-x86_64/sysroot

            else
            echo "ABI NOT OK" >&2
            rm -r lib/
            rm -r ./data/models
            exit 1
            fi

            #=========================================================
            #    CONTRIBS
            #=========================================================
            if [ "$CURRENT_ABI" = armeabi-v7a ]
            then
            CONTRIB_PLATFORM=arm-linux-androideabi

            elif [ "$CURRENT_ABI" = arm64-v8a ]
            then
            CONTRIB_PLATFORM=aarch64-linux-android

            elif [ "$CURRENT_ABI" = x86_64 ]
            then
            CONTRIB_PLATFORM=x86_64-linux-android
            fi

            #NDK SOURCES FOR cpufeatures
            NDK_SOURCES=${ANDROID_NDK}/sources/android

            #=========================================================
            #    LD_FLAGS
            #=========================================================
            if [ "$CURRENT_ABI" = armeabi-v7a ]
            then
            export EXTRA_LDFLAGS="${EXTRA_LDFLAGS} -L${ANDROID_SYSROOT}/usr/lib/arm-linux-androideabi -L${ANDROID_SYSROOT}/usr/lib/arm-linux-androideabi/21"
            elif [ "$CURRENT_ABI" = arm64-v8a ]
            then
            export EXTRA_LDFLAGS="${EXTRA_LDFLAGS} -L${ANDROID_SYSROOT}/usr/lib/aarch64-linux-android -L${ANDROID_SYSROOT}/usr/lib/aarch64-linux-android/21"
            elif [ "$CURRENT_ABI" = x86_64 ]
            then
            export EXTRA_LDFLAGS="${EXTRA_LDFLAGS} -L${ANDROID_SYSROOT}/usr/lib/x86_64-linux-android -L${ANDROID_SYSROOT}/usr/lib/x86_64-linux-android/21"
            fi

            #=========================================================
            #    Compile CPU FEATURES, NEEDED FOR OPENCV
            #=========================================================
            $CC -c "$NDK_SOURCES/cpufeatures/cpu-features.c" -o cpu-features.o -o cpu-features.o --sysroot=$ANDROID_SYSROOT

            #=========================================================
            #    Compile the plugin
            #=========================================================

            # Create so destination folder
            $CXX --std=c++14 -O3 -g -fPIC \
            -Wl,-Bsymbolic,-rpath,"\${ORIGIN}" \
            -shared \
            -Wall -Wextra \
            -Wno-unused-variable \
            -Wno-unused-function \
            -Wno-unused-parameter \
            -DTFLITE \
            -I"." \
            -I"${DAEMON_SRC}" \
            -I"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/include" \
            -I"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/include/opencv4" \
            -I"${LIBS_DIR}/${TF}/include/flatbuffers" \
            -I"${LIBS_DIR}/${TF}/include" \
            -I"${PLUGINS_LIB}" \
            main.cpp \
            videoSubscriber.cpp \
            pluginProcessor.cpp \
            pluginMediaHandler.cpp \
            TFInference.cpp \
            pluginInference.cpp \
            pluginParameters.cpp \
            cpu-features.o \
            -L"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/lib/" \
            -L"${TF_LIBS_DIR}/${TF}/lib/${CURRENT_ABI}/" \
            -lswscale \
            -lavutil \
            -lopencv_imgcodecs \
            -lopencv_imgproc \
            -lopencv_core \
            -llibpng \
            -ltensorflowlite \
            -llog -lz \
            --sysroot=$ANDROID_SYSROOT \
            -o "lib/$CURRENT_ABI/${SO_FILE_NAME}"

            cp "${TF_LIBS_DIR}/${TF}/lib/${CURRENT_ABI}/libtensorflowlite.so" "lib/$CURRENT_ABI"
            rm cpu-features.o
        }

        # Build the so
        for i in ${ANDROID_ABI}; do
            CURRENT_ABI=$i
            buildlib
        done
    fi

    cp ./modelsSRC/mobilenet_v2_deeplab_v3_256_myquant.tflite ./data/models/mModel.tflite
    cp ./preferences-tflite.json ./data/preferences.json
fi

zip -r ${JPL_FILE_NAME} data manifest.json lib
mv ${JPL_FILE_NAME} ${DESTINATION_PATH}/

# Cleanup
# Remove lib after compilation
rm -rf lib
rm -r ./data/models
rm ./data/preferences.json

