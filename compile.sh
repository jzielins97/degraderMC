#!/bin/bash
#To compile Geant4 simulation on lxplus

GITDIR=/afs/cern.ch/user/j/jzielins/AEgIS/degraderMC

#Set up compulers and environment
. $GITDIR/lxplus-setup.sh

# create a new directory for building
if [ ! -d $GITDIR/build ]
then
    mkdir -p build
fi
cd $GITDIR/build

# Compile simulation
cmake -DGeant4_DIR=/cvmfs/geant4.cern.ch/geant4/11.1/x86_64-centos7-gcc9-optdeb-MT/lib64/Geant4-11.1.0 $GITDIR/

# Make
make -j4

echo "Simulation code made in $( pwd )"

exit
