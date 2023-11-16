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
R1  = 0.5;
L1  = 1e-3;
Rc  = 0.5;
C1  = 44e-6;
%nominal
Rnom = 3;
Lnom = 3.33e-3;
%aberto
Raberto = 1000;
Laberto = 1;
%curto
Rcurto = 0.0642;
Lcurto = 22e-6;

%definicao das matrizes
Anom = [-(R1+Rc)/L1 0 -1/L1 ; 0 -(Rnom+Rc)/Lnom 1/Lnom ; 1/C1 -1/C1 0];
Aaberto = [-(R1+Rc)/L1 0 -1/L1 ; 0 -(Raberto+Rc)/Laberto 1/Laberto ; 1/C1 -1/C1 0];
Acurto = [-(R1+Rc)/L1 0 -1/L1 ; 0 -(Rcurto+Rc)/Lcurto 1/Lcurto ; 1/C1 -1/C1 0];
B = [1/L1; 0; 0];
C = [1 0 0];
D = 0;

%extracao das funcoes de transferencia
[num, den] = ss2tf(Anom,B,C,D);
planta_nominal = minreal(tf(num, den));
planta_nominal_d = c2d(planta_nominal, t_control, 'zoh');

[num, den] = ss2tf(Aaberto,B,C,D);
planta_aberto = minreal(tf(num, den));
planta_aberto_d = c2d(planta_aberto, t_control, 'zoh');

[num, den] = ss2tf(Acurto,B,C,D);
planta_curto = minreal(tf(num, den));
planta_curto_d = c2d(planta_curto, t_control, 'zoh');

%% malha de corrente
%controlador
ki = 20;
%disturbio do pwm
gpwm = 1 / (1 + 1.5*t_control*s);
%planta auxiliar
corrente_aux = gpwm * planta_nominal;
%malha aberta compensada
corrente_nom_ma = ki * gpwm * planta_nominal;
corrente_aberto_ma = ki * gpwm * planta_aberto;
corrente_curto_ma = ki * gpwm * planta_curto;
%malha fechada
corrente_nom_mf = feedback(corrente_nom_ma, 1);
corrente_aberto_mf = feedback(corrente_aberto_ma, 1);
corrente_curto_mf = feedback(corrente_curto_ma, 1);

%figura
figure(3)
bode_corrente = bodeplot(corrente_nom_ma, corrente_aberto_ma, corrente_curto_ma);
title('Diagrama de Bode');
setoptions(bode_corrente, 'FreqScale', 'log', 'FreqUnits', 'Hz', 'Grid', 'on');
legend('ma_{nominal}', 'ma_{vazio}', 'ma_{curto}');
% Altere os rótulos dos eixos para português
ax = findobj(gcf, 'Type', 'axes');
axes(ax(1));  % Seleciona o primeiro eixo
xlabel('Frequência');
ylabel('Fase');

%% malha de tensao
%controlador
kp = 0.075;
csi_z = 0.9;
csi_p = 0.0001;
g_num = s^2 + 2*csi_z*omega0*s + omega0^2;
g_den = s^2 + 2*csi_p*omega0*s + omega0^2;
ctrl2stool = 1 + (g_num/g_den);
controlador = kp*(g_num/g_den);
%funcoes de transferencia da planta
vc_nom = (Rnom + Lnom * s) / (Lnom * C1 * s^2 + Rnom * C1 * s + 1);
vc_aberto = (Raberto + Laberto * s) / (Laberto * C1 * s^2 + Raberto * C1 * s + 1);
vc_curto = (Rcurto + Lcurto * s) / (Lcurto * C1 * s^2 + Rcurto * C1 * s + 1);
%planta auxiliar
tensao_aux = vc_nom * corrente_nom_mf;
%malha aberta
tensao_nom_ma = controlador * vc_nom * corrente_nom_mf;
tensao_aberto_ma = controlador * vc_aberto * corrente_aberto_mf;
tensao_curto_ma = controlador * vc_curto * corrente_curto_mf;
%malha fechada
tensao_nom_mf = feedback(tensao_nom_ma, 1);
tensao_aberto_mf = feedback(tensao_aberto_ma, 1);
tensao_curto_mf = feedback(tensao_curto_ma, 1);
%malha fechada discretizada
tensao_nom_mfd = c2d(minreal(tensao_nom_mf), t_control);
tensao_aberto_mfd = c2d(minreal(tensao_aberto_mf), t_control);
tensao_curto_mfd = c2d(minreal(tensao_curto_mf), t_control);

%figura
figure(4)
%bode_tensao = bodeplot(tensao_nom_ma, tensao_aberto_ma, tensao_curto_ma);
bode_tensao = bodeplot(tensao_nom_mf, tensao_aberto_mf, tensao_curto_mf);
title('Diagrama de Bode');
setoptions(bode_tensao, 'FreqScale', 'log', 'FreqUnits', 'Hz', 'Grid', 'on');
legend('ma_{nominal}', 'ma_{vazio}', 'ma_{curto}');
% Altere os rótulos dos eixos para português
ax = findobj(gcf, 'Type', 'axes');
axes(ax(1));  % Seleciona o primeiro eixo
xlabel('Frequência');
ylabel('Fase');

%mapa de polos e zeros dos sistemas em malha fechada
figure(5)
pzmap(tensao_nom_mfd, tensao_aberto_mfd, tensao_curto_mfd);
xlim([-1.1 1.1]);
ylim([-1.1 1.1]);
xlabel('Eixo imaginário');
ylabel('Eixo real');
title('Lugar das raízes');
legend('mf_{nominal}', 'mf_{vazio}', 'mf_{curto}');
grid on
