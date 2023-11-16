close all
clear all
clc

%% tempos e frequencias
%do sistema
f_rede = 60;
t_rede = 1/f_rede;
f_control = 10e3;
t_control = 1/f_control;
s = tf('s');
omega0 = 2*pi*f_rede;
%para uso do lsim no dominio do tempo
tv = 0:t_control:20*t_rede;

%% Planta
R1  = 0.05;
L1  = 1e-3;
C1  = 44e-6;
%nominal
Rnom = 0.05;
Lnom = 1e-3;
%definicao das matrizes
A = [-R1/L1 0 -1/L1 ; 0 -Rnom/Lnom 1/Lnom ; 1/C1 -1/C1 0];
B = [1/L1; 0; 0];
C = [1 0 0];
D = 0;

%extracao das funcoes de transferencia
[num, den] = ss2tf(A,B,C,D);
planta = minreal(tf(num, den));
planta_d = c2d(planta, t_control, 'zoh');

%% malha de corrente
%controlador
kp = 0.086;
csi_z = 0.9;
csi_p = 0.001;
g_num = 2*csi_z*omega0*s;
g_den = s^2 + 2*csi_p*omega0*s + omega0^2;
controlador = 1 + (g_num/g_den);
controlador = kp * controlador;
%disturbio do pwm
gpwm = 1 / (1 + 1.5*t_control*s);
%planta auxiliar
corrente_aux = gpwm * planta;
%malha aberta compensada
corrente_ma = controlador * gpwm * planta;
%malha fechada
corrente_mf = feedback(corrente_ma, 1);
%malha fechada discretizada
corrente_mfd = c2d(minreal(corrente_mf), t_control);

%figura
figure(3)
bode_corrente = bodeplot(corrente_ma);
title('Diagrama de Bode');
setoptions(bode_corrente, 'FreqScale', 'log', 'FreqUnits', 'Hz', 'Grid', 'on');
% Altere os rótulos dos eixos para português
ax = findobj(gcf, 'Type', 'axes');
axes(ax(1));  % Seleciona o primeiro eixo
xlabel('Frequência');
ylabel('Fase');

figure(4)
pzmap(corrente_mfd);
xlim([-1.1 1.1]);
ylim([-1.1 1.1]);
xlabel('Eixo imaginário');
ylabel('Eixo real');
title('Lugar das raízes');
grid on

%% malha de potencia
%controlador
b = 0.5 * sqrt(2) * 220;
pm = 100;
w_bw = 2 * pi * 5;
kp = cosd(pm - 180)/b;
aux_num = 1 + 2*b*kp - (b^2 * kp^2);
aux_den = b^2;
ki = w_bw * sqrt(aux_num/aux_den);
controlador = kp + ki/s;
%malha aberta
potencia_ma = controlador * b;
%malha fechada
potencia_mf = feedback(potencia_ma, 1);

%figura
figure(5)
bode_corrente = bodeplot(potencia_ma);
title('Diagrama de Bode');
setoptions(bode_corrente, 'FreqScale', 'log', 'FreqUnits', 'Hz', 'Grid', 'on');
% Altere os rótulos dos eixos para português
ax = findobj(gcf, 'Type', 'axes');
axes(ax(1));  % Seleciona o primeiro eixo
xlabel('Frequência');
ylabel('Fase');

wc = 2*pi*6;
lpf = wc/(s+wc);
c2d(lpf, 1/2000)