CCOPT="-march=rv64gcv -ftree-vectorize -O2 "
export RISCV=$HOME/Work/riscv/rvv64/bin
export CC="$RISCV/riscv64-unknown-elf-gcc $CCOPT"
export CXX="$RISCV/riscv64-unknown-elf-g++ $CCOPT"
export LD="$RISCV/riscv64-unknown-elf-ld $CCOPT"
mkdir -p build && cd build
#make makefile
cmake .. -DBUILD_BENCHMARKS=OFF -DWITH_OPENMP=OFF -DBUILD_UNITTESTS=OFF -DBUILD_STATIC=ON -DBUILD_SHARED=OFF -DCMAKE_INSTALL_PREFIX=.
#make
#usr