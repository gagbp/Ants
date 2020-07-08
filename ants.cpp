/*
  Compilar: g++ ants.cpp -fopenmp
  Executar: ./a.out N O P R
            N = Ordem da Matriz
            O = porcentagem de Objetos
            P = Passos
            R = Raio de visao
*/
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <utility>      /* pair */
#include <iostream>
#include <fstream>
#include <omp.h>
#include <ctime>
#include <list>
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

struct Matriz{
	int N; // Tamanho da matriz quadrada
  int** tab; // Ponteiro para a matriz NxN
};

struct Formiga{
	bool Carga; // F- não possui carga / V- possui carga
  std::pair <int,int> Posicao; // <x,y> - posição atual na matriz
  std::list <std::pair <int,int> > PosicaoAnt; // <x,y> - lista das posições anteriores da formiga
};

Matriz Cria_Matriz(int n){ // Cria matriz n x n
  Matriz m;
  m.N = n;
  m.tab = (int **)malloc(n * sizeof(int*));
  for(int i = 0; i < n; i++)
    m.tab[i] = (int *)malloc(n * sizeof(int));
  return m;
}

void Destroi_Matriz(Matriz m){
  for (int i = 0; i < m.N; i++)
    free(m.tab[i]);
  free(m.tab);  
}

void Print_Matriz(int **visited, int N){
		//if(system("CLS")) system("clear");
		printf("   |");
    for (int a = 0; a < N; a++)
      printf("%2d |",a);
    printf("\n");
    for (int a = 0; a <= N; a++)
      printf("----");
    printf("\n");
    for (int i = 0; i < N; i++)
    {
      printf("%2d |",i);
      for (int j = 0; j < N; j++)
      {
        if(visited[i][j] == 1)
          printf(" X |");
        else
          printf("   |");
      }
      printf("\n");
    }
    printf("\n");
		//Print_Matriz(visited,N);
}

