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
	int pai, mae, sub1, sub2;
	conexoes filho1, filho2;

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
	    //srand(time(NULL));
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
		int pai, aux_dead, aux_lat;
		int mae = 100000;
		int sub1 = 0;
		int sub2;
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
		for(int k = 0; k < SIZE_POPULATION; k++){
			if((deadline[k] > deadline[k-1])){
				pai = k;
			}
		}

		int m = pai + 1;

		for(m; m < SIZE_POPULATION; m++){
			if((latencia[m] < latencia[m+1])){
				aux_lat = m;
				if(mae > aux_lat){
					mae = aux_lat;
				}
			}
		}

		for(int n = 0; n < SIZE_POPULATION; n++){
			if((n != pai) && (n != mae) && (deadline[n+1] < deadline[sub1])){
				sub1 = n;
			}
		}

		int o = sub1 + 1;
		sub2 = sub1 + 1;

		for(o; o < SIZE_POPULATION; o++){
			if((o != pai) && (o != mae) && (o != sub1) && (latencia[o+1] > latencia[sub2])){
				sub2 = o;
			}
		}


		this->pai = pai;
		this->mae = mae;
		this->sub1 = sub1;
		this->sub2 = sub2;
	}

	void crossover(){
		srand(time(NULL));
		Conexoes filho1[coreNumbers], filho2[coreNumbers], conexoes_aux[coreNumbers];
		for(int i = 0; i < coreNumbers; i++){
			conexoes_aux[i].primeiro = (rand()%2);
			conexoes_aux[i].segundo = (rand()%2);
		}

		for(int i = 0; i < coreNumbers; i++){
			filho1[i].primeiro = 0;
			filho1[i].segundo = 0;
			filho2[i].primeiro = 0;
			filho2[i].segundo = 0;
		}

		/*Criação do primeiro filho*/
		for(int i = 0; i < coreNumbers; i++){
			if(conexoes_aux[i].primeiro == 1){
				filho1[i].primeiro = conexoes_rede[pai][i].primeiro;
			}
			if(conexoes_aux[i].segundo == 1){
				filho1[i].segundo = conexoes_rede[mae][i].segundo;
			}
		}


		for(int i = 0; i < coreNumbers; i++){
			if(verifica(filho1, i, conexoes_rede[mae][i].primeiro) == true){
				break;
			}else{
				filho1[i].primeiro = conexoes_rede[mae][i].primeiro;
			}
			if(verifica2(filho1, i, conexoes_rede[pai][i].segundo) == true){
				break;
			}else{
				filho1[i].segundo = conexoes_rede[pai][i].segundo;
			}

		}


		/*Criação do segundo filho*/
		for(int i = 0; i < coreNumbers; i++){
			if(conexoes_aux[i].primeiro == 0){
				filho2[i].primeiro = conexoes_rede[mae][i].primeiro;
			}
			if(conexoes_aux[i].segundo == 0){
				filho2[i].segundo = conexoes_rede[pai][i].segundo;
			}
		}


		for(int i = 0; i < coreNumbers; i++){
			if(verifica(filho2, i, conexoes_rede[pai][i].primeiro) == true){
				break;
			}else{
				filho2[i].primeiro = conexoes_rede[pai][i].primeiro;
			}

			if(verifica2(filho2, i,conexoes_rede[mae][i].segundo) == true){
				break;
			}else{
				filho2[i].segundo = conexoes_rede[mae][i].segundo;
			}
		}


		/*Mutação*/
		int aux_mut;
		aux_mut = filho1[(coreNumbers-1)].primeiro;
		filho1[(coreNumbers-1)].primeiro = filho1[0].primeiro;
		filho1[0].primeiro = aux_mut;

		aux_mut = filho2[(coreNumbers-1)].primeiro;
		filho2[(coreNumbers-1)].primeiro = filho2[0].primeiro;
		filho2[0].primeiro = aux_mut;


		/*Troca os filhos pelos indíviduos mais fracos da população*/
		/*for(int i = 0; i < coreNumbers; i++){
			cout << filho2[i].primeiro << " " << filho2[i].segundo << endl;
			this->filho1.push_back({filho1[i].primeiro, filho1[i].segundo});
			this->filho2.push_back({filho2[i].primeiro, filho2[i].segundo});
		}*/
		for(int i = 0; i < coreNumbers; i++){
			conexoes_rede[sub1][i] = filho1[i];
			conexoes_rede[sub2][i] = filho2[i];
		}


	}

	genetico(){

	}
};