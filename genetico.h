#include "systemc.h"
#include "parameters.h"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>

using namespace std;

class genetico{
public:
	conexoes conexoes_rede[SIZE_POPULATION];
	int coreNumbers;
	float deadline[SIZE_POPULATION];
	float latencia[SIZE_POPULATION];
	string grau_latencia[SIZE_POPULATION];
	char linha[15];
	int pai[5], mae[5], sub1[5], sub2[5];

	/*Verifica se o valor criado para o mapeamento já se encontra nele*/
	bool verifica(Conexoes valores[], int tam, int valor){
    	for(int i = 0; i < tam; i++){
        	if(valores[i].primeiro == valor){
        	    return true;
        	}
    	}
    	return false;
	}

	/*Verifica se o valor criado para o mapeamento já se encontra nele*/
	bool verifica2(Conexoes valores[], int tam, int valor){
		for(int i = 0; i < tam; i++){
        	if(valores[i].segundo == valor){
                return true;
        	}
    	}
   		return false;
	}

	/*Verifica se todas as conexões são válidas*/
	bool verifica_conexao(Conexoes valores[], int tam, int valor1, int valor2){
		for(int i = 0; i < tam; i++){
        	if((valor2 == valores[i].primeiro) && (valor1 == valores[i].segundo)){
        	    return true;
        	}
    	}
   		return false;
	}

	
	/*Cria a primeira população de mapeamento*/
	void firstPopulation(){
		Conexoes mapTeste[coreNumbers];
    	int v;
	    int w;
	    srand(time(NULL));
		for(int i = 0; i < SIZE_POPULATION; i++){
		   for(int j = 0; j < coreNumbers; j++){
		      v = 1+(rand()%coreNumbers);
		      w = 1+(rand()%coreNumbers);
		      while(verifica(mapTeste, j, v)){
	             v = 1+(rand()%coreNumbers);
	 	      }
	 	      while(verifica2(mapTeste, j, w)){
	            w = 1+(rand()%coreNumbers);
	 	      }
	 	      while(v == w){
	            v = 1+(rand()%coreNumbers);
	 	      }
	 	      while(verifica(mapTeste, j, v)){
	             v = 1+(rand()%coreNumbers);
	 	      }
	 	     /* while(verifica_conexao(mapTeste, j, v, w)){
	            v = 1+(rand()%coreNumbers);
	 	      }*/
	 	      mapTeste[j].primeiro = v;
	 	      mapTeste[j].segundo = w;
 		      conexoes_rede[i].push_back({mapTeste[j].primeiro, mapTeste[j].segundo});
 		   }
    	}
	}

	/*Pega os valores de saída da simulação e escolhe os progenitores*/
	void fitness(){
		/*Captura dos valores de saída da simulação*/
		string simulacao = "saida_simulacao.txt";
		int j;
		string aux;
		string aux2;
		int pai[5], aux_dead, aux_lat;
		int mae[5] = {100000, 100000, 100000, 100000, 100000};
		int sub1[5] = {0, 0, 0, 0, 0};
		int sub2[5];
		for(int i = 0; i < SIZE_POPULATION; i++){
			ifstream leitura;
			leitura.open(std::to_string(i)+simulacao, ios_base::in);
			if(leitura.is_open()){
				leitura.getline(linha, 15);
				deadline[i] = atof(linha);
				//leitura.getline(linha, 15);
				while(leitura.getline(linha, 15)){
					aux = "";
					aux2 = "";
					for(j = 0; j < 15; j++){
						if(linha[j] != ' '){
							aux = aux + linha[j];
						}else{
							break;
						}
					}
					latencia[i] = atof(aux.c_str());
					j = j + 1;
					for(; j < 15; j++){
						if(linha[j] != ' '){
							aux2 = aux2 + linha[j];
						}else{
							break;
						}
					}

					grau_latencia[i] = aux2;
				}
			}
			leitura.close();
			if((grau_latencia[i][0] == 'p') && (grau_latencia[i][1] == 's')){
				latencia[i] = latencia[i] / 1000;
			}
		}

		/*Seleciona os progenitores*/
		for(int c = 0; c < 5; c++){
			for(int k = 0; k < SIZE_POPULATION; k++){
				if((deadline[k] > deadline[k-1])){
					pai[c] = k;
				}
			}

			int m = pai[c] + 1;

			for(m; m < SIZE_POPULATION; m++){
				if((latencia[m] < latencia[m+1])){
					aux_lat = m;
					if(mae[c] > aux_lat){
						mae[c] = aux_lat;
					}
				}
			}	
		}

			

		for(int c = 0; c < 5; c++){
			for(int n = 0; n < SIZE_POPULATION; n++){
				if((n != pai[c]) && (n != mae[c]) && (deadline[n+1] < deadline[sub1[c]])){
					sub1[c] = n;
				}
			}

			int o = sub1[c] + 1;
			sub2[c] = sub1[c] + 1;

			for(o; o < SIZE_POPULATION; o++){
				if((o != pai[c]) && (o != mae[c]) && (o != sub1[c]) && (latencia[o+1] > latencia[sub2[c]])){
					sub2[c] = o;
				}
			}


			this->pai[c] = pai[c];
			this->mae[c] = mae[c];
			this->sub1[c] = sub1[c];
			this->sub2[c] = sub2[c];
		}
	}

