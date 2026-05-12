# [CLEAN] prefix adder
This can be used for members of RAW - Radio at Warwick University - to add the [CLEAN] prefix to all files names and title properties of mp3 files in the current directory.


# Usage
Put the executable in the folder with all the .mp3 files and run it! - this should correctly format them all, simple!


# Compile using
Linux: 

    g++ -std=c++17 -o clean_mp3 clean_mp3.cpp $(pkg-config --cflags --libs taglib)


Cross-compule Linux to Windows:
You need to install the MingGW-w64 cross compuler:

    sudo apt install mingw-w64 cmake

Then build TagLib:

    git clone https://github.com/taglib/taglib.git && cd taglib
    mkdir build-win && cd build-win
    cmake .. \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
        -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
        -DCMAKE_INSTALL_PREFIX=$HOME/taglib-win \
        -DBUILD_SHARED_LIBS=OFF \
        -DFETCH_MISSING_DEPS=ON \
        -DCMAKE_EXE_LINKER_FLAGS="-static"
    make -j$(nproc) && make install

The move the clean_mp3.ccp file into the build-win directory and run

    x86_64-w64-mingw32-g++ -std=c++17 -o clean_mp3.exe clean_mp3.cpp \
        DTAGLIB_STATIC \
        -I$HOME/taglib-win/include \
        -L$HOME/taglib-win/lib \
        -L/usr/x86_64-w64-mingw32/lib \
        -static -ltag -lz -lws2_32 -lole32


Which should cross-compile it


