
export RISCV=$HOME/Work/riscv/rvv64/bin
export CC=$RISCV/riscv64-unknown-elf-gcc
export CXX=$RISCV/riscv64-unknown-elf-g++
export LD=$RISCV/riscv64-unknown-elf-ld

mkdir -p build && cd build
if [$1 -eq *v*] ;then
    VOPT="-march=rv64gcv -ftree-vectorize -O2 "
else
    VOPT=""
fi

export CMAKE_CXX_FLAGS="-mcmodel=medany -static -nostdlib -nostartfiles -fno-common -fno-builtin-printf -fno-builtin-strcmp ${VOPT}"
export CMAKE_C_FLAGS=$CMAKE_CXX_FLAGS
export CMAKE_EXE_LINKER_FLAGS="${CMAKE_CXX_FLAGS} -lm -lgcc"

## make makefile
cmake .. -DBUILD_BENCHMARKS=OFF -DWITH_OPENMP=OFF -DBUILD_UNITTESTS=OFF -DBUILD_STATIC=ON -DBUILD_SHARED=OFF -DCMAKE_INSTALL_PREFIX=. 
#make -j4
#make install 