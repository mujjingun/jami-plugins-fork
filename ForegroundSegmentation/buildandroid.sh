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
if [ -z $ANDROID_NDK ]; then
	ANDROID_NDK=/home/${USER}/Android/Sdk/ndk/21.1.6352462
    echo "ANDROID_NDK not provided, building with ${ANDROID_NDK}"
fi

PLUGIN_NAME="foregroundsegmentation"
JPL_FILE_NAME=${PLUGIN_NAME}".jpl"
SO_FILE_NAME="lib"${PLUGIN_NAME}".so"
LIBS_DIR="/home/${USER}/Libs"
DAEMON_SRC="${DAEMON}/src"
CONTRIB_PATH="${DAEMON}/contrib"
DESTINATION_PATH="./../build/"
PLUGINS_LIB="../lib"

#=========================================================
#	Check if the ANDROID_ABI was provided
#	if not, set default
#=========================================================
if [ -z $ANDROID_ABI ]; then
    ANDROID_ABI="armeabi-v7a arm64-v8a"
    echo "ANDROID_ABI not provided, building for ${ANDROID_ABI}"
fi

buildlib() {
	echo $CURRENT_ABI
	#=========================================================
	#	ANDROID TOOLS
	#=========================================================
	export HOST_TAG=linux-x86_64
	export TOOLCHAIN=$ANDROID_NDK/toolchains/llvm/prebuilt/$HOST_TAG

	if [ $CURRENT_ABI = armeabi-v7a ]
	then
	export AR=$TOOLCHAIN/bin/arm-linux-android-ar
	export AS=$TOOLCHAIN/bin/arm-linux-android-as
	export CC=$TOOLCHAIN/bin/armv7a-linux-androideabi21-clang
	export CXX=$TOOLCHAIN/bin/armv7a-linux-androideabi21-clang++
	export LD=$TOOLCHAIN/bin/arm-linux-android-ld
	export RANLIB=$TOOLCHAIN/bin/arm-linux-android-ranlib
	export STRIP=$TOOLCHAIN/bin/arm-linux-androideabi-strip
	export ANDROID_SYSROOT=./../../client-android/android-toolchain-21-arm/sysroot

	elif [ $CURRENT_ABI = arm64-v8a ]
	then
	export AR=$TOOLCHAIN/bin/aarch64-linux-android-ar
	export AS=$TOOLCHAIN/bin/aarch64-linux-android-as
	export CC=$TOOLCHAIN/bin/aarch64-linux-android21-clang
	export CXX=$TOOLCHAIN/bin/aarch64-linux-android21-clang++
	export LD=$TOOLCHAIN/bin/aarch64-linux-android-ld
	export RANLIB=$TOOLCHAIN/bin/aarch64-linux-android-ranlib
	export STRIP=$TOOLCHAIN/bin/aarch64-linux-android-strip
	export ANDROID_SYSROOT=./../../client-android/android-toolchain-21-arm64/sysroot

	elif [ $CURRENT_ABI = x86_64 ]
	then
	export AR=$TOOLCHAIN/bin/x86_64-linux-android-ar
	export AS=$TOOLCHAIN/bin/x86_64-linux-android-as
	export CC=$TOOLCHAIN/bin/x86_64-linux-android21-clang
	export CXX=$TOOLCHAIN/bin/x86_64-linux-android21-clang++
	export LD=$TOOLCHAIN/bin/x86_64-linux-android-ld
	export RANLIB=$TOOLCHAIN/bin/x86_64-linux-android-ranlib
	export STRIP=$TOOLCHAIN/bin/x86_64-linux-android-strip
	export ANDROID_SYSROOT=./../../client-android/android-toolchain-21-x86_64/sysroot

	else
	echo "ABI NOT OK" >&2
	exit 1
	fi

	#=========================================================
	#	CONTRIBS
	#=========================================================
	if [ $CURRENT_ABI = armeabi-v7a ]
	then
	CONTRIB_PLATFORM=arm-linux-androideabi

	elif [ $CURRENT_ABI = arm64-v8a ]
	then
	CONTRIB_PLATFORM=aarch64-linux-android

	elif [ $CURRENT_ABI = x86_64 ]
	then
	CONTRIB_PLATFORM=x86_64-linux-android
	fi

	# ASSETS
	ANDROID_PROJECT_ASSETS=./../../client-android/ring-android/app/src/main/assets
	# LIBS FOLDER
	ANDROID_PROJECT_LIBS=./../../client-android/ring-android/app/src/main/libs/$CURRENT_ABI
	#NDK SOURCES FOR cpufeatures
	NDK_SOURCES=${ANDROID_NDK}/sources/android

	#=========================================================
	#	LD_FLAGS
	#=========================================================
	if [ $CURRENT_ABI = armeabi-v7a ]
	then
	export EXTRA_LDFLAGS="${EXTRA_LDFLAGS} -L${ANDROID_SYSROOT}/usr/lib/arm-linux-androideabi -L${ANDROID_SYSROOT}/usr/lib/arm-linux-androideabi/21"
	elif [ $CURRENT_ABI = arm64-v8a ]
	then
	export EXTRA_LDFLAGS="${EXTRA_LDFLAGS} -L${ANDROID_SYSROOT}/usr/lib/aarch64-linux-android -L${ANDROID_SYSROOT}/usr/lib/aarch64-linux-android/21"
	elif [ $CURRENT_ABI = x86_64 ]
	then
	export EXTRA_LDFLAGS="${EXTRA_LDFLAGS} -L${ANDROID_SYSROOT}/usr/lib/x86_64-linux-android -L${ANDROID_SYSROOT}/usr/lib/x86_64-linux-android/21"
	fi

	#=========================================================
	#	Compile CPU FEATURES, NEEDED FOR OPENCV
	#=========================================================
	$CC -c $NDK_SOURCES/cpufeatures/cpu-features.c -o cpu-features.o -o cpu-features.o --sysroot=$ANDROID_SYSROOT

	#=========================================================
	#	Compile the plugin
	#=========================================================

	# Create so destination folder
	mkdir -p lib/$CURRENT_ABI

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
	cpu-features.o \
	-L${CONTRIB_PATH}/${CONTRIB_PLATFORM}/lib/ \
	-L${LIBS_DIR}/_tensorflow_distribution/lib/${CURRENT_ABI}/ \
	-lswscale \
	-lavutil \
	-lopencv_imgcodecs \
	-lopencv_imgproc \
	-lopencv_core \
    -llibpng \
    -ltensorflowlite \
	-llog -lz \
	--sysroot=$ANDROID_SYSROOT \
	-o lib/$CURRENT_ABI/${SO_FILE_NAME}
	# (above) Always put opencv_core after all other opencv libs when linking statically
	# (above) Put libavutil after other ffmpeg libraries

	cp ${LIBS_DIR}/_tensorflow_distribution/lib/${CURRENT_ABI}/libtensorflowlite.so lib/$CURRENT_ABI
}

# Build the so
for i in ${ANDROID_ABI}; do
	CURRENT_ABI=$i
	buildlib
done

#Export the plugin data folder
mkdir -p ${DESTINATION_PATH}/jpl/${PLUGIN_NAME}/
zip -r ${JPL_FILE_NAME} data manifest.json lib
mv ${JPL_FILE_NAME} ${DESTINATION_PATH}/jpl/${PLUGIN_NAME}/

# Cleanup
# Remove cpu-features object after compilation
rm cpu-features.o
rm -rf lib
