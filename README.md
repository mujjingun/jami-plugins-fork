$ export DAEMON="<ring-project>/daemon"

OPENCV AND OPENCV_CONTRIB BUILD INSTRUCTIONS

OPENCV VERSION 4.1.1
OPENCV_CONTRIB VERSION 4.1.1

For Android:
    $ cd <ring-project>/clent-android/
    change line 158:
        from ../bootstrap --host=${TARGET_TUPLE} --enable-ffmpeg #--disable-opencv --disable-opencv_contrib
        to ../bootstrap --host=${TARGET_TUPLE} --enable-ffmpeg --disable-opencv --disable-opencv_contrib
    $ cd .. && ./make-ring.py --install --distribution=Android

For Linux:
    $ cd ${DAEMON}/contrib/native/
    $ ./../bootstrap --enable-opencv --enable-opencv_contrib
    $ make install


TENSORFLOW BUILD INSTRUCTIONS

TF VERSION 2.1.0

Dependencies:
    1 - python 3
    2 - bazel 0.27.1

$ git clone https://github.com/tensorflow/tensorflow.git
$ cd tensorflow
$ git checkout -b v2.1.0


For Android:
    Dependencies:
        1 - Android NDK 18r

    $ ./configure 
        >> Do you wish to build TensorFlow with XLA JIT support? [Y/n]: n
        >> Do you wish to download a fresh release of clang? (Experimental) [y/N]: y
        >> Would you like to interactively configure ./WORKSPACE for Android builds? [y/N]: y
        >> Please specify the home path of the Android NDK to use. [Default is /home/pfreitas/Android/Sdk/ndk-bundle]: put the right path to ndk 18r

    $ bazel build //tensorflow/lite:libtensorflowlite.so --crosstool_top=//external:android/crosstool --cpu=armeabi-v7a --host_crosstool_top=@bazel_tools//tools/cpp:toolchain --cxxopt="-std=c++11"

    $ bazel build //tensorflow/lite:libtensorflowlite.so --crosstool_top=//external:android/crosstool --cpu=arm64-v8a --host_crosstool_top=@bazel_tools//tools/cpp:toolchain --cxxopt="-std=c++11"    

    $ bazel build //tensorflow/lite:libtensorflowlite.so --crosstool_top=//external:android/crosstool --cpu=x86_64  --host_crosstool_top=@bazel_tools//tools/cpp:toolchain --cxxopt="-std=c++11"

For Linux:

    $ ./configure 

    $ bazel build //tensorflow/lite:libtensorflowlite.so



TENSORFLOW INCLUDES ASSEMBLE INSTRUCTIONS

    Keep in mind that after each of bazel build instructions above listed, there will be a "libtensorflowlite.so" created at:
        "<tensorflow>/bazel-genfiles/tensorflow/lite/"
    or at:
        "<tensorflow>/bazel-out/<cpu>-opt/bin/tensorflow/lite/"
    (cpu may be "armeabi-v7a", "arm64-v8a", "x86_64" or "k8" depending on the build realized)
    The lib in the first folder is overwritten after each build.
    The lib in the second folder is not.

    create folders and copy files to have the following path struture:

    ~home/Libs/
            _tensorflow_distribuiton/
                lib/
                    arm64-v8a/
                        libtensorflowlite.so
                    armeabi-v7a/
                        libtensorflowlite.so
                    x86_64/
                        libtensorflowlite.so
                    x86_64-linux-gnu/
                        libtensorflowlite.so
                    ...
                include/
                    tensorflow/
                        lite/
                            c/
                                buitin_op_data.h
                                c_api_internal.h
                            core/
                                api/
                                    error_reporter.h
                                    op_resolver.h
                                    profiler.h
                                subgraph.h
                            delegates/
                                gpu/
                                    delegate.h
                                nnapi/
                                    nnapi_delegate.h
                            experimental/
                                resource_variable/
                                    resource_variable.h
                            kernels/
                                register.h
                            nnapi/
                                NeuralNetworksShim.h
                                NeuralNetworksTypes.h
                                nnapi_implementation.h
                                nnapi_util.h
                            schema/
                                schema_generated.h
                            tools/
                                evaluation/
                                    utils.h
                            allocation.h
                            builtin_op_data.h
                            context.h
                            external_cpu_backend_context.h
                            interpreter.h
                            memory_planner.h
                            model.h
                            mutable_op_resolver.h
                            optinal_debug_tools.h
                            simple_memory_arena.h
                            stderr_reporter.h
                            string_type.h
                            util.h
                    flatbuffers/
                        base.h
                        code_generators.h
                        flatbuffers.h
                        flatc.h
                        flexbuffers.h
                        grpc.h
                        hash.h
                        idl.h
                        minireflect.h
                        reflection.h
                        reflection_generated.h
                        registry.h
                        stl_emulation.h
                        util.h

