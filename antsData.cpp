/*
  Compilar: g++ antsData.cpp -fopenmp -std=c++11
  Executar: ./a.out N D P R F
            N = Ordem da Matriz
            D = Arquivo Data
            P = Passos
            R = Raio de visao
            F = Numero de Formigas

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
#include <algorithm>
#include <math.h>
#include <random>
#include <chrono>
#include <fstream>

std::default_random_engine dre (std::chrono::steady_clock::now().time_since_epoch().count());     // provide seed
int random (int lim){
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int> uid{0,lim};   // help dre to generate nos from 0 to lim (lim included);
    return uid(dre);    // pass dre as an argument to uid to generate the random no
}

struct Dado {
  double X;
  double Y;
  int Rotulo;
};

struct Matriz{
	int N; // Tamanho da matriz quadrada
  Dado** tab; // Ponteiro para a matriz de Dados NxN
};

struct Formiga{
	Dado Carga; // 0- não possui carga / carga > 0- possui carga rotulada
  std::pair <int,int> Posicao; // <x,y> - posição atual na matriz
  std::list <std::pair <int,int> > PosicaoAnt; // <x,y> - lista das posições anteriores da formiga
};



Matriz Cria_Matriz(int n){ // Cria matriz n x n
  Matriz m;
  m.N = n;
  m.tab = (Dado **)malloc(n * sizeof(Dado*));
  for(int i = 0; i < n; i++)
    m.tab[i] = (Dado *)malloc(n * sizeof(Dado));
  return m;
}

void Destroi_Matriz(Matriz m){
  for (int i = 0; i < m.N; i++)
    free(m.tab[i]);
  free(m.tab);
}

void Print_Matriz(Dado **visited, int N){
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
        if(visited[i][j].Rotulo != 0)
          printf("%2d |",visited[i][j].Rotulo);
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
	visao.tab = (Dado **)malloc(Raio * sizeof(Dado*));
	for(int i = 0; i < Raio; i++){
		visao.tab[i] = (Dado *)malloc(Raio * sizeof(Dado));
	}

	int auxi,auxj;

	for (int i = 0 ; i < visao.N ; i++){
		for(int j = 0 ; j < visao.N ; j++){
			auxi = f.Posicao.first - ( Raio-1 /2) + i;
			auxj = f.Posicao.second -( Raio-1 /2) + j;
		  //if(auxi >= campo.N || auxi < 0 || auxj >= campo.N || auxj < 0){
			if(auxi >= campo.N){
				auxi = auxi - campo.N;
			}
			if(auxi < 0){
				auxi = campo.N + auxi;
			}
			if(auxj >= campo.N){
				auxj = auxj - campo.N;
			}
			if(auxj < 0){
				auxj = campo.N + auxj;
			}

			visao.tab[i][j] = campo.tab[auxi][auxj];
	  }
  }
	return visao;
}

void LiberarMatrizVisao(Matriz visao){
	for(int i = 0 ; i< visao.N ; i++){
		free(visao.tab[i]);
	}
	free(visao.tab);
}



Formiga Cria_Formiga(int x, int y, std::list <std::pair <int,int> > antigas){ // Cria um agente com posição <x,y>

  Formiga f;

  Dado Carga_aux;
  Carga_aux.X = 0;
  Carga_aux.Y = 0 ;
  Carga_aux.Rotulo = 0 ;

  f.Carga = Carga_aux;
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
    int d = random(7);
    Aux = Cria_Formiga(F.Posicao.first, F.Posicao.second,F.PosicaoAnt);
	  d = random(7);
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

              if(Aux.Posicao.first >= N){ // caso maior q maixmo em X
        				Aux.Posicao.first = Aux.Posicao.first - N; // entao volta para 0
        			}
        			if(Aux.Posicao.first < 0){ //caso menor 0 ou seja -1 -> vai para max-1
        				Aux.Posicao.first = N + Aux.Posicao.first;
        			}
        			if(Aux.Posicao.second >= N){
        				Aux.Posicao.second = Aux.Posicao.second - N;
        			}
        			if(Aux.Posicao.second < 0){
        				Aux.Posicao.second = N + Aux.Posicao.second;
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

double distEuclidiana(Dado a, Dado b){
  double c1 , c2;
  c1 = pow( (a.X+b.X),2);
  c2 = pow( (a.Y+b.Y),2);
  //printf("c1 :%lf  c2: %lf \n",c1,c2 );
  return sqrt(c1+c2);
}

bool Pegar(Matriz visao){

if(visao.tab[(visao.N/2)][(visao.N/2)].Rotulo == 0){
  return false;
}

  int qtd_dados_vizinhos = 0;
  double somatorio_de_1menosdistporalpha = 0.0;  //E (1-d(i,j)/a)
  double fij;
  // parametros constantes definidos impiricamento
  double k = 0.7; //[ 0 a 1 ]
  // parametros
  double alpha = 0.0;
  // paremtro alpha
  for (int i = 0; i < visao.N; i++) {
    for (int j = 0; j < visao.N; j++) {
      if (visao.tab[i][j].Rotulo!=0 ) { //(visao.N/2) +1  eh o lugar onde a formiga esta
        double d =  distEuclidiana(visao.tab[i][j],visao.tab[(visao.N/2)][(visao.N/2)]);
        alpha = alpha + d;
        qtd_dados_vizinhos++;
      }
    }
  }
  alpha = alpha/qtd_dados_vizinhos;


  for (int i = 0; i < visao.N; i++) {
    for (int j = 0; j < visao.N; j++) {
      if (visao.tab[i][j].Rotulo!=0) { //(visao.N/2) +1  eh o lugar onde a formiga esta
        double d =  distEuclidiana(visao.tab[i][j],visao.tab[(visao.N/2)][(visao.N/2)]);
        if(alpha == 0){ alpha = 1 ;} // pra nao dividir por  0
        somatorio_de_1menosdistporalpha = somatorio_de_1menosdistporalpha + (1 - (d/ (alpha*1.2)));
        //qtd_dados_vizinhos++;
      }
    }
  }
  //int aux = visao.N -1;
  //int func = cont/aux;
  //printf("somatorio : %lf\n", somatorio_de_1menosdistporalpha );
  fij = somatorio_de_1menosdistporalpha/pow(qtd_dados_vizinhos  ,2);
  //printf("%lf\n", fij);
  double func = pow(k/(k + fij),2);
  long int probilidade = (long int) 100 * func; // probilidad de pegar
  //printf("MINHA VISAO:\n");
  //Print_Matriz(visao.tab,visao.N);
  //printf("PROB DE PEGAR %d\n" ,probilidade );
  int r = random(100);
	return (probilidade >= r);
}

bool Soltar(Matriz visao){
  if(visao.tab[(visao.N/2)][(visao.N/2)].Rotulo != 0){
    return false;
  }

  int qtd_dados_vizinhos = 0;
  double somatorio_de_1menosdistporalpha = 0.0;  //E (1-d(i,j)/a)
  double fij;

  // parametros constantes definidos impiricamento
  double k = 0.5; //[ 0 a 1 ]
  // parametros

  double alpha = 0.0;
  // paremtro alpha
  for (int i = 0; i < visao.N; i++) {
    for (int j = 0; j < visao.N; j++) {
      if (visao.tab[i][j].Rotulo!=0 ) { //(visao.N/2) +1  eh o lugar onde a formiga esta
        double d =  distEuclidiana(visao.tab[i][j],visao.tab[(visao.N/2)][(visao.N/2)]);
        alpha = alpha + d;
        qtd_dados_vizinhos++;
      }
    }
  }
  alpha = alpha/qtd_dados_vizinhos;


  for (int i = 0; i < visao.N; i++) {
    for (int j = 0; j < visao.N; j++) {
      if (visao.tab[i][j].Rotulo != 0)  { // nao vai contar o centro
        double d =  distEuclidiana(visao.tab[i][j],visao.tab[(visao.N/2)][(visao.N/2)]);
        double aux_e =(d);
        if(alpha == 0){ alpha = 1 ;} // pra nao dividir por zero
        //printf("aux : %lf\n",aux_e );
        somatorio_de_1menosdistporalpha = somatorio_de_1menosdistporalpha + (1 - (d/(alpha *1.2  )));
      }
    }
  }
  //int aux = visao.N -1;
  //int func = cont/aux;

  //printf("somatorio : %lf\n", somatorio_de_1menosdistporalpha );
  fij = somatorio_de_1menosdistporalpha/pow(qtd_dados_vizinhos,2);
  //printf("%lf\n",fij );
  double func = pow(fij/(k + fij),2);
  long int probilidade = (long int) 100 * func; // probilidad de largar
  //printf("MINHA VISAO:\n");
  //Print_Matriz(visao.tab,visao.N);
  //printf("PROB DE SOLTAR %d\n",probilidade );
  int r = random(100);
  //printf("RANDOM GERADO : %d\n",r );

  return (probilidade >= r);
}

void Encher_Matriz(Matriz m,std::string D){

  // le do arquivo
  std::string qtd;
  double X,Y;
  int Rotulo;
  std::ifstream data(D);

  data >> qtd;
  printf("quantidade %d\n", stoi(qtd));

  if(stoi(qtd) >= m.N*m.N ){
    printf("Matriz muito pequena para arquivo\n");
    exit(EXIT_FAILURE);
  }
  int cont_aux = 0;
	for (int i = 0; i < m.N; i++){// Preenche a matriz com 0 e 1 randomicamente
    for (int j = 0; j < m.N; j++){
			if(cont_aux <= stoi(qtd) ){
        data >> X >> Y >> Rotulo;

        m.tab[i][j].X = X;
        m.tab[i][j].Y = Y;
        m.tab[i][j].Rotulo = Rotulo;
        cont_aux++;
      }else{
        m.tab[i][j].X = 0;
        m.tab[i][j].Y = 0;
        m.tab[i][j].Rotulo = 0;
      }
    }
  }

	//std::random_shuffle(m.tab, (m.tab + m.N));
	for (int i = m.N -1 ; i != 0; i--){
		for (int j = m.N -1 ;j != 0; j--){
			int randi = random(m.N-1);
			int randj = random(m.N-1);
			Dado aux =  m.tab[i][j];
			m.tab[i][j] = m.tab[randi][randj];
			m.tab[randi][randj] = aux;

		}
	}
}
/*
double Get_parametroAlpha(std::string nome_file){
  std::string qtd;
  double X,Y;
  int Rotulo;
  std::ifstream data(nome_file);
  data >> qtd;
  printf("quantidade %d\n", stoi(qtd));
  Dado *vetor_dados =(Dado*) malloc(sizeof(Dado) * stoi(qtd));

  int cont_aux = 0;
	for (int i = 0; i < stoi(qtd); i++){// Preenche a matriz com 0 e 1 randomicamente
    data >> X >> Y >> Rotulo;
    vetor_dados[i].X = X;
    vetor_dados[i].Y = Y;
    vetor_dados[i].Rotulo = Rotulo;
  }
  double soma_distancia = 0;
  for (int i = 0; i < stoi(qtd); i++){
    for (int j = 0; j < stoi(qtd); j++){
      soma_distancia = soma_distancia + distEuclidiana(vetor_dados[j],vetor_dados[i]);
    }
  }
  free(vetor_dados);
  return soma_distancia/(stoi(qtd)*stoi(qtd));
}
*/
int fileout_matriz (Matriz m, FILE *infile){

	fprintf(infile,"   |");
	for (int a = 0; a < m.N; a++)
		fprintf(infile,"%2d |",a);
	fprintf(infile,"\n");
	for (int a = 0; a <= m.N; a++)
		fprintf(infile,"----");
	fprintf(infile,"\n");
	for (int i = 0; i < m.N; i++){
		fprintf(infile,"%2d |",i);
		for (int j = 0; j < m.N; j++){
			if(m.tab[i][j].Rotulo != 0)
				fprintf(infile," X |");
			else
				fprintf(infile,"   |");
		}
		fprintf(infile,"\n");
	}
	fprintf(infile,"\n");
  return 0;
}