	void crossover(){
		srand(time(NULL));
		Conexoes filho1[5][coreNumbers], filho2[5][coreNumbers], conexoes_aux[5][coreNumbers];
		
		for(int c = 0; c < 5; c++){
			for(int i = 0; i < coreNumbers; i++){
				conexoes_aux[c][i].primeiro = (rand()%2);
				conexoes_aux[c][i].segundo = (rand()%2);
			}

			for(int i = 0; i < coreNumbers; i++){
				filho1[c][i].primeiro = 0;
				filho1[c][i].segundo = 0;
				filho2[c][i].primeiro = 0;
				filho2[c][i].segundo = 0;
			}
		}

		/*Criação dos primeiros 5  filhos*/
		for(int c = 0; c < 5; c++){
			for(int i = 0; i < coreNumbers; i++){
				if(conexoes_aux[c][i].primeiro == 1){
					filho1[c][i].primeiro = conexoes_rede[pai[c]][i].primeiro;
				}
				if(conexoes_aux[c][i].segundo == 1){
					filho1[c][i].segundo = conexoes_rede[mae[c]][i].segundo;
				}
			}


			for(int i = 0; i < coreNumbers; i++){
				if(verifica(filho1[c], i, conexoes_rede[mae[c]][i].primeiro) == true){
					break;
				}else{
					filho1[c][i].primeiro = conexoes_rede[mae[c]][i].primeiro;
				}
				if(verifica2(filho1[c], i, conexoes_rede[pai[c]][i].segundo) == true){
					break;
				}else{
					filho1[c][i].segundo = conexoes_rede[pai[c]][i].segundo;
				}

			}	
		}

		


		/*Criação dos ultimos 5 filhos*/
		for(int c = 0; c < 5; c++){
			for(int i = 0; i < coreNumbers; i++){
				if(conexoes_aux[c][i].primeiro == 0){
					filho2[c][i].primeiro = conexoes_rede[mae[c]][i].primeiro;
				}
				if(conexoes_aux[c][i].segundo == 0){
					filho2[c][i].segundo = conexoes_rede[pai[c]][i].segundo;
				}
			}


			for(int i = 0; i < coreNumbers; i++){
				if(verifica(filho2[c], i, conexoes_rede[pai[c]][i].primeiro) == true){
					break;
				}else{
					filho2[c][i].primeiro = conexoes_rede[pai[c]][i].primeiro;
				}

				if(verifica2(filho2[c], i,conexoes_rede[mae[c]][i].segundo) == true){
					break;
				}else{
					filho2[c][i].segundo = conexoes_rede[mae[c]][i].segundo;
				}
			}	
		}
		


		/*Mutação*/
		int aux_mut;
		for(int c = 0; c < 5; c++){
			aux_mut = filho1[c][(coreNumbers-1)].primeiro;
			filho1[c][(coreNumbers-1)].primeiro = filho1[c][0].primeiro;
			filho1[c][0].primeiro = aux_mut;
		}
		



		/*Troca os filhos pelos indíviduos mais fracos da população*/
		for(int c = 0; c < 5; c++){
			for(int i = 0; i < coreNumbers; i++){
				conexoes_rede[sub1[c]][i] = filho1[c][i];
				conexoes_rede[sub2[c]][i] = filho2[c][i];
			}	
		}
	}

	genetico(){

	}
};