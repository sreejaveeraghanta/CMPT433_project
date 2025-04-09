#!/bin/bash

# Exit on error:
set -e

if [[ "$VIRTUAL_ENV" == "" ]]
then
    echo "ERROR: Must be in correct virtual environment to build Zephyr project."
    echo "   Suggest running:"
    echo "   source ~/zephyrproject/.venv/bin/activate"
    echo ""
    echo "   For more info, see: "
    echo "   https://docs.zephyrproject.org/latest/develop/getting_started/index.html"
    exit
fi

cmake -S . -B build -DBOARD=beagley_ai/j722s/mcu_r5f0_0
cd build
make
cd ..

echo "Copying to NFS..."
mkdir -p ~/cmpt433/public/r5/
cp build/zephyr/zephyr.elf ~/cmpt433/public/r5/zephyr_mcu.elf
cp ./load_r5_mcu.sh ~/cmpt433/public/r5/
chmod +x ~/cmpt433/public/r5/load_r5_mcu.sh
echo "--> Done building Zephyr firmware! <--"
echo ""
echo "To install on target:"
echo "   1. SSH to target and mount NFS"
echo "   2. Run /mnt/remote/r5/load_r5_mcu.sh"
