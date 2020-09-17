#! /bin/bash
# Build Jami daemon for architecture specified by ANDROID_ABI
set -e

tar -xf libs.tar.gz

if [ -z "$ANDROID_NDK" -o -z "$ANDROID_SDK" ]; then
   echo "You must define ANDROID_NDK, ANDROID_SDK and ANDROID_ABI before starting."
   echo "They must point to your NDK and SDK directories."
   exit 1
fi
if [ -z "$ANDROID_ABI" ]; then
   echo "Please set ANDROID_ABI to your architecture: armeabi-v7a, x86."
   exit 1
fi

HAVE_ARM=0
HAVE_X86=0
HAVE_64=0

# Set up ABI variables
if [ ${ANDROID_ABI} = "x86" ] ; then
    TARGET_TUPLE="i686-linux-android"
    PJ_TARGET_TUPLE="i686-pc-linux-android"
    PATH_HOST="x86"
    HAVE_X86=1
    PLATFORM_SHORT_ARCH="x86"
elif [ ${ANDROID_ABI} = "x86_64" ] ; then
    TARGET_TUPLE="x86_64-linux-android"
    PJ_TARGET_TUPLE="x86_64-pc-linux-android"
    PATH_HOST="x86_64"
    HAVE_X86=1
    HAVE_64=1
    PLATFORM_SHORT_ARCH="x86_64"
elif [ ${ANDROID_ABI} = "arm64-v8a" ] ; then
    TARGET_TUPLE="aarch64-linux-android"
    PJ_TARGET_TUPLE="aarch64-unknown-linux-android"
    PATH_HOST=$TARGET_TUPLE
    HAVE_ARM=1
    HAVE_64=1
    PLATFORM_SHORT_ARCH="arm64"
else
    TARGET_TUPLE="arm-linux-androideabi"
    PJ_TARGET_TUPLE="arm-unknown-linux-androideabi"
    PATH_HOST=$TARGET_TUPLE
    HAVE_ARM=1
    PLATFORM_SHORT_ARCH="arm"
fi

if [ "${HAVE_64}" = 1 ];then
    LIBDIR=lib64
else
    LIBDIR=lib
fi
ANDROID_API_VERS=21
ANDROID_API=android-$ANDROID_API_VERS

export ANDROID_TOOLCHAIN="`pwd`/../../client-android/android-toolchain-$ANDROID_API_VERS-$PLATFORM_SHORT_ARCH"
if [ ! -d "$ANDROID_TOOLCHAIN" ]; then
    $ANDROID_NDK/build/tools/make_standalone_toolchain.py \
        --arch=$PLATFORM_SHORT_ARCH \
        --api $ANDROID_API_VERS \
        --stl libc++ \
        --install-dir=$ANDROID_TOOLCHAIN
fi

export ANDROID_API
export TARGET_TUPLE
export HAVE_ARM
export HAVE_X86
export HAVE_64

# Add the NDK toolchain to the PATH, needed both for contribs and for building
# stub libraries
NDK_TOOLCHAIN_PATH="${ANDROID_TOOLCHAIN}/bin"
CROSS_COMPILE=${NDK_TOOLCHAIN_PATH}/${TARGET_TUPLE}-
export PATH=${NDK_TOOLCHAIN_PATH}:${PATH}
export CROSS_COMPILE="${CROSS_COMPILE}"
export SYSROOT=$ANDROID_TOOLCHAIN/sysroot

if [ -z "$DAEMON_DIR" ]; then
    DAEMON_DIR="$(pwd)/../../daemon"
    echo "DAEMON_DIR not provided trying to find it in $DAEMON_DIR"
fi

if [ ! -d "$DAEMON_DIR" ]; then
    echo 'Daemon not found.'
    echo 'If you cloned the daemon in a custom location override' \
            'DAEMON_DIR to point to it'
    echo "You can also use our meta repo which contains both:
          https://review.jami.net/#/admin/projects/ring-project"
    exit 1
fi
export DAEMON_DIR

# Setup LDFLAGS
if [ ${ANDROID_ABI} = "armeabi-v7a" ] ; then
    EXTRA_CFLAGS="${EXTRA_CFLAGS} -march=armv7-a -mthumb -mfpu=vfpv3-d16"
    EXTRA_LDFLAGS="${EXTRA_LDFLAGS} -march=armv7-a -mthumb -mfpu=vfpv3-d16"
elif [ ${ANDROID_ABI} = "arm64-v8a" ] ; then
    EXTRA_LDFLAGS="${EXTRA_LDFLAGS} -L${SYSROOT}/usr/lib -L${ANDROID_TOOLCHAIN}/${TARGET_TUPLE}/lib"
fi
EXTRA_LDFLAGS="${EXTRA_LDFLAGS} -L${ANDROID_TOOLCHAIN}/${TARGET_TUPLE}/${LIBDIR}/${ANDROID_ABI} -L${ANDROID_TOOLCHAIN}/${TARGET_TUPLE}/${LIBDIR}"
EXTRA_CFLAGS="${EXTRA_CFLAGS} -fPIC"
EXTRA_CXXFLAGS="${EXTRA_CXXFLAGS} -fPIC"
EXTRA_LDFLAGS="${EXTRA_LDFLAGS} -L${SYSROOT}/usr/${LIBDIR}"
echo "EXTRA_CFLAGS= ${EXTRA_CFLAGS}" >> config.mak
echo "EXTRA_CXXFLAGS= ${EXTRA_CXXFLAGS}" >> config.mak
echo "EXTRA_LDFLAGS= ${EXTRA_LDFLAGS}" >> config.mak

echo "Daemon in debug mode."
OPTS="--enable-debug"

# Make in //
UNAMES=$(uname -s)
MAKEFLAGS=
if which nproc >/dev/null
then
MAKEFLAGS=-j`nproc`
elif [ "$UNAMES" == "Darwin" ] && which sysctl >/dev/null
then
MAKEFLAGS=-j`sysctl -n machdep.cpu.thread_count`
fi

# Build buildsystem tools
cd $DAEMON_DIR/extras/tools
export PATH=`pwd`/build/bin:$PATH
echo "Building tools"
./bootstrap
make $MAKEFLAGS
make .pkg-config
make .gas

############
# Contribs #
############
echo "Building the contribs"
CONTRIB_DIR=${DAEMON_DIR}/contrib/native-${TARGET_TUPLE}
CONTRIB_SYSROOT=${DAEMON_DIR}/contrib/${TARGET_TUPLE}
mkdir -p ${CONTRIB_DIR}
mkdir -p ${CONTRIB_SYSROOT}/lib/pkgconfig

cd ${CONTRIB_DIR}
../bootstrap --host=${TARGET_TUPLE} --enable-ffmpeg --disable-argon2 --disable-asio --disable-fmt --disable-gcrypt --disable-gmp --disable-gnutls --disable-gpg-error --disable-gsm --disable-http_parser --disable-jack --disable-jsoncpp --disable-libarchive --disable-libressl --disable-msgpack --disable-natpmp --disable-nettle --enable-opencv --disable-opendht --disable-pjproject --disable-portaudio --disable-restinio --disable-secp256k1 --disable-speexdsp --disable-upnp --disable-uuid --disable-yaml-cpp

make list
make fetch
export PATH="$PATH:$CONTRIB_SYSROOT/bin"
make $MAKEFLAGS

