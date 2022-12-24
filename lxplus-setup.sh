#!/bin/bash
#To compile Geant4 simulation on lxplus
#1. Set up compiler
source /cvmfs/sft.cern.ch/lcg/releases/gcc/9.2.0/x86_64-centos7/setup.sh

#2. Set up environment for Geant4
source /cvmfs/geant4.cern.ch/geant4/11.1/x86_64-centos7-gcc9-optdeb-MT/CMake-setup.sh

#3. gcc compilator
export CC=/cvmfs/sft.cern.ch/lcg/releases/gcc/9.2.0/x86_64-centos7/bin/gcc
export CXX=/cvmfs/sft.cern.ch/lcg/releases/gcc/9.2.0/x86_64-centos7/bin/g++

#4. CMake
source /cvmfs/sft.cern.ch/lcg/releases/LCG_97/CMake/3.14.3/x86_64-centos7-gcc9-opt/CMake-env.sh
