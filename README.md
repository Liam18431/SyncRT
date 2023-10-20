# SyncRT
A simple Geant4 application for calculating synchrotron microbeam radiotherapy doses.

# Requirements

* [Geant4](https://geant4.web.cern.ch/)

# Build instructions (using CMake)

* Set the **Geant4_DIR** CMake variable to point to the directory containing the **Geant4Config.cmake** file.

* To build from the command line, navigate to the source directory and run:
```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=Path/to/install/ -DGeant4_DIR=Path/to/Geant4/lib/cmake/ ../
cmake --build . --config Release --target install
```

# Usage

* To start a UI session, run the SyncRT executable (vis.mac and spectrum.dat must be in the current directory).

* Including the **--histories** command line option will perform a simulation in batch mode:
```
./SyncRT.exe --histories=10000
```

* Command line options can be queried by including the **--help** argument

* Outputs are dose data structures that can be opened using MATLAB or Python. An example Python script is provided.
