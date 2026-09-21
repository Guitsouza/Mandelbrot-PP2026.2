#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define tam 4096
#define maxi 1000

typedef struct {
    unsigned char r, g, b;
} Cor;

int mandelbrot(int *matriz){

    const double passo_real = (1.0 - (-2.0)) / (tam - 1);
    const double passo_imag = (1.5 - (-1.5)) / (tam - 1);
    
    double *coord_real = malloc(tam * sizeof(double));
    double *coord_imag = malloc(tam * sizeof(double));

    if(coord_real == NULL || coord_imag == NULL){ 
        printf("Erro ao alocar memória para as coordenadas\n"); 
        free(coord_real); 
        free(coord_imag); 
        return 0; 
    }

    for(int coluna = 0; coluna < tam; coluna++){ 
        coord_real[coluna] = -2.0 + coluna * passo_real; 
    }

    for(int linha = 0; linha < tam; linha++){ 
        coord_imag[linha] = -1.5 + linha * passo_imag; 
    }

    double inicio = 0.0;
    double fim = 0.0;

    for(int i = 0; i < tam * tam; i++){
        int linha = i / tam;
        int coluna = i % tam;

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
    return 1;
}

void mapa_cores(double valor, unsigned char *r, unsigned char *g, unsigned char *b) {
    const Cor cores[] = {
        {11,  29,  58},
        {18,  56, 110},
        {31, 111, 180},
        {34, 182, 200},
        {240, 180,  41},
        {255, 243, 209}
    };

    if (valor < 0.0) valor = 0.0;
    if (valor > 1.0) valor = 1.0;

    double pos = valor * 5.0;
    int i = (int)pos;

    if (i > 4) i = 4;

    double t = pos - i;

    *r = cores[i].r + t * (cores[i + 1].r - cores[i].r);
    *g = cores[i].g + t * (cores[i + 1].g - cores[i].g);
    *b = cores[i].b + t * (cores[i + 1].b - cores[i].b);
}

void imagem(int *matriz){
    
    FILE *arq = fopen("mandelbrot.ppm", "wb");

    if(arq == NULL){
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    fprintf(arq, "P6\n");
    fprintf(arq, "%d %d\n", tam, tam);
    fprintf(arq, "255\n");

    unsigned char *imagem = malloc((size_t)tam * tam * 3);

    if(imagem == NULL){
        printf("Erro ao alocar imagem\n");
        fclose(arq);
        return;
    }

    unsigned char cores[maxi][3];

    double log_maxi = log1p((double)maxi);

    for (int i = 0; i < maxi; i++) {
        double valor = log1p((double)i) / log_maxi;

        mapa_cores(
            valor,
            &cores[i][0],
            &cores[i][1],
            &cores[i][2]
        );
    }

    for(int i = 0; i < tam * tam; i++){
        int iter = matriz[i];

        if (iter >= maxi) {
            imagem[i * 3]     = 11;
            imagem[i * 3 + 1] = 29;
            imagem[i * 3 + 2] = 58;
        } else {
            imagem[i * 3]     = cores[iter][0];
            imagem[i * 3 + 1] = cores[iter][1];
            imagem[i * 3 + 2] = cores[iter][2];
        }
    }

    fwrite(imagem, sizeof(unsigned char), (size_t)tam * tam * 3, arq);

    free(imagem);
    fclose(arq);
}

int main(void){

    int *matriz = malloc(tam * tam * sizeof(int));

    if(matriz == NULL){ 
        printf("Erro ao alocar memória para a matriz\n"); 
        return 1; 
    }

    if(!mandelbrot(matriz)){
        free(matriz);
        return 1;
    }

    imagem(matriz);

    free(matriz);

    return 0;
}
