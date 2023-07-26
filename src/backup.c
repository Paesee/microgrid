/*
clc
clear all
close all

%definicoes
f = 2e3;
ts = 1/f;
s = tf('s');
z = tf('z', ts);

%funcao de transferencia
w0    = 2*pi*120; 
LPF   = (w0/20)/(s+(w0/20));
LPFd  = c2d(LPF, ts, 'tustin')
LPFd  = LPFd*z^-2
syms z
F = (0.009337*z + 0.009337)/(z);
iztrans(F)
*/
