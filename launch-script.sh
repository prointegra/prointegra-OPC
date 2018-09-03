#!/bin/bash
SCRIPT=$(readlink -f $0)
SCRIPT_PATH=`dirname $SCRIPT`
export LD_LIBRARY_PATH=$SCRIPT_PATH/lib/gStools/:$LD_LIBRARY_PATH

#
#chmod +x ./prointegra-OPC
#gdb ./prointegra-OPC
./prointegra-OPC
