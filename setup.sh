# DOWNLOAD AND SET ALL THE GIT REPOSITORIES WITH THE SOURCE CODE OF ENGINES
git submodule update --init --recursive
git submodule update --recursive
# Set up Cadmium repository
cd simulators/cadmium
git checkout b6636f791d3fbff41b6b72e1d9e34ce18152065d
# Set up aDEVS repository
cd ../adevs
git checkout ae4ed330151416f20366d04074e3b1e38e08beec
# Set up xDEVS Java repository
cd ../xdevs-java
git checkout a180827f9bdfc3f44d9f1cc207294d8552e2882d
# Set up xDEVS Python repository
cd ../xdevs-python
git checkout e578d581452394556bb0a85f03f5427fd5e72857
# Set up xDEVS C++ repository
cd ../xdevs-c++
git checkout bdb8c72dc25d7793665e7146b9d17f70b5a12db0
# Set up PythonPDEVS repository
cd ../pythonpdevs
git checkout 190a70532a96a76445b007c4e0f9cb5a4ce03e55
cd ../..

# COMPILE THE DEVSTONE PROJECT FOR ALL THE DIFFERENT ENGINES
cd devstone
# Set up aDEVS project
cd adevs
mkdir build
cd build
cmake ..
make
# Set up Cadmium project
cd ../../cadmium
mkdir build
cd build
cmake ..
make
# Compile xDEVS java
cd ../../../simulators/xdevs-java
find . -name "*.java" | grep src/* > sources.txt
javac @sources.txt -encoding ISO-8859-1 -d out
# build xDEVS Python
cd ../xdevs-python
python3 setup.py install
# build PythonPDEVS
cd ../pythonpdevs/src
python3 setup.py install --user
# Set up xDEVS C++ project
cd ../../xdevs-c++/src/xdevs/examples/DevStone/
cp ../../../../../../devstone/xdevs-c++/Makefile Makefile
make
