# DOWNLOAD AND SET ALL THE GIT REPOSITORIES WITH THE SOURCE CODE OF ENGINES
git submodule update --init --recursive
git submodule update --recursive
# Set up Cadmium v1 repository
cd simulators/cadmium
git checkout b6636f791d3fbff41b6b72e1d9e34ce18152065d
# Set up Cadmium v2 repository
cd ../cadmium_v2
git checkout devel # TODO set to a release tag when available
# Set up CDBoost repository
cd ../cdboost
git checkout 6c5f4a457745a5da4f9bf2ddc5a5d46ff4f78fa2
# Set up xDEVS Java repository
# cd ../xdevs-java
# git checkout a180827f9bdfc3f44d9f1cc207294d8552e2882d
# Set up xDEVS Python repository
# cd ../xdevs-python
# git checkout e578d581452394556bb0a85f03f5427fd5e72857
# Set up xDEVS C++ repository
# cd ../xdevs-c++
# git checkout bdb8c72dc25d7793665e7146b9d17f70b5a12db0
# Set up PythonPDEVS repository
cd ../pythonpdevs
git checkout 190a70532a96a76445b007c4e0f9cb5a4ce03e55
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
# Compile DEVStone for CDBoost
cd ../cdboost
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release
cmake --build build/ --target devstone
# Compile DEVStone for Cadmium v2
cd ../../simulators/cadmium_v2
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release
cmake --build build/ --target main_devstone parallel_main_devstone

# Compile xDEVS java
cd ../../simulators/xdevs-java
find -name "*.java" | grep src/* > sources.txt
javac @sources.txt -encoding ISO-8859-1 -d out
# build xDEVS Python
cd ../xdevs-python
python3 setup.py install
# build PythonPDEVS
cd ../pythonpdevs/src
python3 setup.py install --user
# Set up xDEVS C++ project
# cd ../../xdevs-c++/src/xdevs/examples/DevStone/
# make
