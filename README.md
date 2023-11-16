In order to generate .a files, its necessary to compile the files as descrived bellow.

arm-xilinx-eabi-gcc -Iinclude -c src\grid_forming.c -o object\grid_forming.o
arm-xilinx-eabi-gcc -Iinclude -c src\grid_following.c -o object\grid_following.o
arm-xilinx-eabi-gcc -Iinclude -c src\pll.c -o object\pll.o
arm-xilinx-eabi-gcc -Iinclude -c src\microgrid.c -o object\microgrid.o
arm-xilinx-eabi-ar rcs lib\libmicrogrid.a object\grid_forming.o object\grid_following.o object\pll.o object\microgrid.o
