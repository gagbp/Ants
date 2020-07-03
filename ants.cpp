#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <utility>      /* pair */
#include <iostream>
#include <fstream>
#include <omp.h>
/*
Variáveis do sistema
  Tamanho da matriz
  Numero de agentes 10
  Raio de visão 1
  Numero de itens
  Critério de parada

Step-by-Step
  Criar ambiente x
  Dispor itens  x
  Dispor agentes  x
  Definir deslocamento
  Definir regras de pegar/largar

Regras
  Regra de movimentação


  Regra de pegar/largar
    Se ( Não tenho item)
      Regra para pegar um item
      Se (Casa ocupada)
        Devo pegar? Analisar os vizinhos
        Se (Área populada)
          Chance baixa de pegar
        else
          Chance alta de pegar
      else
        Mudar casa
    else
      Regra para soltar um item
      Se (Casa vazia)
        Devo largar? Analisar os vizinhos
        Se (Area populada)
          Chance alta de largar
        else
          Chance baixa de largar
      else
        Mudar casa

*/

struct Matriz
{
	int N;
  int** tab;
};

struct Formiga
{
	bool Carga; // F- não possui carga / V- possui carga
  std::pair <int,int> Posicao; // <x,y> - posição atual na matriz
  std::pair <int,int> PosicaoAnt; // <x,y> - posição anterior na matriz
};

Matriz Cria_Matriz(int n) // Cria matriz n x n
{
  Matriz m;
  m.N = n;
  m.tab = (int **)malloc(n * sizeof(int*));
  for(int i = 0; i < n; i++)
    m.tab[i] = (int *)malloc(n * sizeof(int));
  return m;
}

Matriz Cria_MatrizVisao(Matriz campo,Formiga f){
	Matriz visao;
	visao.N = 3; // tamnho maximo para a visao da formiga , acho q ta bom
	visao.tab = (int **)malloc(3 * sizeof(int*));
	for(int i = 0; i < 3; i++){
		visao.tab[i] = (int *)malloc(3 * sizeof(int));
	}
	for (int i = 0 ; i < visao.N ; i++){
		for(int j = 0 ; j < visao.N ; j++){
			try{
				visao.tab[i][j] = campo.tab[f.Posicao.first -1 + i][f.Posicao.second -1 +j];
			} catch(const std::exception& e) {
				visao.tab[i][j] = 1; // pode acontecer da visao passar do maximo da matriz, entao deixei  1 para nao atrbalhar a visao;
			}

		}
	}
	return visao;
}

void LiberarMatrizVisao(Matriz visao){
	for(int i = 0 ; i< 3 ; i++){
		free(visao.tab[i]);
	}
	free(visao.tab);
}

void Print_Matriz(int **visited, int N)
{
		//if(system("CLS")) system("clear");
		for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            printf("%2d ", visited[i][j]);
        printf("\n");
    }
    printf("\n");
		//Print_Matriz(visited,N);
}




Formiga Cria_Formiga(int x, int y) // Cria um agente com posição <x,y>
{
  Formiga f;
  f.Carga = false;
  f.Posicao = std::make_pair(x,y);
  f.PosicaoAnt = std::make_pair(x,y);
  return f;
}

Formiga Move_Formiga(Formiga F, int N) // Move o agente F
{
  /*
    0 1 2
    3 X 4
    5 6 7
  */
  srand(time(NULL));
  Formiga Aux;
  bool a = true;
  while (a)
  {
    Aux = Cria_Formiga(F.Posicao.first, F.Posicao.second);
    int d = rand()%8;
    switch (d)
    {
      case 0:
        Aux.Posicao.first--;
        Aux.Posicao.second--;
        break;
      case 1:
        Aux.Posicao.second--;
        break;
      case 2:
        Aux.Posicao.first++;
        Aux.Posicao.second--;
        break;
      case 3:
        Aux.Posicao.second--;
        break;
      case 4:
        Aux.Posicao.second++;
        break;
      case 5:
        Aux.Posicao.first++;
        Aux.Posicao.second--;
        break;
      case 6:
        Aux.Posicao.first++;
        break;
      case 7:
        Aux.Posicao.first++;
        Aux.Posicao.second++;
        break;
      default:
        break;
    }
    if (!(Aux.Posicao.first < 0 || Aux.Posicao.second < 0 || Aux.Posicao.first >= N || Aux.Posicao.second >= N || Aux.Posicao == Aux.PosicaoAnt))
      a = false;
  }
  return Aux;
}

bool Pegar(Matriz visao)
{
  int cont = 0;
  for (int i = 0; i < visao.N; i++)
  {
    for (int j = 0; j < visao.N; j++)
    {
      if (visao.tab[i][j]==1)
      {
        cont++;
      }
    }
  }
  if (cont< (visao.N*visao.N)/2)
  {
    return true;
  }
  return false;
}

bool Soltar(Matriz visao)
{
  int cont = 0;
  for (int i = 0; i < visao.N; i++)
  {
    for (int j = 0; j < visao.N; j++)
    {
      if (visao.tab[i][j]==1)
      {
        cont++;
      }
    }
  }
  if (cont>= (visao.N*visao.N)/2)
  {
    return true;
  }
  return false;
}

int main(int argc, char **argv)
{
  int N = 20;
	int nthreads,tid;
	srand(time(NULL));
  Matriz M = Cria_Matriz(N);// Cria o ambiente
  for (int i = 0; i < M.N; i++)// Preenche a matriz com 0 e 1 randomicamente
  {
    for (int j = 0; j < M.N; j++)
    {
      M.tab[i][j] = rand() % 2;
    }
  }

	Print_Matriz(M.tab, M.N);
 // Cria os agentes em posição de agentes

/*
  printf("f0: <%d,%d>\n", f0.Posicao.first, f0.Posicao.second);
*/
	#pragma omp parallel private(nthreads,tid) shared(M)
	{
		tid = omp_get_thread_num();
		if(tid != 0){ // tid 0 => thread master
			Formiga f0;
			Matriz visao;
			f0 = Cria_Formiga((rand()%N),(rand()%N));
			for(int passos = 0 ; passos < 10 ; passos++){ // cada Formiga da 1000 passos ateh morrer
					f0 = Move_Formiga(f0,N);
					#pragma omp critical
					{
						//criar matriz visao
						visao = Cria_MatrizVisao(M,f0);
						if(M.tab[f0.Posicao.first][f0.Posicao.second] == 1){ //encontrou sugeira
							if(Pegar(visao)){ // se pode pegar
								f0.Carga = true; //pegou
								M.tab[f0.Posicao.first][f0.Posicao.second] = 0; // liberou lugar
							}
						}else{ // ta limpo
							if(Soltar(visao) && f0.Carga){
								f0.Carga = false;
								M.tab[f0.Posicao.first][f0.Posicao.second] = 1; // liberou carga
							}
					}
					printf("oi da thread %d\n",tid);
			//libear matriz visao;
			LiberarMatrizVisao(visao);
			}
		}

		}else{
			int a;
			scanf("%d", &a);
		}
	}

/*
printf("\nf0: <%d,%d>\nf0: <%d,%d>\n", f0.Posicao.first, f0.Posicao.second, f0.PosicaoAnt.first, f0.PosicaoAnt.second);
*/
  //int a;
  //scanf("%d", &a);
	Print_Matriz(M.tab, M.N);
	return 0;
}
