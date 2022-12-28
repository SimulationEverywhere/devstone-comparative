# DOWNLOAD AND SET ALL THE GIT REPOSITORIES WITH THE SOURCE CODE OF ENGINES
git submodule update --init --recursive
git submodule update --recursive
# Set up Cadmium v1 repository
cd simulators/cadmium
git checkout b6636f791d3fbff41b6b72e1d9e34ce18152065d
# Set up Cadmium v2 repository
cd ../cadmium_v2
git checkout devel # TODO set to a specific commit when available
# Set up PythonPDEVS repository
cd ../pythonpdevs
git checkout 50164a92c6
# Set up xDEVS C repository
cd ../xdevs.c
git checkout 9c2a54ddd671a790528f6ba4d5a71c2732a431dd
# Set up xDEVS C++ repository
cd ../xdevs.cpp || exit
git checkout 4a5b9cf7b6498e88e760dfba177a761240999955
# TODO xDEVS C#
# TODO xDEVS Go
# Set up xDEVS Java repository
cd ../xdevs.java || exit
git checkout 94b7d15ab3e79220e8145f9576e69948db4b4fb6
# Set up xDEVS Python repository
cd ../xdevs.py || exit
git checkout b53a6f170350af8296af43d1b2334e173e95990c
# Set up xDEVS Rust repository
cd ../xdevs.rs || exit
git checkout 7d4179e61e44f53bb67e2a32abdb3a1ca41752af
# Go back to root directory
cd ../..

# COMPILE THE DEVSTONE PROJECT FOR ALL THE DIFFERENT ENGINES
# Compile DEVStone for aDEVS
cd devstone/adevs || exit
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release
cmake --build build/ --target devstone
# Compile DEVStone for Cadmium v1
cd ../cadmium || exit
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release
cmake --build build/ --target devstone
# Compile DEVStone for Cadmium v2
cd ../../simulators/cadmium_v2 || exit
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release
cmake --build build/ --target main_devstone parallel_main_devstone
# Install PythonPDEVS
cd ../pythonpdevs/src || exit
python3 setup.py install --user  # TODO this fails
# Compile DEVStone for xDEVS C
cd ../../xdevs.c/examples/devstone/ || exit
make
# Compile DEVStone for xDEVS C++
cd ../../../xdevs.cpp/src/xdevs/examples/DevStone/ || exit
make
# TODO xDEVS C#
# TODO xDEVS Go
# Compile xDEVS java
cd ../../../../../xdevs.java || exit
mvn clean
mvn compile
mvn package
# Install xDEVS Python
cd ../xdevs.py || exit
python3 setup.py install
# Compile DEVStone for xDEVS Rust
cd ../xdevs.rs || exit
cargo build --release
