#include <stdlib.h>
#include <stdio.h>

// Mudança 1: Inclusão da biblioteca OpenMp
#ifdef _OPENMP
#include <omp.h>
#endif

#define tam 4096
#define maxi 1000

// Mudança 2: Alteração do escopo de variaveis e tipo de retorno da função mandelbrot
// Agora recebe também o número de threads com a qual a área paralelizada irá trabalhar
// Retorna 0 caso ocorra erros de alocação e 1 se for executada com exito

int mandelbrot(int *matriz, int n){

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

    // Mudança 3: Criação da área paralela

    #pragma omp parallel num_threads(n)
    {

        // Mudança 4: Divisão da área paralela entre 3 blocos for

        #pragma omp for
        
        for(int coluna = 0; coluna < tam; coluna++){ 
            coord_real[coluna] = -2.0 + coluna * passo_real; 
        }
    
        #pragma omp for

        for(int linha = 0; linha < tam; linha++){ 
            coord_imag[linha] = -1.5 + linha * passo_imag; 
        }
    
        // Mudança 5: Utilização do dynamic para evitar que threads ficassem paradas e sem trabalhar
        // Obs: Implementar método de teste de eficiência e testar se guided ou dynamic em chunks
        // seria mais eficiente que só dynamic

        #pragma omp for schedule(dynamic)
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
    }
    free(coord_real); 
    free(coord_imag);
    return 1;
}

// Mudança 6: Alteração do escopo da função imagem
// Agora também recebe a quantidade de threads que a área paralelizada irá utilizar

void imagem(int *matriz, int n){
    
    FILE *arq = fopen("mandelbrot.ppm", "wb");

    if(arq == NULL){
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    fprintf(arq, "P6\n");
    fprintf(arq, "%d %d\n", tam, tam);
    fprintf(arq, "255\n");

    // Mudança 7: Criação de um array dinamicamente alocado pra guardar os pixeis da imagem
    // Foi criado para permitir a paralelização da atribuição dos valores dos pixeis

    unsigned char *imagem = malloc((size_t)tam * tam * 3);

    if(imagem == NULL){
        printf("Erro ao alocar imagem\n");
        fclose(arq);
        return;
    }

    // Mudança 8: Criação da área paralelizada para atribuição dos pixeis

    #pragma omp parallel for num_threads(n)
    {
        for(int i = 0; i < tam * tam; i++){
    
            unsigned char pix = (unsigned char)((matriz[i] * 255) / maxi);
    
            imagem[i * 3] = pix;
            imagem[i * 3 + 1] = pix;
            imagem[i * 3 + 2] = pix;
        }
    }

    // Mudança 9: Escrita sequencial dos valores no arquivo fora do loop

    fwrite(imagem, sizeof(unsigned char), (size_t)tam * tam * 3, arq);

    free(imagem);
    fclose(arq);
}

int main(void){


    // Mudança 10: Criação de uma variavel n para guardar os números de threads disponiveis
    // Além disso, é feito uma checagem para ver se a OpenMp está disponivel
    // se não estiver, o número de threads será 1, evitando erros 

    int n;

    #ifdef _OPENMP
        n = omp_get_max_threads();
    #else
        n = 1;
    #endif

    int *matriz = malloc(tam * tam * sizeof(int));

    if(matriz == NULL){ 
        printf("Erro ao alocar memória para a matriz\n"); 
        return 1; 
    }

    // Mudança 11: Checagem de erro na função mandelbrot antes de chamar a função imagem

    if(!mandelbrot(matriz, n)){
        free(matriz);
        return 1;
    }

    imagem(matriz, n);

    free(matriz);

    return 0;
}