Matriz Cria_MatrizVisao(Matriz campo, Formiga f, int Raio){
	Matriz visao;
	visao.N = Raio; // tamnho maximo para a visao da formiga , acho q ta bom
	visao.tab = (int **)malloc(Raio * sizeof(int*));
	for(int i = 0; i < Raio; i++){
		visao.tab[i] = (int *)malloc(Raio * sizeof(int));
	}

	int auxi,auxj;

	for (int i = 0 ; i < visao.N ; i++){
		for(int j = 0 ; j < visao.N ; j++){
			auxi = f.Posicao.first -1 + i;
			auxj = f.Posicao.second -1 +j;

		  if(auxi >= campo.N || auxi < 0 || auxj >= campo.N || auxj < 0){
			  visao.tab[i][j] = 0;
			}else{
			  visao.tab[i][j] = campo.tab[auxi][auxj];
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



Formiga Cria_Formiga(int x, int y, std::list <std::pair <int,int> > antigas){ // Cria um agente com posição <x,y>

  Formiga f;
  f.Carga = false;
  f.Posicao = std::make_pair(x,y);
  f.PosicaoAnt = antigas;
  return f;
}

bool ProcuraParNoVetor(std::list <std::pair <int,int>> v, std::pair <int,int> par){
	for(auto b : v){
		if(b.first == par.first && b.second == par.second){
			return true;
		}
	}
	return false;
}

Formiga Move_Formiga(Formiga F, int N){ // Move o agente F
  Formiga Aux;
  bool a = true;
  bool cond;
  int count = 0;
  while (a)
  {
    int d = rand()%8;
    Aux = Cria_Formiga(F.Posicao.first, F.Posicao.second,F.PosicaoAnt);
	  if(F.Posicao.first == 0) {
      if(F.Posicao.second == 0) {//<0,0>
        /*
          X 0
          1 2
        */
        d = rand()%3;
        switch(d){
          case 0:
            Aux.Posicao.second++;
            break;
          case 1:
            Aux.Posicao.first++;
            break;
          case 2:
            Aux.Posicao.first++;
            Aux.Posicao.second++;
            break;
          default:
            break;
        }
      }
      else {
        if(F.Posicao.second == N-1) {//<0,n-1>
          /*
            0 X
            1 2
          */
          d = rand()%3;
          switch(d){
            case 0:
              Aux.Posicao.second--;
              break;
            case 1:
              Aux.Posicao.first++;
              Aux.Posicao.second--;
              break;
            case 2:
              Aux.Posicao.first++;
              break;
            default:
              break;
          }
        }
        else { //<0,y>
          /*
            0 X 1
            2 3 4
          */
          d = rand()%5;
          switch(d) {
            case 0:
              Aux.Posicao.second--;
              break;
            case 1:
              Aux.Posicao.second++;
              break;
            case 2:
              Aux.Posicao.first++;
              Aux.Posicao.second--;
              break;
            case 3:
              Aux.Posicao.first++;
              break;
            case 4:
              Aux.Posicao.first++;
              Aux.Posicao.second++;
              break;
            default:
              break;
          }
        }
      }
    }
    else {
      if(F.Posicao.first == N-1) {
        if(F.Posicao.second == 0) {//<n-1,0>
          /*
            0 1
            X 2
          */
          d = rand()%3;
          switch(d) {
            case 0:
              Aux.Posicao.first--;
              break;
            case 1:
              Aux.Posicao.first--;
              Aux.Posicao.second++;
              break;
            case 2:
              Aux.Posicao.second++;
              break;
            default:
              break;
          }
        }
        else {
          if(F.Posicao.second == N-1) {//<n-1,n-1>
            /*
              0 1
              2 X
            */
            d = rand()%3;
            switch(d) {
              case 0:
                Aux.Posicao.first--;
                Aux.Posicao.second--;
                break;
              case 1:
                Aux.Posicao.first--;
                break;
              case 2:
                Aux.Posicao.second--;
                break;
              default:
                break;
            }
          }
          else{//<n-1,y>
            /*
              0 1 2
              3 X 4
            */
            d = rand()%5;
            switch(d) {
              case 0:
                Aux.Posicao.first--;
                Aux.Posicao.second--;
                break;
              case 1:
                Aux.Posicao.first--;
                break;
              case 2:
                Aux.Posicao.first--;
                Aux.Posicao.second++;
                break;
              case 3:
                Aux.Posicao.second--;
                break;
              case 4:
                Aux.Posicao.second++;
                break;
              default:
                break;
            }
          }
        }
      }
      else{
        if(F.Posicao.second == 0){//<x,0>
          /*
          0 1
          X 2
          3 4
          */
          d = rand()%5;
          switch(d) {
            case 0:
              Aux.Posicao.first--;
              break;
            case 1:
              Aux.Posicao.first--;
              Aux.Posicao.second++;
              break;
            case 2:
              Aux.Posicao.second++;
              break;
            case 3:
              Aux.Posicao.first++;
              break;
            case 4:
              Aux.Posicao.first++;
              Aux.Posicao.second++;
              break;
            default:
              break;
          }
        }
        else{
          if(F.Posicao.second == N-1){//<x,0>
            /*
            0 1
            2 X
            3 4
            */
            d = rand()%5;
            switch(d) {
              case 0:
                Aux.Posicao.first--;
                Aux.Posicao.second--;
                break;
              case 1:
                Aux.Posicao.first--;
                break;
              case 2:
                Aux.Posicao.second--;
                break;
              case 3:
                Aux.Posicao.first++;
                Aux.Posicao.second--;
                break;
              case 4:
                Aux.Posicao.first++;
                break;
              default:
                break;
            }
          }
          else{//<x,y>
            /*
            0 1 2
            3 X 4
            5 6 7
            */
            d = rand()%8;
            switch (d) {
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
          }
        }
      }
    }
    
		cond = ProcuraParNoVetor(Aux.PosicaoAnt,Aux.Posicao);
    if(!cond || count >= 3)
      a = false;
    else
      count++;
  }
  Aux.Carga = F.Carga;
  if(Aux.PosicaoAnt.size() >= 5){
		Aux.PosicaoAnt.pop_front();
	}
  Aux.PosicaoAnt.push_back(F.Posicao);
  
  return Aux;
}

bool Pegar(Matriz visao){
  int cont = 0;
  for (int i = 0; i < visao.N; i++) {
    for (int j = 0; j < visao.N; j++) {
      if (visao.tab[i][j]==1) {
        cont++;
      }
    }
  }
  if (cont< (visao.N*visao.N)/2) {
    return true;
  }
  return false;
}

bool Soltar(Matriz visao){
  int cont = 0;
  for (int i = 0; i < visao.N; i++) {
    for (int j = 0; j < visao.N; j++) {
      if (visao.tab[i][j]==1) {
        cont++;
      }
    }
  }
  if (cont>= (visao.N*visao.N)/2) {
    return true;
  }
  return false;
}



int main(int argc, char **argv){
/*
  N = Ordem da Matriz
  O = porcentagem de Objetos
  P = Passos
  R = Raio de visao
*/

  int N = atoi(argv[1]);
  int O = atoi(argv[2]);
  int P = atoi(argv[3]);
  int R = 2*atoi(argv[4]) + 1;

	
  int nthreads,tid;
	int randaux;

	srand( time(0));
  Matriz M = Cria_Matriz(N);// Cria o ambiente
  int count = 0;
  for (int i = 0; i < M.N; i++)// Preenche a matriz com 0 e 1 randomicamente
  {
    for (int j = 0; j < M.N; j++)
    {
			randaux = rand()%4;
			if(randaux == 1){
			  M.tab[i][j] = 1;
        count++;
      }else{
        M.tab[i][j] = 0;
      }
		}
  }

	Print_Matriz(M.tab, M.N);
  // Cria os agentes em posição de agentes

  // printf("f0: <%d,%d>\n", f0.Posicao.first, f0.Posicao.second);
  
	omp_set_dynamic(0);
	omp_set_num_threads(10); // numero de threads
  #pragma omp parallel private(nthreads,tid) shared(M)
	{
    Formiga f0;
		std::list <std::pair <int,int>> P_antigas;
    Matriz visao;
		tid = omp_get_thread_num();// tid != 0 => thread slave
		f0 = Cria_Formiga((rand()%N), (rand()%N), P_antigas);
    //printf("thread %d: f0<%d,%d>\n", tid, f0.Posicao.first, f0.Posicao.second);
    for(int p = 0;p < P; p++) // cada Formiga da 1000 p ate morrer
    {
			{
				srand((int) time(0));
				f0 = Move_Formiga(f0,N);
				//printf("f%d %s : <%d,%d>\n",tid, f0.Carga ? "True":"False", f0.Posicao.first, f0.Posicao.second);
				//criar matriz visao
				visao = Cria_MatrizVisao(M,f0,R);
				if(M.tab[f0.Posicao.first][f0.Posicao.second] == 1){ //encontrou sugeira
          if(!f0.Carga && Pegar(visao)){ // pode pegar
            //printf("%d pegou <%d,%d>\n", tid, f0.Posicao.first, f0.Posicao.second);
            f0.Carga = true; //pegou
            #pragma omp critical
            {
              M.tab[f0.Posicao.first][f0.Posicao.second] = 0; // liberou lugar
            }
          }
				}else{ // ta limpo
					if(Soltar(visao) && f0.Carga){ // pode soltar
						//printf("%d largou <%d,%d>\n", tid, f0.Posicao.first, f0.Posicao.second);
						f0.Carga = false;
						#pragma omp critical
            {
              M.tab[f0.Posicao.first][f0.Posicao.second] = 1; // liberou carga
            }
					}
        }
        //libera matriz visao;
        LiberarMatrizVisao(visao);
      }
      //Completou todos p mas ainda tem Carga
      if(p == P-1 && f0.Carga)
        p--;

	  }
    printf("f%d %s\n", tid, f0.Carga ? "True":"False");
	}
  #pragma omp barrier
  Print_Matriz(M.tab, M.N);
  /*
  printf("\nf0: <%d,%d>\nf0: <%d,%d>\n", f0.Posicao.first, f0.Posicao.second, f0.PosicaoAnt.first, f0.PosicaoAnt.second);
  */
  //int a;
  //scanf("%d", &a);
	return 0;
}
