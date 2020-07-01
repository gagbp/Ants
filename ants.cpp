#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <utility>      /* pair */
#include <string>
#include <iostream>

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

void printMatriz(int **visited, int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            printf("%2d ", visited[i][j]);
        printf("\n");
    }
    printf("\n");
}

Formiga Cria_Formiga(int x, int y) // Cria um agente com posição <x,y>
{
  Formiga f;
  f.Carga = false;
  f.Posicao = std::make_pair(x,y);
  return f;
}

int main(int argc, char **argv)
{
  srand(time(NULL));
  Matriz M = Cria_Matriz(10);
  for (int i = 0; i < M.N; i++)
  {
    for (int j = 0; j < M.N; j++)
    {
      M.tab[i][j] = rand() % 2;
    }
  }
  
  printMatriz(M.tab, M.N);

  Formiga f0, f1, f2, f3, f4, f5, f6, f7, f8, f9;
  f0 = Cria_Formiga((rand()%10),(rand()%10));
  f1 = Cria_Formiga((rand()%10),(rand()%10));
  f2 = Cria_Formiga((rand()%10),(rand()%10));
  f3 = Cria_Formiga((rand()%10),(rand()%10));
  f4 = Cria_Formiga((rand()%10),(rand()%10));
  f5 = Cria_Formiga((rand()%10),(rand()%10));
  f6 = Cria_Formiga((rand()%10),(rand()%10));
  f7 = Cria_Formiga((rand()%10),(rand()%10));
  f8 = Cria_Formiga((rand()%10),(rand()%10));
  f9 = Cria_Formiga((rand()%10),(rand()%10));
  printf("f0: <%d,%d>\n", f0.Posicao.first, f0.Posicao.second);
  printf("f1: <%d,%d>\n", f1.Posicao.first, f1.Posicao.second);
  printf("f2: <%d,%d>\n", f2.Posicao.first, f2.Posicao.second);
  printf("f3: <%d,%d>\n", f3.Posicao.first, f3.Posicao.second);
  printf("f4: <%d,%d>\n", f4.Posicao.first, f4.Posicao.second);
  printf("f5: <%d,%d>\n", f5.Posicao.first, f5.Posicao.second);
  printf("f6: <%d,%d>\n", f6.Posicao.first, f6.Posicao.second);
  printf("f7: <%d,%d>\n", f7.Posicao.first, f7.Posicao.second);
  printf("f8: <%d,%d>\n", f8.Posicao.first, f8.Posicao.second);
  printf("f9: <%d,%d>\n", f9.Posicao.first, f9.Posicao.second);
  
  

  int a;
  scanf("%d", &a);
  return 0;
}