#!/bin/sh

# 
# Vivado(TM)
# runme.sh: a Vivado-generated Runs Script for UNIX
# Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
# 

if [ -z "$PATH" ]; then
  PATH=/thayerfs/apps/xilinx/Vitis/current/bin:/thayerfs/apps/xilinx/Vivado/current/ids_lite/ISE/bin/lin64:/thayerfs/apps/xilinx/Vivado/current/bin
else
  PATH=/thayerfs/apps/xilinx/Vitis/current/bin:/thayerfs/apps/xilinx/Vivado/current/ids_lite/ISE/bin/lin64:/thayerfs/apps/xilinx/Vivado/current/bin:$PATH
fi
export PATH

if [ -z "$LD_LIBRARY_PATH" ]; then
  LD_LIBRARY_PATH=
else
  LD_LIBRARY_PATH=:$LD_LIBRARY_PATH
fi
export LD_LIBRARY_PATH

HD_PWD='/thayerfs/home/f0056cj/workspace/Module5/Module5.runs/Module5_hw_axi_gpio_0_0_synth_1'
cd "$HD_PWD"

HD_LOG=runme.log
/bin/touch $HD_LOG

ISEStep="./ISEWrap.sh"
EAStep()
{
     $ISEStep $HD_LOG "$@" >> $HD_LOG 2>&1
     if [ $? -ne 0 ]
     then
         exit
     fi
}

EAStep vivado -log Module5_hw_axi_gpio_0_0.vds -m64 -product Vivado -mode batch -messageDb vivado.pb -notrace -source Module5_hw_axi_gpio_0_0.tcl
