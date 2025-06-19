#!/bin/bash
ENERGY=500
NEVENTS=100
SEED=454533333
CONFIGKEYVALUES="GeneratorExternal.fileName=$(pwd)/gun.C;GeneratorExternal.funcName=gun($ENERGY)"
cmd=$(printf "o2-sim-serial -m FOC --detectorList ALICE2.1 -n %d -g external --seed %d --configKeyValues
\"%s\" &> o2sim.log" $NEVENTS $SEED $CONFIGKEYVALUES)
eval $cmd