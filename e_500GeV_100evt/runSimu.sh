#!/bin/bash
# This script runs an O2 simulation using a custom external generator defined in gun.C
# Monalisa Melo (maria.paulino@usp.br)

# Define the beam energy (in GeV/c)
ENERGY=500
# Define the number of events to simulate
NEVENTS=100
# Define the random seed for reproducibility
SEED=454533333

# Set config key values for gun.C and calls the function gun(ENERGY)
CONFIGKEYVALUES="GeneratorExternal.fileName=$(pwd)/gun.C;GeneratorExternal.funcName=gun($ENERGY)"

# Construct the simulation command with the parameters above
# -m FOC: select the transport engine (e.g. Geant4)
# --detectorList ALICE2.1: use the ALICE Run 4 configuration
# -n: number of events
# -g external: use external generator (gun.C)
# --seed: random seed
# --configKeyValues: pass the generator setup
# Output is redirected to o2sim.log
cmd=$(printf "o2-sim-serial -m FOC --detectorList ALICE2.1 -n %d -g external --seed %d --configKeyValues \"%s\" &> o2sim.log" $NEVENTS $SEED $CONFIGKEYVALUES)

# Run the command using eval (to expand variables and execute the full line)
eval $cmd