int main(int argc, char **argv){
  /*
    N = Ordem da Matriz
    O = porcentagem de Objetos
    P = Passos
    R = Raio de visao
    F = Numero de Formigas
  */

  int N = atoi(argv[1]);
  std::string D = argv[2];
  int P = atoi(argv[3]);
  int R = 2*atoi(argv[4]) + 1;
  int F = atoi(argv[5]);

  int nthreads,tid;
	int randaux;

	char filename[100];
	FILE *fmat;

	srand( time(0));
  Matriz M = Cria_Matriz(N);// Cria o ambiente
  int count = 0;
	//printf("%d\n",O );

  Encher_Matriz(M,D);
  
  if(M.N <= 30){
	  Print_Matriz(M.tab, M.N);
	}
  // Cria os agentes em posição de agentes

	sprintf(filename, "matriz.txt");
	fmat = fopen(filename,"w");
  fprintf(fmat,"Matriz Inicial\n\n");
	fileout_matriz(M, fmat);
  fprintf(fmat,"\n\n\nAlterações\n");
	fclose(fmat);

  // printf("f0: <%d,%d>\n", f0.Posicao.first, f0.Posicao.second);

	omp_set_dynamic(0);
	omp_set_num_threads(F); // numero de threads
  #pragma omp parallel private(nthreads,tid) shared(M)
	{
    Formiga f0;
		std::list <std::pair <int,int>> P_antigas;
    Matriz visao;
		tid = omp_get_thread_num();// tid != 0 => thread slave
		f0 = Cria_Formiga((random(N-1)), (random(N-1)), P_antigas);
    //printf("thread %d: f0<%d,%d>\n", tid, f0.Posicao.first, f0.Posicao.second);
    for(int p = 0;p < P; p++) // cada Formiga da P passos ate morrer
    {
			srand((int) time(0));
			f0 = Move_Formiga(f0,N);
			//printf("f%d %s : <%d,%d>\n",tid, f0.Carga ? "True":"False", f0.Posicao.first, f0.Posicao.second);
			//criar matriz visao
			visao = Cria_MatrizVisao(M,f0,R);
      #pragma omp critical
      {
      if(M.tab[f0.Posicao.first][f0.Posicao.second].Rotulo != 0){ //encontrou sugeira
        if(f0.Carga.Rotulo == 0 && Pegar(visao)){ // pode pegar
          //printf("%d pegou <%d,%d>\n", tid, f0.Posicao.first, f0.Posicao.second);
          f0.Carga = M.tab[f0.Posicao.first][f0.Posicao.second]; //pegou

          M.tab[f0.Posicao.first][f0.Posicao.second].Rotulo = 0; // liberou lugar
          M.tab[f0.Posicao.first][f0.Posicao.second].X = 0; // liberou lugar
          M.tab[f0.Posicao.first][f0.Posicao.second].Y = 0; // liberou lugar

          sprintf(filename, "matriz.txt");
          fmat = fopen(filename,"a");
          fprintf(fmat,"%d pegou <%d,%d>\n", tid, f0.Posicao.first, f0.Posicao.second);
          fclose(fmat);

        }
		  }else{ // ta limpo
			  if(Soltar(visao) && f0.Carga.Rotulo != 0){ // pode soltar
					 // printf("%d largou <%d,%d>\n", tid, f0.Posicao.first, f0.Posicao.second);
            //printf("SOLTOU UM %d \n",f0.Carga.Rotulo );
            M.tab[f0.Posicao.first][f0.Posicao.second].Rotulo = f0.Carga.Rotulo; // liberou carga
            M.tab[f0.Posicao.first][f0.Posicao.second].X = f0.Carga.X; // liberou lugar
            M.tab[f0.Posicao.first][f0.Posicao.second].Y = f0.Carga.Y; // liberou lugar

            f0.Carga.Rotulo = 0;
            f0.Carga.X = 0;
            f0.Carga.Y = 0;

            sprintf(filename, "matriz.txt");
            fmat = fopen(filename,"a");
            fprintf(fmat,"%d soltou <%d,%d>\n", tid, f0.Posicao.first, f0.Posicao.second);
            fclose(fmat);
          }
				}
      }
      //libera matriz visao;
      LiberarMatrizVisao(visao);
      //Completou todos p mas ainda tem Carga
      if(p == P-1 && f0.Carga.Rotulo != 0)
        p--;
		}

    printf("f%d OK\n", tid);
  }
  #pragma omp barrier

	if(M.N <= 30){
	   Print_Matriz(M.tab, M.N);
	}

	sprintf(filename, "matriz.txt" );
	fmat = fopen(filename,"a");
	fprintf(fmat,"Matriz Final\n\n");
	fileout_matriz(M, fmat);
	fclose(fmat);
  Destroi_Matriz(M);

  return 0;
}
