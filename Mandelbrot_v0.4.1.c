#include <stdlib.h>
#include <stdio.h>

#define tam 4096
#define maxi 1000

void mandelbrot(int *matriz){
    // Mudança 1: pré-cálculo dos passos
    const double passo_real = (1.0 - (-2.0)) / (tam - 1);
    const double passo_imag = (1.5 - (-1.5)) / (tam - 1);
    
    // Mudança 2: pré-calcular todas as linhas invés de recalcular para cada pixel
    // Mudança 3: alocação de memoria dos vetores com todas as coordenadas pré-calculadas
    double *coord_real = malloc(tam * sizeof(double));
    double *coord_imag = malloc(tam * sizeof(double));

    if(coord_real == NULL || coord_imag == NULL){ 
        printf("Erro ao alocar memória para as coordenadas\n"); 
        free(coord_real); 
        free(coord_imag); 
        return; 
    }

    // colunas 
    for(int coluna = 0; coluna < tam; coluna++){ 
        coord_real[coluna] = -2.0 + coluna * passo_real; 
    }

    // linhas 
    for(int linha = 0; linha < tam; linha++){ 
        coord_imag[linha] = -1.5 + linha * passo_imag; 
    }

    for(int i = 0; i < tam * tam; i++){
        int linha = i / tam;
        int coluna = i % tam;

        // Apenas acessa os valores já calculados
        double real = coord_real[coluna]; 
        double imag = coord_imag[linha];

        double zr_antigo = 0.0;
        double zi_antigo = 0.0;
        double zr_novo, zi_novo;
        int j = 0;

        for(; j < maxi; j++){
            zr_novo = zr_antigo * zr_antigo
                    - zi_antigo * zi_antigo
                    + real;

            zi_novo = 2.0 * zr_antigo * zi_antigo
                    + imag;

            if(zr_novo * zr_novo + zi_novo * zi_novo > 4.0)
                break;

            zr_antigo = zr_novo;
            zi_antigo = zi_novo;
        }

        matriz[i] = j;
    }
    free(coord_real); 
    free(coord_imag);
}

void imagem(int *matriz){
    // Mudança 4: escrever arquivo ppm em P6 invés de P3
    FILE *arq = fopen("mandelbrot.ppm", "wb");

    if(arq == NULL){
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    // P3 mudou para P6
    fprintf(arq, "P6\n");
    fprintf(arq, "%d %d\n", tam, tam);
    fprintf(arq, "255\n");

    for(int i = 0; i < tam * tam; i++){
        // valores guardados em byte
        unsigned char pix = (unsigned char)((matriz[i] * 255) / maxi);
        // 1 byte pra R, 1 para G e 1 para B
        unsigned char rgb[3] = {pix, pix, pix};
        // P6 deixa os bytes já no arquivo usando fwrite invés de fprintf
        fwrite(rgb, sizeof(unsigned char), 3, arq);
    }

    fclose(arq);
}

int main(void){
    // Mudança 3: alocação de memória da matriz 
    int *matriz = malloc(tam * tam * sizeof(int));

    if(matriz == NULL){ 
    printf("Erro ao alocar memória para a matriz\n"); 
    return 1; 
    }

    mandelbrot(matriz);
    imagem(matriz);

    free(matriz);

    return 0;
}
