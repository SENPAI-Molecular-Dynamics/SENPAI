#!/bin/bash

#
# 1-ethane.sh
# SENPAI technological demonstration
# 2022-01-11 Chelsea Murgia
#

SIMULATION_NAME="1-ethane techologicial demonstration"
AUTHOR_NAME="Chelsea Murgia"

SUBSTRATE=ethane.mds
SOLVENT=void.mds
MODEL=model.mdm
OUTPUT=render.xyz
DURATION=0.001
ETHANE_NB=1
TARGET_POTENTIAL=0.001
SEED=1312
DENSITY=0.01

echo "[ INFO ] ${SIMULATION_NAME} by ${AUTHOR_NAME}"
echo "[ INFO ] Starting SENPAI"
senpai --substrate ${SUBSTRATE}\
       --solvent ${SOLVENT} \
       --out ${OUTPUT} \
       --model ${MODEL} \
       --time ${DURATION} \
       --copy ${ETHANE_NB} \
       --reduce_potential ${TARGET_POTENTIAL} \
       --srand ${SEED} \
       --density ${DENSITY}

if [ $? -ne 0 ]; then
  echo "[FAILED] SENPAI couldn't execute properly."
  exit 255
else
  echo "[  OK  ] SENPAI executed properly"
  exit 0
fi
