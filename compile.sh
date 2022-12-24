#!/bin/bash
#To compile Geant4 simulation on lxplus
#Set up compulers and environment
. /afs/cern.ch/user/j/jzielins/AEgIS/degraderMC/lxplus-setup.sh

#5. Compile simulation
cmake -DGeant4_DIR=/cvmfs/geant4.cern.ch/geant4/11.1/x86_64-centos7-gcc9-optdeb-MT/lib64/Geant4-11.1.0 /afs/cern.ch/user/j/jzielins/AEgIS/degraderMC/

#6. Make
make -j4

exit
