
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <time.h>

typedef struct cliente{
    int key;
    char nome[50];
    int coordX;
    int coordY;
}Cliente;

typedef struct hash{
    int quant;
    int tamanho;
    Cliente *hashClientes;
    double **matrizDistancias;
}Hash;

typedef struct grafo{
    double **matriz;
    int digrafo;
    int quantV;
}Grafo;

typedef struct nodijkstra{
    int p;
    double d;
    int aberto;
}NoDijkstra;


//|||||||||||||| CABEÇALHO DE FUNÇÕES |||||||||||||||||||
void limpa_tela();
void limpaBuffer();
void inicializaHashClientes(Hash *h, int numeroClientes);
int funcaoHash(int key, int tamanho);
int hashEstaCheia(Hash *h);
void inserirClienteHash(Hash *h, Cliente *c);
void exibeHashClientes(Hash *h);
void inicializaEPreencheMatrizDistancias(Hash *h);
void destroiMatrizDistancias(Hash *h);
void exibeMatrizDistancias(Hash *h);
void receberHashClientesArquivo(Hash *h);
void inicializaGrafo(Grafo *g, int digrafo, int quantidadeVertices);
void criarArestaGrafo(Grafo *g, int v1, int v2, double peso);
void exibeGrafo(Grafo *g, Hash *h);
void destruirGrafo(Grafo *g);
double distanciaEntrePontos(int coordxV1, int coordyV1, int coordxV2, int coordyV2);
void receberMatrizClientesArquivo(Grafo *g, Hash *h);
void relaxa(Grafo *g, NoDijkstra *n, int u, int v);
void adquireLocalizacoes(Hash *h,int* localizacoesEquipes, int *locChamado, int quantidadeClientes, int quantidadeEquipes);
void calculaTempos(NoDijkstra *noD, Grafo *g ,int noInicial, int numeroAbertos);
int verificaCaminho(NoDijkstra *noD, Grafo *g ,int *caminho, int noInicial, int noFinal);
void criarRelatorio(NoDijkstra *noD ,Hash *h ,int *caminho ,int noInicial ,int noFinal, int numeroClientes, char *tipoArquivo);
void criarRelatorioBinario(NoDijkstra *noD ,Hash *h ,int *caminho ,int noInicial ,int noFinal, int numeroClientes);
void exibirRelatorio(NoDijkstra *noD ,Hash *h ,int *caminho ,int noInicial ,int noFinal, int numeroClientes);
void encontrarMenorCaminhoDijkstra(Grafo *g, Hash *h);
void menu();
///////////////////////////////////////////////////////////

int main()
{
    menu();
    return 0;
}

//////////////////////////////////////////////////////////
/**
 * @brief procedimento utilizado para limpar a tela do terminal
 * 
 */
void limpa_tela(){
    system("clear||cls");
}

/**
 * @brief procedimento para limpar o buffer do teclado da melhor maneira dependendo o systema sendo utilizado, Windows ou Linux
 * 
 */
void limpaBuffer(){
    #ifdef _WIN32 //Testa se o SO é Windows
        fflush(stdin);
    #else
        setbuf(stdin,NULL);
    #endif
}

//////////////////////////////////////////////
/**
 * @brief Procedimento para inicializar a tabela Hash que irá conter os clientes
 * 
 * @param h ponteiro para a tabela hash
 * @param numeroClientes  quantidade de clientes 
 */
void inicializaHashClientes(Hash *h, int numeroClientes){
    h->tamanho = numeroClientes;
    h->hashClientes = (Cliente*) malloc(h->tamanho * sizeof(Cliente));
    for(int i = 0; i < h->tamanho; i++)
        h->hashClientes[i].key = -1;

    h->quant = 0;
    return;
}

/**
 * @brief função hash para a obtenção da chave do cliente da tabela
 *  
 * @param key key predeterminada do cliente
 * @return int key atualizada do cliente
 */
int funcaoHash(int key, int tamanho){
    return key % tamanho;
}

/**
 * @brief Função que verifica se a tabela hash esta cheia
 * 
 * @param h ponteiro para a tabela hash
 * @return int se a tabela estiver cheia retorna 1, caso contrario retorna 0
 */
