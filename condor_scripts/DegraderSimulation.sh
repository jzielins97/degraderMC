#!/bin/sh
# Gets 5 arguments: first material, thickness, second material, thickness, BField on/off
if [ $# -lt 5 ]
then
    # if there are only 3 arguments: second material, thickness, BField on/off
    if [ $# -lt 3 ]
    then
	echo "Error: Not enough parameters"
	echo "Either provide 3: second material, thickness, BField on/off (first foil is removed)"
	echo "Or provide 5: first material, thickness, second material, thickness, BField on/off"
	exit 0
    fi
    firstMaterial=G4_Galactic
    firstThickness=0
    secondMaterial=$1
    secondThickness=$2
    BFieldFlag=$3
else
    firstMaterial=$1
    firstThickness=$2
    secondMaterial=$3
    secondThickness=$4
    BFieldFlag=$5
fi

echo "First material="$firstMaterial
echo "First thickness="$firstThickness
echo "Second material="$secondMaterial
echo "Second thickness="$secondThickness
echo "BField="$BFieldFlag

GITPATH="/afs/cern.ch/user/j/jzielins/AEgIS/degraderMC" # path to the G4 conde for the analysis
OUTPUTDIR="/eos/user/j/jzielins/G4degraderMC" # path to copy output files in the end
FILENAME=$secondThickness${secondMaterial#G4_} # name used for all files generated from the simulation
if [ $BFieldFlag = "on" ]
then
    OUTPUTDIR=$OUTPUTDIR/withBField
else
    OUTPUTDIR=$OUTPUTDIR/withoutBField
fi

if [ $firstThickness -gt 0 ]
then
    # both degrader are in position, update filename
    FILENAME=$firstThickness${firstMaterial#G4_}+$FILENAME
fi

# move to the output directory and create a temporary directory to be used as the work directory
TEMPDIR=$FILENAME"-tmp"
WORKDIR=$OUTPUTDIR/$TEMPDIR
mkdir $WORKDIR
cd $WORKDIR
pwd

# create config file for the simulation
cat << EOF > $FILENAME.in
# Macro file for the initialization of example B2 
# in interactive session
#
# Set some default verbose
/control/verbose 2
/control/saveHistory
/run/verbose 0
#
# Change the default number of threads (in multi-threaded mode)
/run/numberOfThreads 1
#
# Foil geometry
# G4_NAPHTHALENE
/AEgIS/degrader/setFirstMaterial $firstMaterial
/AEgIS/degrader/setFirstThickness $firstThickness nm

/AEgIS/degrader/setSecondMaterial $secondMaterial
/AEgIS/degrader/setSecondThickness $secondThickness nm

/AEgIS/BField $BFieldFlag
#
# Initialize kernel
/run/initialize
#
/gun/particle anti_proton
/gun/energy 100 keV
/tracking/verbose 0

/run/beamOn 1000000
EOF

if [ ! -f $FILENAME.in ]
then
    echo "Error: config file was not created"
    exit 1
fi

# Set up compilers and environments
. $GITPATH/lxplus-setup.sh

# copy the magnetic field file
cp $GITPATH/src/bfield.csv $WORKDIR

# execute simulation
echo
echo "Start time: $(date)"

$GITPATH/build/degraderSim $FILENAME.in

echo "Stop time: $(date)"
echo

if [ -f "man_output.root" ]
then
    mv man_output.root $OUTPUTDIR/$FILENAME.root
    mv $FILENAME.in $OUTPUTDIR/configs/
else
    echo "Error: output file (man_output.root) doesn't exist"
    exit 1
fi

# rm -rf $WORKDIR
