# /*
#  *  Copyright (C) 2004-2020 Savoir-faire Linux Inc.
#  *
#  *  Author: Aline Gondim Santos <aline.gondimsantos@savoirfairelinux.com>
#  *
#  *  This program is free software; you can redistribute it and/or modify
#  *  it under the terms of the GNU General Public License as published by
#  *  the Free Software Foundation; either version 3 of the License, or
#  *  (at your option) any later version.
#  *
#  *  This program is distributed in the hope that it will be useful,
#  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  *  GNU General Public License for more details.
#  *
#  *  You should have received a copy of the GNU General Public License
#  *  along with this program; if not, write to the Free Software
#  *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.
#  */
 
#! /bin/bash
# Build the plugin for the project
if [ -z $DAEMON ]; then
    DAEMON="./../../daemon"
    echo "DAEMON not provided, building for ${DAEMON}"
fi

PLUGIN_NAME="foregroundsegmentation"
JPL_FILE_NAME=${PLUGIN_NAME}".jpl"
SO_FILE_NAME="lib"${PLUGIN_NAME}".so"
DAEMON_SRC="${DAEMON}/src"
CONTRIB_PATH="${DAEMON}/contrib"
DESTINATION_PATH="./../build/"
PLUGINS_LIB="../lib"
LIBS_DIR="/home/${USER}/Libs"


CONTRIB_PLATFORM_CURT=x86_64
CONTRIB_PLATFORM=${CONTRIB_PLATFORM_CURT}-linux-gnu

mkdir -p lib/${CONTRIB_PLATFORM_CURT}
mkdir -p ${DESTINATION_PATH}/${CONTRIB_PLATFORM}/jpl

# Compile
clang++ -std=c++14 -shared -fPIC \
-Wl,-Bsymbolic,-rpath,"\${ORIGIN}" \
-Wall -Wextra \
-Wno-unused-variable \
-Wno-unused-function \
-Wno-unused-parameter \
-DTFLITE \
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
-L${CONTRIB_PATH}/${CONTRIB_PLATFORM}/lib/opencv4/3rdparty/ \
-L${LIBS_DIR}/_tensorflow_distribution/lib/${CONTRIB_PLATFORM}/ \
-lswscale \
-lavutil \
-lopencv_imgcodecs \
-lopencv_imgproc \
-lopencv_core \
-ltensorflowlite \
-lpng \
-o lib/${CONTRIB_PLATFORM_CURT}/${SO_FILE_NAME}
# (above) Always put opencv_core after all other opencv libs
# (above) Always put avutil after all other ffmpeg libs
# (above) Always put png after all other libs

cp ${LIBS_DIR}/_tensorflow_distribution/lib/${CONTRIB_PLATFORM}/libtensorflowlite.so lib/$CONTRIB_PLATFORM_CURT
cp /usr/lib/${CONTRIB_PLATFORM}/libswscale.so.4 lib/$CONTRIB_PLATFORM_CURT
cp /usr/lib/${CONTRIB_PLATFORM}/libavutil.so.55 lib/$CONTRIB_PLATFORM_CURT
cp /usr/lib/${CONTRIB_PLATFORM}/libpng16.so.16 lib/$CONTRIB_PLATFORM_CURT

zip -r ${JPL_FILE_NAME} data manifest.json lib
mv ${JPL_FILE_NAME} ${DESTINATION_PATH}/${CONTRIB_PLATFORM}/jpl/

# Cleanup
# Remove lib after compilation
rm -rf lib
