#include <stdio.h>
#include <math.h>
#include <complex.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef I
#define I _Complex_I
#endif

// Função para calcular os coeficientes do filtro passa-baixa de primeira ordem
void calculateFilterCoefficients(double cutoff_frequency, double fs, double *a, double *b) {
    double w0 = 2.0 * M_PI * cutoff_frequency;
    double alpha = w0 / (2.0 * fs);

    // Coeficientes do filtro (a e b para a implementação do filtro IIR)
    *a = alpha / (1.0 + alpha);
    *b = 1.0 / (1.0 + alpha);
}

// Função de transferência do filtro passa-baixa de primeira ordem no domínio do tempo discreto
double filterPassaBaixa(double input, double *z1, double a, double b) {
    double output = b * input + *z1;
    *z1 = -a * output + input;
    return output;
}

int main() {
    double fs = 10000.0; // Frequência de amostragem em Hz
    double frequencia_corte = 5.0; // Frequência de corte desejada em Hz
    double w0 = 2.0 * M_PI * frequencia_corte; // Frequência angular de corte

    double a, b;
    calculateFilterCoefficients(frequencia_corte, fs, &a, &b);

    double z1 = 0.0; // Estado anterior do filtro

    // Simulação do filtro com sinais de entrada
    int num_samples = 500;
    double input_signal[num_samples];
    double output_signal[num_samples];
    double impulse_response[num_samples];

    // Geração do sinal de entrada (120 Hz)
    double frequency_input = 120.0;
    for (int i = 0; i < num_samples; i++) {
        input_signal[i] = sin(2.0 * M_PI * frequency_input * i / fs);
        output_signal[i] = filterPassaBaixa(input_signal[i], &z1, a, b);

        // Calculando a resposta ao impulso do filtro passa-baixa
        double t = i / fs; // Tempo em segundos
        impulse_response[i] = w0 * exp(-w0 * t) * (t >= 0);
    }

    // Plotando gráficos usando o Gnuplot
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
    if (gnuplotPipe) {
        // Gráfico do sinal de entrada e saída
        fprintf(gnuplotPipe, "set xlabel 'Tempo (s)'\n");
        fprintf(gnuplotPipe, "set ylabel 'Amplitude'\n");
        fprintf(gnuplotPipe, "plot '-' with lines title 'Sinal de Entrada', '-' with lines title 'Sinal de Saída'\n");

        for (int i = 0; i < num_samples; i++) {
            fprintf(gnuplotPipe, "%lf %lf\n", i / fs, input_signal[i]);
        }
        fprintf(gnuplotPipe, "e\n");

        for (int i = 0; i < num_samples; i++) {
            fprintf(gnuplotPipe, "%lf %lf\n", i / fs, output_signal[i]);
        }
        fprintf(gnuplotPipe, "e\n");

        fflush(gnuplotPipe);
        fprintf(gnuplotPipe, "exit\n");
        pclose(gnuplotPipe);
    } else {
        printf("Gnuplot não encontrado. Não é possível plotar os gráficos.\n");
    }
    while(1){};
    return 0;
}
