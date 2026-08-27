#include <stdlib.h>
#include <stdio.h>
#define tam 4096
#define maxi 1000

void mandelbrot(int *matriz){
    for(int i = 0; i < tam*tam; i++){
        int linha = i / tam;
        int coluna = i % tam;

        double real = -2.0 + ((double)coluna / (tam - 1)) * (1.0 - (-2.0));
        double imag = -1.5 + ((double)linha / (tam - 1)) * (1.5 - (-1.5));
        double zr_antigo = 0.0;
        double zi_antigo = 0.0;
        double zr_novo, zi_novo;
        int j = 0;

        for( ; j < maxi; j++){
            zr_novo = zr_antigo * zr_antigo - zi_antigo * zi_antigo + real;
            zi_novo = 2.0 * zr_antigo * zi_antigo + imag;

            if(zr_novo * zr_novo + zi_novo * zi_novo > 4.0) break;

            zr_antigo = zr_novo;
            zi_antigo = zi_novo;
        }

        matriz[i] = j;
    }
}

void imagem(int *matriz){
    FILE *arq = fopen ("mandelbrot.ppm", "w");
    if(arq == NULL){
        printf("Erro ao abrir o arquivo\n");
        return;
    }
    fprintf(arq, "P3\n");
    fprintf(arq, "%d %d\n", tam, tam);
    fprintf(arq, "255\n");

    for (int i = 0; i < tam * tam; i++){
        int pix = (matriz[i] * 255)/maxi;
        fprintf(arq, "%d %d %d\n", pix, pix, pix);
    }

    fclose(arq);
}

int main(void){
    static int matriz[tam*tam];
    mandelbrot(matriz);
    imagem(matriz);
    return 0;
}
