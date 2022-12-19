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
cd ../xdevs.cpp
git checkout 4a5b9cf7b6498e88e760dfba177a761240999955
# TODO xDEVS C#
# TODO xDEVS Go
# Set up xDEVS Java repository
cd ../xdevs.java
git checkout 9482cf31a873b63f529aa328e913ade9e8edad55
# Set up xDEVS Python repository
cd ../xdevs.py
git checkout 143541d23c48c21f8b2380bdd966384363d306b5
# Set up xDEVS Rust repository
cd ../xdevs.rs
git checkout 9105a2ebbfb176c7333e3765ac2c79d52a2cc03b
# Go back to root directory
cd ../..

# COMPILE THE DEVSTONE PROJECT FOR ALL THE DIFFERENT ENGINES
# Compile DEVStone for aDEVS
cd devstone/adevs
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release
cmake --build build/ --target devstone
# Compile DEVStone for Cadmium v1
cd ../cadmium
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release
cmake --build build/ --target devstone
# Compile DEVStone for Cadmium v2
cd ../../simulators/cadmium_v2
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release
cmake --build build/ --target main_devstone parallel_main_devstone
# Install PythonPDEVS
cd ../pythonpdevs/src
python3 setup.py install --user
# Compile DEVStone for xDEVS C
cd ../../xdevs.c/examples/devstone/
make
# Compile DEVStone for xDEVS C++
cd ../../../xdevs.cpp/src/xdevs/examples/DevStone/
make
# TODO xDEVS C#
# TODO xDEVS Go
# Compile xDEVS java
cd ../../../../../xdevs.java
mvn clean
mvn compile
mvn package
# Install xDEVS Python
cd ../xdevs.py
python3 setup.py install
# Compile DEVStone for xDEVS Rust
cd ../xdevs.rs
cargo build --release
