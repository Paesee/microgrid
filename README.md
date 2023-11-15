In order to generate .a files, its necessary to compile the files written bellow.

Commands to compile:

arm-xilinx-eabi-gcc -Iinclude -c src\grid_forming_inverter.c -o src\grid_forming_inverter.o
arm-xilinx-eabi-gcc -Iinclude -c src\grid_following_inverter.c -o src\grid_following_inverter.o
arm-xilinx-eabi-gcc -Iinclude -c src\sogi_qsg.c -o src\sogi_qsg.o
arm-xilinx-eabi-ar rcs microgrid.a src\grid_forming_inverter.o src\grid_following_inverter.o src\sogi_qsg.o

Commands to compile for microgrid controller:

arm-xilinx-eabi-gcc -Iinclude -c src\grid_forming_inverter.c -o src\grid_forming_inverter.o
arm-xilinx-eabi-ar rcs ugrid.a src\grid_forming_inverter.o