int hashEstaCheia(Hash *h){
    return h->quant == h->tamanho;
}

/**
 * @brief Procedimento que realiza a inserção de um cliente na tabela hash
 * 
 * @param h ponteiro para a tabela hash
 * @param c ponteiro para o cliente a ser inserido
 */
void inserirClienteHash(Hash *h, Cliente *c){
    if(hashEstaCheia(h) == 1) return;

    //determina a key a ser inserida e se há espaço na tabela no posição indicada, se não tiver espaço verifica o próximo
    int k = funcaoHash(c->key,h->tamanho);
    int deslocamento = 0;
    while(h->hashClientes[k].key != -1){
        deslocamento++;
        k = (k+1) % h->tamanho;
    }

    //inserção dos dados na tabela
    h->hashClientes[k].key = k;
    strcpy(h->hashClientes[k].nome,c->nome);
    h->hashClientes[k].coordX = c->coordX;
    h->hashClientes[k].coordY = c->coordY;

    return;
}

/**
 * @brief Procedimento que exibe a tabela hash formatada
 * 
 * @param h ponteiro para a tabela hash
 */
void exibeHashClientes(Hash *h){
    printf(" Key                           Nome                         X     Y\n");
    for(int i = 0; i < h->tamanho; i++){
       if(h->hashClientes[i].key != -1) printf("%3d | %50s | %3d | %3d \n",h->hashClientes[i].key ,h->hashClientes[i].nome ,h->hashClientes[i].coordX ,h->hashClientes[i].coordY);
    }
    printf("\n");
}

/**
 * @brief inicializa a matriz de distancias e a preenche com todas as distancias entre cada cliente e todos os outros
 * 
 * @param h ponteiro para a tabela hash
 */
void inicializaEPreencheMatrizDistancias(Hash *h){
    h->matrizDistancias = (double**) malloc(h->tamanho * sizeof(double *));
    for(int i = 0; i < h->tamanho; i++){
        h->matrizDistancias[i] = (double*) malloc(h->tamanho * sizeof(double));
    }

    for(int i = 0; i < h->tamanho; i++){
        for(int j = 0; j < h->tamanho; j++){
            h->matrizDistancias[i][j] = distanciaEntrePontos(h->hashClientes[i].coordX ,h->hashClientes[i].coordY ,h->hashClientes[j].coordX ,h->hashClientes[j].coordY);
        }
    }

}

/**
 * @brief desaloca a memoria ocupada pela matriz de distancias
 * 
 * @param h ponteiro para a tabela hash
 */
void destroiMatrizDistancias(Hash *h){
    for(int i = 0; i < h->tamanho; i++){
        free(h->matrizDistancias[i]);
    }
    free(h->matrizDistancias);
}

/**
 * @brief exibe a matriz de distancias 
 * 
 * @param h ponteiro para a tabela hash
 */
