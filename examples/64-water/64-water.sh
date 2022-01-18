#!/bin/bash

#
# 64-water.sh
# SENPAI technological demonstration
# 2022-01-07 Chelsea Murgia
#

SIMULATION_NAME="16-water techologicial demonstration"
AUTHOR_NAME="Chelsea Murgia"

SUBSTRATE=water.mds
SOLVENT=void.mds
MODEL=model.mdm
OUTPUT=render.xyz
DURATION=0.001
WATER_NB=64
TARGET_POTENTIAL=0.1
RANDOM_SEED=1337

echo "[ INFO ] ${SIMULATION_NAME} by ${AUTHOR_NAME}"
echo "[ INFO ] Starting SENPAI"
senpai --substrate ${SUBSTRATE}\
       --solvent ${SOLVENT} \
       --out ${OUTPUT} \
       --model ${MODEL} \
       --time ${DURATION} \
       --copy ${WATER_NB} \
       --reduce_potential ${TARGET_POTENTIAL} \
       --srand ${RANDOM_SEED}

if [ $? -ne 0 ]; then
  echo "[FAILED] SENPAI couldn't execute properly."
  exit 255
else
  echo "[  OK  ] SENPAI executed properly"
  exit 0
fi
