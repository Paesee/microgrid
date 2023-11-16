This repository contains the digital implementation of a Microgrid (MR) simulation using the Typhoon HIL 402 real-time simulator and the Typhoon HIL Control Center software. The physical platform for simulation is Typhoon HIL 402, and the software tool used is the Typhoon HIL Control Center provided by the same manufacturer. The electrical circuit of the MR within the Schematic Editor using software blocks can be seen in the Figure \ref{fig: HIL_microgrid}.

Microgrid Schematic

Figure 1: Microgrid schematic within the HIL Control Center. Source: Author.

The simulation step, determined based on the processing capacity of HIL and the computational load demanded by the circuit and control algorithms, was set to 0.5 $\mu s$. For system interaction, the HIL SCADA was employed, providing a user-friendly graphical interface for real-time changes to the simulated plant through programmable Python actions. The oscilloscope available in HIL SCADA was used to extract the results presented in the next section.

Concerning the control algorithms, they were developed in the C language using concepts resembling Object-Oriented Programming (OOP). OOP, a native feature of higher-level languages, allows code reuse and error reduction \cite{OOP_IBM}. Despite the chosen language lacking native OOP features, its emulation is achieved through the use of data structs and pointers, enabling references to the structures themselves. The model definition for each controller was done through header files (.h), and the function implementation through source files (.c).

Once the libraries were written, they are pre-compiled into static libraries (.a). This type of library usage allows the incorporation of all defined functions by calling the .a file during the executable compilation process, as mentioned in the software documentation \cite{advancedcfunction} for utilizing C code within the HIL platform.

This approach is advantageous as it adds a layer of abstraction, easing the readability and execution of control routines. As shown in Table 5, generating the voltage control reference signal, obtained through integrating the angular velocity $w_{ref}$, calculating the sine function, multiplying by the magnitude $v_{ref}$, executing the controller's difference equation, and exporting the result to HIL are condensed into three lines of code.

Table 1: Implementation of Control Algorithms using Libraries

scss
Copy code
| Line | Code |
|------|------|
| 1    | generateReference(&vc, v_ref, w_ref, &v_signal, &theta); |
| 2    | executeVoltageControl(&vc, v_signal, in_vc, &u);       |
| 3    | out_u = u;                                              |

Finally, it is worth noting that the difference equations of the controllers were derived from implementing systems in continuous-time in Matlab. The transformation of these transfer functions was achieved using the continue2discrete (c2d) function with the Tustin discretization method. Once in the Z-domain equation, the inverse Z-transform is applied, resulting in a difference equation implementable in real-time digital systems.

In order to generate .a files, its necessary to compile the files as descrived bellow.

arm-xilinx-eabi-gcc -Iinclude -c src\grid_forming.c -o object\grid_forming.o
arm-xilinx-eabi-gcc -Iinclude -c src\grid_following.c -o object\grid_following.o
arm-xilinx-eabi-gcc -Iinclude -c src\pll.c -o object\pll.o
arm-xilinx-eabi-gcc -Iinclude -c src\microgrid.c -o object\microgrid.o
arm-xilinx-eabi-ar rcs lib\libmicrogrid.a object\grid_forming.o object\grid_following.o object\pll.o object\microgrid.o
