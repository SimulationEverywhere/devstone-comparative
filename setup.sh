# DOWNLOAD AND SET ALL THE GIT REPOSITORIES WITH THE SOURCE CODE OF ENGINES
git submodule --init --recursive
cd simulators
# Set up CDBoost repository
cd simulators/cdboost
git checkout 90e7074edb3ab6a655104bbc82adc57fcfafa373
# Set up Cadmium repository
cd ../cadmium
git checkout b6636f791d3fbff41b6b72e1d9e34ce18152065d
# Set up aDEVS repository
cd ../adevs
git checkout ae4ed330151416f20366d04074e3b1e38e08beec
# Set up xDEVS Java repository
cd ../xdevs-java
git checkout 9315e71b46e576b98536150ab52bc900f91e53a8
# Set up xDEVS Python repository
cd ../xdevs-python
git checkout 224482d46a222d5d7c71ad5b2f6b4ba7264a809d
# Set up PythonPDEVS repository
cd ../pythonpdevs
git checkout 190a70532a96a76445b007c4e0f9cb5a4ce03e55
cd ..

# COMPILE THE DEVSTONE PROJECT FOR ALL THE DIFFERENT ENGINES
