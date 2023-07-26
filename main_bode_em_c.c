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
    double fs = 1000.0; // Frequência de amostragem em Hz
    double frequencia_corte = 100.0; // Frequência de corte desejada em Hz

    double a, b;
    calculateFilterCoefficients(frequencia_corte, fs, &a, &b);

    double z1 = 0.0; // Estado anterior do filtro

    // Simulação do filtro com sinais de entrada
    int num_samples = 100;
    double input_signal[num_samples];
    double output_signal[num_samples];

    // Geração de diferentes sinais de entrada (por exemplo, seno, degrau, etc.)
    double frequency1 = 5.0;
    double frequency2 = 50.0;

    for (int i = 0; i < num_samples; i++) {
        input_signal[i] = sin(2.0 * M_PI * frequency1 * i / fs) + 0.5 * sin(2.0 * M_PI * frequency2 * i / fs);
        output_signal[i] = filterPassaBaixa(input_signal[i], &z1, a, b);
    }

    // Salvando dados para a resposta em frequência em um arquivo
    FILE *fp;
    fp = fopen("data.txt", "w");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    double w0 = 2.0 * M_PI * frequencia_corte; // Frequência angular de corte
    for (double frequency = 0.0; frequency <= fs / 2.0; frequency += 1.0) {
        double omega = 2.0 * M_PI * frequency;
        double complex s = I * omega; // Variável complexa s
        double complex H = (w0 / 20.0) / (s + (w0 / 20.0)); // Resposta em frequência H(s)
        fprintf(fp, "%lf %lf\n", frequency, 20.0 * log10(cabs(H))); // Escreve no arquivo em dB
    }

    fclose(fp);

    // Plotando gráfico usando o Gnuplot
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
    if (gnuplotPipe) {
        fprintf(gnuplotPipe, "set xlabel 'Amostras'\n");
        fprintf(gnuplotPipe, "set ylabel 'Amplitude'\n");
        fprintf(gnuplotPipe, "plot '-' with lines title 'Sinal de Entrada', '-' with lines title 'Sinal de Saída'\n");

        for (int i = 0; i < num_samples; i++) {
            fprintf(gnuplotPipe, "%d %lf\n", i, input_signal[i]);
        }
        fprintf(gnuplotPipe, "e\n");

        for (int i = 0; i < num_samples; i++) {
            fprintf(gnuplotPipe, "%d %lf\n", i, output_signal[i]);
        }
        fprintf(gnuplotPipe, "e\n");

        fprintf(gnuplotPipe, "set xlabel 'Frequência (Hz)'\n");
        fprintf(gnuplotPipe, "set ylabel 'Magnitude (dB)'\n");
        fprintf(gnuplotPipe, "plot 'data.txt' with lines title 'Resposta em Frequência'\n");

        fflush(gnuplotPipe);
        fprintf(gnuplotPipe, "exit\n");
        pclose(gnuplotPipe);
    } else {
        printf("Gnuplot não encontrado. Não é possível plotar os gráficos.\n");
    }

    return 0;
}