void exibeMatrizDistancias(Hash *h){
    for(int i = 0; i < h->tamanho; i++){
        for(int j = 0; j < h->tamanho; j++){
            printf("distancia[%d][%d] -> %.3f \n",i ,j ,h->matrizDistancias[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief Procedimento que realiza a leitura de um arquivo contendo os clientes e as coordenadas de suas casas e salva os dados em uma tabela Hash
 * 
 * @param h ponteiro para a tabela hash
 */
void receberHashClientesArquivo(Hash *h){

    //abertura e verificação do arquivo
    FILE *arq = fopen("coordenadas_clientes.csv","r");

    if(arq == NULL){
        printf("Falha na abertura do arquivo \n");
        return;
    }

    //o primeiro caractere do arquivo representa a quantidade de clientes, assim faz a leitura desse caractere e inicializa a tabela hash
    char numeroClientes[10] ;
    fscanf(arq,"%[^,],",numeroClientes);
    
    inicializaHashClientes(h,atoi(numeroClientes));

    //faz a devida leitura dos dados do arquivo e os insere na tabela hash
    Cliente aux;
    char linha[100];
    int keyInicial = 0;

    while(fgets(linha,100,arq) != NULL){
        fscanf(arq,"%[^,],%d,%d",aux.nome ,&aux.coordX ,&aux.coordY);
        aux.key = keyInicial;
        keyInicial++;
        inserirClienteHash(h,&aux);
    }

    inicializaEPreencheMatrizDistancias(h);
    


    fclose(arq);
    return;
}

////////////////////////////////////////////

/**
 * @brief Procedimento que inicializa o grafo que irá conter as rotas entre as casas e os tempos para cada rota
 * 
 * @param g ponteiro para o grafo
 * @param digrafo especifica se o grafo é orientado ou não
 * @param quantidadeVertices numero de vertices = numero de clientes
 */
void inicializaGrafo(Grafo *g, int digrafo, int quantidadeVertices){
    g->quantV = quantidadeVertices;
    g->digrafo = digrafo;

    g->matriz = (double**) malloc(quantidadeVertices * sizeof(double*));
    for(int i = 0; i < quantidadeVertices; i++){
        g->matriz[i] = (double*) malloc(quantidadeVertices * sizeof(double));
        for(int j = 0; j < quantidadeVertices; j++){
            g->matriz[i][j] = 0;
        }
    }
}

/**
 * @brief procedimento que cria a conexão entre os vertices, especificando o tempo entre as conexões
 * 
 * @param g ponteiro para o grafo
 * @param v1 vertice de saida
 * @param v2 vertice de chegada
 * @param peso representção do valor da aresta
 */
void criarArestaGrafo(Grafo *g, int v1, int v2, double peso){
    g->matriz[v1][v2] = peso;
    if(g->digrafo == 0) g->matriz[v2][v1] = peso;
}

/**
 * @brief Procedimento que exibe o grafo formatado
 * 
 * @param g ponteiro para o grafo
 * @param h ponteiro para a tabela hash
 */
void exibeGrafo(Grafo *g, Hash *h){
    printf("          Clientes\t      Keys ");
    for(int i= 0; i < g->quantV; i++){
        printf("%5d ",h->hashClientes[i].key);
    }
    printf("\n");




    for(int i = 0; i < g->quantV; i++){
        printf("%25s - %5d  ",h->hashClientes[i].nome ,h->hashClientes[i].key);
        for(int j = 0; j < g->quantV; j++){
            printf("%2.1f ",g->matriz[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief Procedimento que libera a memoria alocada pelo grafo
 * 
 * @param g ponteiro para o grafo
 */
void destruirGrafo(Grafo *g){
    for(int i = 0; i < g->quantV; i++){
        free(g->matriz[i]);
    }
    free(g->matriz);
}

/**
 * @brief Função que realiza o cálculo da distância entre dois pontos
 * 
 * @param coordxV1 coordenada x do ponto 1
 * @param coordyV1 coordenada y do ponto 1 
 * @param coordxV2 coordenada x do ponto 2 
 * @param coordyV2 coordenada y do ponto 2 
 * @return int retorna o resultado do calculo
 */
double distanciaEntrePontos(int coordxV1, int coordyV1, int coordxV2, int coordyV2){
    return (sqrt( pow(coordxV2 - coordxV1,2) + pow(coordyV2 - coordyV1,2)  ));
}

/**
 * @brief Procedimento que faz a leitura de um arquivo contendo as ligações entre as casas dos clientes e então salva em uma matriz de adjacencia
 *        o tempo necessario para cada ligação
 * 
 * @param g ponteiro para o grafo
 * @param h ponteiro para a tabela hash
 */
void receberMatrizClientesArquivo(Grafo *g, Hash *h){
    inicializaGrafo(g, 1, h->tamanho);

    //abertura e verificação do arquivo
    FILE *arq = fopen("clientes adjacencia.csv","r");

    if(arq == NULL){
        printf("Falha na abertura do arquivo \n");
        return;
    }

    //faz a devida leitura dos dados do arquivo
    char linha[5000];
    fgets(linha,5000,arq);
    int ligacao = 0;
    double pesoAresta;
    for(int i = 0; i < g->quantV; i++){
        fscanf(arq,"%[^,],",linha);
        for(int j = 0; j < g->quantV; j++){
            if(j != g->quantV-1)fscanf(arq,"%d,",&ligacao);
            else fscanf(arq,"%d",&ligacao);

            //quando há ligação entre dois vertices, faz o calculo da distancia entre eles e cria uma aresta entre eles tendo o resultado
            //da distância como o peso da aresta
            if(ligacao == 1){
                pesoAresta = h->matrizDistancias[i][j];
                criarArestaGrafo(g,i,j,pesoAresta);
            }
        }
        fgets(linha,5000,arq);
    }
    fclose(arq);
    return;
}

/////////////////////////////////////////////

/**
 * @brief Procedimento relaxa os nós no algoritmo de Dijkstra verificando a distancia entre um vértices e seus adjacentes, atualizando 
 *        os valores de distância e ponto anterior caso a distancia seja menor a que está armazenada nos nós adjcentes
 * 
 * @param g ponteiro para o grafo 
 * @param n vetor para o NoDijkstra que armazena as condições dos vértices no algoritmo
 * @param u vertice base para verificar as distancias com seus adjacentes v
 * @param v vertice adjacente ao vertice u
 */
void relaxa(Grafo *g, NoDijkstra *n, int u, int v){
    if((n[u].d + g->matriz[u][v]) < n[v].d){
        n[v].p = u;
        n[v].d = n[u].d + g->matriz[u][v];
    }
}

/**
 * @brief Procedimento adquire as localizações de todas as equipes disponiveis para o atendimento e a localização do local de chamado para atendimento
 * 
 * @param h ponteiro para a tabela hash dos clientes
 * @param localizacoesEquipes vetor para armazenar as localizaçoes das equipes de atendimento
 * @param locChamado guarda o local de chamado de atendimento
 * @param quantidadeClientes quantidade de clientes cadastrados no sistema
 * @param quantidadeEquipes quantidade de equipes disponiveis
 */
void adquireLocalizacoes(Hash *h,int* localizacoesEquipes, int *locChamado, int quantidadeClientes, int quantidadeEquipes){
    int a;

    //adquire as localizações das equipes de atendimento
    for(int i = 0; i < quantidadeEquipes; i++){
        a = 0;
        do{ 
            limpa_tela();
            if(a != 0) printf("Selecao invalida\n");
            printf("Insira a localizacao da equipe de atendimento %d -> ",i+1);
            scanf("%d",&a);
            limpaBuffer();
        }while(a<0 || a >= quantidadeClientes);
        localizacoesEquipes[i] = a;
    }
    
    //adquire a localização do chamado de emergencia
    a = 0;
    do{ 
        limpa_tela();
        if(a != 0) printf("Selecao invalida\n");
        exibeHashClientes(h);
        printf("\nInsira a localizacao do chamado de emergencia -> ");
        scanf("%d",&a);
        limpaBuffer();
    }while(a<0 || a >= quantidadeClientes);
    (*locChamado) = a;
    limpa_tela();
}

/**
 * @brief Procedimento que é o algoritmo de Dijkstra, a partir do vértice inicial calcula-se a menor distância de todos os outros vertices do grafo
 *        até o ponto inicial
 * 
 * @param noD vetor para o NoDijkstra que armazena as condições dos vértices no algoritmo
 * @param g ponteiro para o grafo 
 * @param noInicial vertice inicial do grafo 
 * @param numeroAbertos quantidade de vertices, usado para estabelecer o limite de repetições na função
 */
void calculaTempos(NoDijkstra *noD, Grafo *g ,int noInicial, int numeroAbertos){
    int u = noInicial;
    //inicializa o vetor noD de acordo com o vertice inicial
    //todos os nos sao abertos e o nó anterior é o -1, sendo um nó que não existe
    for(int i = 0; i < g->quantV; i++){
        //vertice inicial tem distancia 0, dele até ele mesmo
        if(i == noInicial){
            noD[i].p = -1;
            noD[i].d = 0;
            noD[i].aberto = 1;
            continue;
        }
        //outros vertices tem distancia infinita
        noD[i].p = -1;
        noD[i].d = INT_MAX/2;
        noD[i].aberto = 1;
    }

    while(numeroAbertos != 0){

        //guarda em u o primeiro nó aberto que encontrar
        for(int i = 0; i < g->quantV; i++){
            if(noD[i].aberto == 1){
                u = i;
                break;
            } 
        }

        //dentre todos os nós abertos guarda em u o que tiver a menor distancia até o inicial entre eles
        for(int i = 0; i < g->quantV; i++){
            if((noD[i].d < noD[u].d) && (noD[i].aberto == 1)) u = i;
        }

        noD[u].aberto = 0;
        numeroAbertos--;

        //relaxa o nó selecionado em u
        for(int i = 0; i < g->quantV; i++){
            if(g->matriz[u][i] != 0) relaxa(g,noD,u,i);
        }

    }
}

/**
 * @brief Função verifica o caminho do vertice inicial até o vertice final, cada nó guarda o no anterior a ele no caminho
 *        então a função segue esse nó anterior de cada vértice do vertice final até chegar no vertice inicial
 * 
 * @param noD vetor para o NoDijkstra que armazena as condições dos vértices no algoritmo
 * @param g ponteiro para o grafo
 * @param caminho vetor para guardar o caminho
 * @param noInicial vertice inicial do grafo
 * @param noFinal vertice final do grafo
 * @return int retorna 1 caso tenha um caminho entre os vertices final e inicial, se nao retorna 0
 */
int verificaCaminho(NoDijkstra *noD, Grafo *g ,int *caminho, int noInicial, int noFinal){
    //preenche o vetor caminho com -1
    for(int i = 0; i < g->quantV; i++){
        caminho[i] = -1;
    }

    int u = noFinal;
    int contaIndice = g->quantV-1;

    //o vetor é preenchido do fim para o comeco, sendo o primeiro indice a ser inserido no vetor caminho o proprio vertice final
    caminho[contaIndice] = u;
    while(u != noInicial){
        u = noD[u].p; 
        //se nao tiver caminho retorna 0
        if(u == -1) return 0;
        contaIndice--;

        caminho[contaIndice] = u;
    }
    return 1;
}

/**
 * @brief Procedimento que cria um relatório sobre o atendimento de acordo com o tipo de arquivo desejado e salva nesse tipo
 * 
 * @param noD vetor para o NoDijkstra que armazena as condições dos vértices no algoritmo
 * @param h ponteiro para a tabela hash
 * @param caminho vetor do caminho entre os vertices inicail e final
 * @param noInicial vertice inicial 
 * @param noFinal vertice final
 * @param numeroClientes numero de clientes cadastrados no sistema
 * @param tipoArquivo string com o tipo de arquivo desejado
 */
void criarRelatorio(NoDijkstra *noD ,Hash *h ,int *caminho ,int noInicial ,int noFinal, int numeroClientes, char *tipoArquivo){
    time_t tempo_atual;
    struct tm *info_tempo;

    time(&tempo_atual);
    info_tempo = localtime(&tempo_atual);

    //adquire a data e hora atual do computador
    int dia, mes, ano, hora, minuto;
    dia = info_tempo->tm_mday;
    mes = info_tempo->tm_mon + 1;
    ano = info_tempo->tm_year + 1900;
    hora = info_tempo->tm_hour;
    minuto = info_tempo->tm_min;

    //cria o nome do arquivo
    char relatorio[30];
    sprintf(relatorio,"Relatorio_%d_%d_%d_%d.%s",noFinal ,dia ,mes ,ano ,tipoArquivo); 
    
    
    //abre o arquivo em modo de escrita e imprime nele os dados necessariso do relatorio
    FILE *arq;
    arq = fopen(relatorio,"w");
    fprintf(arq,"Relatorio atendimento emergencial: %s\n",h->hashClientes[noFinal].nome);
    fprintf(arq,"Data: %d/%d/%d\n",dia ,mes ,ano);
    fprintf(arq,"Horario: %d:%d\n",hora ,minuto);
    fprintf(arq,"Equipe de atendimento: %d\n",noInicial);
    fprintf(arq,"Relatorio atendimento emergencial: ");
    for(int i = 0; i < numeroClientes; i++){
        if(caminho[i] != -1) fprintf(arq,"%d ",caminho[i]);
    }
    fprintf(arq,"\nTempo previsto de atendimento: %.2f\n",noD[noFinal].d);
    fclose(arq);
}

/**
 * @brief Procedimento que cria um relatório binario sobre o atendimento
 * 
 * @param noD vetor para o NoDijkstra que armazena as condições dos vértices no algoritmo
 * @param h ponteiro para a tabela hash
 * @param caminho vetor do caminho entre os vertices inicail e final
 * @param noInicial vertice inicial 
 * @param noFinal vertice final
 * @param numeroClientes numero de clientes cadastrados no sistema
 */
void criarRelatorioBinario(NoDijkstra *noD ,Hash *h ,int *caminho ,int noInicial ,int noFinal, int numeroClientes){
    time_t tempo_atual;
    struct tm *info_tempo;

    time(&tempo_atual);
    info_tempo = localtime(&tempo_atual);

    //adquire a data e hora atual do computador
    int dia, mes, ano, hora, minuto, inicio, fim;
    int tempoAtendimento;
    char linha[50];
    char nome[50];
    int valor[1];
    strcpy(nome,h->hashClientes[noFinal].nome);
    dia = info_tempo->tm_mday;
    mes = info_tempo->tm_mon + 1;
    ano = info_tempo->tm_year + 1900;
    hora = info_tempo->tm_hour;
    minuto = info_tempo->tm_min;
    inicio = noInicial;
    fim = noFinal;
    tempoAtendimento = noD[fim].d;


    //cria o nome do arquivo
    char relatorio[30];
    #ifdef _WIN32
        sprintf(relatorio,"Relatorio_%d_%d_%d_%d.bin",noFinal ,dia ,mes ,ano );
    #else
        sprintf(relatorio,"Relatorio_%d_%d_%d_%d",noFinal ,dia ,mes ,ano );
    #endif
    FILE *arq;
    arq = fopen(relatorio,"wb");

    strcpy(linha,"Relatorio atendimento emergencial: ");
    fwrite(linha,1,sizeof(linha),arq);
    fwrite(linha,1,sizeof(nome),arq);
    fwrite("\n",1,1,arq);

    strcpy(linha,"Data: ");
    fwrite(linha,1,sizeof(linha),arq);
    valor[0] = dia;
    fwrite(valor,4,1,arq);
    fwrite("/",1,1,arq);
    valor[0] = mes;
    fwrite(valor,4,1,arq);
    fwrite("/",1,1,arq);
    valor[0] = ano;
    fwrite(valor,4,1,arq);
    fwrite("\n",1,1,arq);

    strcpy(linha,"Horario: ");
    fwrite(linha,1,sizeof(linha),arq);
    valor[0] = hora;
    fwrite(valor,4,1,arq);
    fwrite(":",1,1,arq);
    valor[0] = minuto;
    fwrite(valor,4,1,arq);
    fwrite("\n",1,1,arq);

    strcpy(linha,"Equipe de atendimento: ");
    fwrite(linha,1,sizeof(linha),arq);
    valor[0] = inicio;
    fwrite(valor,4,1,arq);
    fwrite("\n",1,1,arq);

    strcpy(linha,"Relatorio atendimento emergencial");
    fwrite(linha,1,sizeof(linha),arq);
    for(int i = 0; i < numeroClientes; i++){
        if(caminho[i] != -1){
            valor[0] = caminho[i];
            fwrite(valor,4,1,arq);
            fwrite(" ",1,1,arq);
        }
    }
    fwrite("\n",1,1,arq);
    
    strcpy(linha,"Tempo previsto de atendimento: ");
    valor[0] = tempoAtendimento;
    fwrite(valor,4,1,arq);
    fwrite("\n",1,1,arq);

    fclose(arq);
}

/**
 * @brief Procedimento que cria um relatório sobre o atendimento e exibe na tela
 * 
 * @param noD vetor para o NoDijkstra que armazena as condições dos vértices no algoritmo
 * @param h ponteiro para a tabela hash
 * @param caminho vetor do caminho entre os vertices inicail e final
 * @param noInicial vertice inicial 
 * @param noFinal vertice final
 * @param numeroClientes numero de clientes cadastrados no sistema
 */
void exibirRelatorio(NoDijkstra *noD ,Hash *h ,int *caminho ,int noInicial ,int noFinal, int numeroClientes){
    time_t tempo_atual;
    struct tm *info_tempo;

    time(&tempo_atual);
    info_tempo = localtime(&tempo_atual);

    //adquire a data e hora atual do computador
    int dia, mes, ano, hora, minuto;
    dia = info_tempo->tm_mday;
    mes = info_tempo->tm_mon + 1;
    ano = info_tempo->tm_year + 1900;
    hora = info_tempo->tm_hour;
    minuto = info_tempo->tm_min;
 
    
    //abre o arquivo em modo de escrita e imprime nele os dados necessariso do relatorio
    printf("Relatorio atendimento emergencial: %s\n",h->hashClientes[noFinal].nome);
    printf("Data: %d/%d/%d\n",dia ,mes ,ano);
    printf("Horario: %d:%d\n",hora ,minuto);
    printf("Equipe de atendimento: %d\n",noInicial);
    printf("Relatorio atendimento emergencial: ");
    for(int i = 0; i < numeroClientes; i++){
        if(caminho[i] != -1) printf("%d ",caminho[i]);
    }
    printf("\nTempo previsto de atendimento: %.2f\n",noD[noFinal].d);
}

/**
 * @brief Procedimento que realiza o calculo de nova rota
 * 
 * @param g ponteiro para o grafo
 * @param h ponteiro para a tabela hash
 */
void encontrarMenorCaminhoDijkstra(Grafo *g, Hash *h){
    NoDijkstra noD[g->quantV];
    int noInicial;
    int noFinal;
    int numeroAbertos = g->quantV;
    int caminho[g->quantV];
    int temCaminho;
    int quantidadeEquipes;
    int a = 0;

    //verifica a quantidade de equipes de atendimento
    do{ 
        limpa_tela();
        if(a != 0) printf("Selecao invalida\n");
        printf("Insira a quantidade de equipes ativas no momento -> ");
        scanf("%d",&a);
        limpaBuffer();
    }while(a<=0);
    quantidadeEquipes = a;

    int localizacoesEquipes[quantidadeEquipes];
    double menorDistancia = INT_MAX/2;

    adquireLocalizacoes(h,localizacoesEquipes,&noFinal,g->quantV,quantidadeEquipes);

    //dentre todas as equipes verifica a que está mais próxima do local de atendimento
    for(int i = 0; i < quantidadeEquipes; i++){
        calculaTempos(noD,g,localizacoesEquipes[i],numeroAbertos);
        temCaminho = verificaCaminho(noD,g,caminho,localizacoesEquipes[i],noFinal);
        if(noD[noFinal].d < menorDistancia && temCaminho == 1){
            noInicial = localizacoesEquipes[i];
            menorDistancia = noD[noFinal].d;
        }
    }
    calculaTempos(noD,g,noInicial,numeroAbertos);
    temCaminho = verificaCaminho(noD,g,caminho,noInicial,noFinal);


//////////////
    //caso tenha alguma equipe que possa ir até o local de atendimento crio o relatorio do atendimento
    if(temCaminho == 1){
        criarRelatorio(noD,h,caminho,noInicial,noFinal,g->quantV,"txt");
        criarRelatorioBinario(noD,h,caminho,noInicial,noFinal,g->quantV);
        exibirRelatorio(noD,h,caminho,noInicial,noFinal,g->quantV);
    }else{
        printf("Nao há caminho entre os pontos \n");
    }
///////////////////////
    printf("\npressione enter para continuar... ");
    getchar();
    limpa_tela();
    limpaBuffer();
}

/**
 * @brief Procedimento do menu utilizado pela atendente para designar uma equipe para o local do atendimento
 * 
 */
void menu(){
    Hash hash;
    receberHashClientesArquivo(&hash);
    Grafo grafo;
    receberMatrizClientesArquivo(&grafo,&hash);

    int selecao;
    while(1){
        limpa_tela();
        printf("SOFTWARE DE ROTAS\n 1 - CALCULAR NOVA ROTA\n 0 - SAIR\n-> ");
        scanf("%d",&selecao);
        limpa_tela();
        limpaBuffer();
        switch (selecao)
        {
        case 1:
            encontrarMenorCaminhoDijkstra(&grafo,&hash);
            break;
        case 0:
            return;
            break;
        default:
            printf("selecao invalida\n");
            break;
        }
    }
    destroiMatrizDistancias(&hash);
    destruirGrafo(&grafo);
}