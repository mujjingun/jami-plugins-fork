#! /bin/bash
# Build the plugin for the project
if [ -z $DAEMON ]; then
    DAEMON="./../../daemon"
    echo "DAEMON not provided, building for ${DAEMON}"
fi

PLUGIN_NAME="foregroungsegmentation"
JPL_FILE_NAME=${PLUGIN_NAME}".jpl"
SO_FILE_NAME="lib"${PLUGIN_NAME}".so"
DAEMON_SRC="${DAEMON}/src"
CONTRIB_PATH="${DAEMON}/contrib"
# DESTINATION_PATH=/home/${USER}/Projects/ring-plugins
DESTINATION_PATH="./../build/"
PLUGINS_LIB="../lib"
LIBS_DIR="/home/${USER}/Libs"

CONTRIB_PLATFORM_CURT=x86_64
CONTRIB_PLATFORM=${CONTRIB_PLATFORM_CURT}-linux-gnu

mkdir -p lib/${CONTRIB_PLATFORM_CURT}
mkdir -p ${DESTINATION_PATH}/${CONTRIB_PLATFORM}/jpl

# Compile
clang++ -std=c++14 -shared -fPIC \
-Wl,-Bsymbolic \
-Wall -Wextra \
-Wno-unused-variable \
-Wno-unused-function \
-Wno-unused-parameter \
-I"." \
-I${DAEMON_SRC} \
-I"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/include" \
-I"${CONTRIB_PATH}/${CONTRIB_PLATFORM}/include/opencv4" \
-I${LIBS_DIR}/_tensorflow_distribution/include/flatbuffers \
-I${LIBS_DIR}/_tensorflow_distribution/include \
-I${PLUGINS_LIB} \
main.cpp \
videoSubscriber.cpp \
pluginProcessor.cpp \
pluginMediaHandler.cpp \
TFInference.cpp \
pluginInference.cpp \
pluginParameters.cpp \
-L${CONTRIB_PATH}/${CONTRIB_PLATFORM}/lib/ \
-L${LIBS_DIR}/_tensorflow_distribution/lib/${CONTRIB_PLATFORM}/ \
-lswscale \
-lavutil \
-lopencv_imgcodecs \
-lopencv_imgproc \
-lopencv_core \
-ltensorflowlite \
-o lib/${CONTRIB_PLATFORM_CURT}/${SO_FILE_NAME}
# (above) Always put opencv_core after all other opencv libs when linking statically

cp ${LIBS_DIR}/_tensorflow_distribution/lib/${CONTRIB_PLATFORM}/libtensorflowlite.so lib/$CONTRIB_PLATFORM_CURT

zip -r ${JPL_FILE_NAME} data manifest.json lib
mv ${JPL_FILE_NAME} ${DESTINATION_PATH}/${CONTRIB_PLATFORM}/jpl/

# Cleanup
# Remove lib after compilation
rm -rf lib
