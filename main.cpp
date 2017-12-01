#include "systemc.h"
#include <iostream>
#include "router.h"
#include "parameters.h"
#include "genetico.h"


SC_MODULE(NoC){

	sc_in<bool> clk;

	int coreNumbers;

	trafego_rede trafego;

	conexoes conexoes_rede;

	router *rt[25];
	routing_table table[25];

	void chaveamento_externo(){
		for(int i = 0; i < coreNumbers; i++){
			if(conexoes_rede[i].primeiro == 1){
				rt[(conexoes_rede[i].primeiro-1)]->in_port[0] = rt[(conexoes_rede[i].segundo-1)]->out_port[2];
				rt[conexoes_rede[i].primeiro]->in_val[0].write(rt[(conexoes_rede[i].segundo-1)]->out_val[2]);
				rt[(conexoes_rede[i].segundo-1)]->in_port[2] = rt[(conexoes_rede[i].primeiro-1)]->out_port[0];
				rt[(conexoes_rede[i].segundo-1)]->in_val[2].write(rt[(conexoes_rede[i].primeiro-1)]->out_val[0]);
			}else if((conexoes_rede[i].primeiro % 2) == 1){
				rt[(conexoes_rede[i].primeiro-1)]->in_port[0] = rt[(conexoes_rede[i].segundo-1)]->out_port[2];
				rt[(conexoes_rede[i].primeiro-1)]->in_val[0].write(rt[(conexoes_rede[i].segundo-1)]->out_val[2]);
				rt[(conexoes_rede[i].segundo-1)]->in_port[2] = rt[(conexoes_rede[i].primeiro-1)]->out_port[0];
				rt[(conexoes_rede[i].segundo-1)]->in_val[2].write(rt[(conexoes_rede[i].primeiro-1)]->out_val[0]);
			}else if((conexoes_rede[i].primeiro % 2) == 0){
				rt[(conexoes_rede[i].primeiro-1)]->in_port[1] = rt[(conexoes_rede[i].segundo-1)]->out_port[3];
				rt[(conexoes_rede[i].primeiro-1)]->in_val[1].write(rt[(conexoes_rede[i].segundo-1)]->out_val[3]);
				rt[(conexoes_rede[i].segundo-1)]->in_port[3] = rt[(conexoes_rede[i].primeiro-1)]->out_port[1];
				rt[(conexoes_rede[i].segundo-1)]->in_val[3].write(rt[(conexoes_rede[i].primeiro-1)]->out_val[1]);
			}
		}
	}

	void map_rede(){
		for (int i = 0; i < coreNumbers; i++){
			rt[i]->tabela = table[i];
		}
	}

	void set_packets(){
		for(int i = 0; i < coreNumbers; i++){
			for(int j = 0; j < trafego.size(); j++){
				if(rt[i]->position == trafego[j].origem){
					rt[i]->in_val[4].write(1);
					for(int k = 0; k < trafego[j].pacotes; k++){
						for(int l = 0; l < 5; l++){
							if(l == 0){
								rt[i]->in_port[4].type = BEGIN_PACKET;
								rt[i]->in_port[4].payload = (8 + i);
								rt[i]->in_port[4].destiny = trafego[j].destino;
								wait(3);
								rt[i]->in_port[4].type = 0;
							}else if(l == 4){
								rt[i]->in_port[4].type = END_PACKET;
								rt[i]->in_port[4].payload = (8 + i);
								rt[i]->in_port[4].destiny = trafego[j].destino;
								wait(3);
								rt[i]->in_port[4].type = 0;
							}else{
								rt[i]->in_port[4].type = INSTRUCTION;
								rt[i]->in_port[4].payload = (8 + i);
								rt[i]->in_port[4].destiny = trafego[j].destino;
								wait(3);
								rt[i]->in_port[4].type = 0;
							}
						}
					}
				}
			}
		}
	}


	SC_CTOR(NoC){
		for(int i = 0; i < 25; i++){
			rt[i] = new router("rt");
			rt[i]->clk(clk);
		}

		SC_METHOD(chaveamento_externo);
		sensitive << clk.pos();
		SC_METHOD(map_rede);
		sensitive << clk.pos();
		SC_CTHREAD(set_packets, clk.pos());
	}
};

using namespace std;

int sc_main (int argc, char* argv[]){

	sc_clock clock("Clock", 10, SC_NS);

	NoC rede("NoC");
	rede.clk(clock);

	genetico gene;

	int coreNumbers;
	char linha[100];
	string temp0;
	string temp1;
	string temp2;
	string temp3;
	int temp00;
	int temp01;
	int temp02;
	int temp03;
	int count;


	trafego_rede trafego;

	float deadline_parcial, deadline;
	int total_packets;
	sc_time latencia_parcial, latencia_media;

	ifstream arquivoTrafego;
	ofstream saidaDados;


	//Instanciamento do arquivo de trafego
	arquivoTrafego.open("01.txt", ios_base::in);
	if (arquivoTrafego.is_open()){
		arquivoTrafego.getline(linha, 100);
		coreNumbers = atoi(linha);
		while(arquivoTrafego.getline(linha, 100)){
			temp0 = "";
			temp1 = "";
			temp2 = "";
			temp3 = "";
			for(count = 0; count < 100; count++){
				if(linha[count] != ' '){
					temp0 = temp0 + linha[count];
				}else{
					break;
				}
			}
			temp00 = atoi(temp0.c_str());
			count = count + 1;

			for(; count < 100; count ++){
				if(linha[count] != ' '){
					temp1 = temp1 + linha[count];
				}else{
					break;
				}
			}
			temp01 = atoi(temp1.c_str());
			count = count + 1;

			for(; count < 100; count ++){
				if(linha[count] != ' '){
					temp2 = temp2 + linha[count];
				}else{
					break;
				}
			}
			temp02 = atoi(temp2.c_str());
			count = count + 1;

			for(; count < 100; count ++){
				if(linha[count] != ' '){
					temp3 = temp3 + linha[count];
				}else{
					break;
				}
			}
			temp03 = atoi(temp3.c_str());

			trafego.push_back({temp00, temp01, temp02, temp03});
		}
	}

	arquivoTrafego.close();

	gene.coreNumbers = coreNumbers;

	gene.firstPopulation();

	rede.coreNumbers = coreNumbers;
	rede.trafego = trafego;

	for(int b = 0; b < 50; b++){
		for(int a = 0; a < SIZE_POPULATION; a++){
			cout << b << endl;
		rede.conexoes_rede = gene.conexoes_rede[a];
		//cout << gene.conexoes_rede[a].size() << endl;


			for(int i = 0; i < coreNumbers; i++){
				rede.rt[i]->position = i + 1;
			}

			for(int i = 0; i < coreNumbers; i++){
			rede.table[i].push_back({rede.rt[i]->position, LOCAL, 0});
				for(int j = 0; j < (coreNumbers - 1); j++){
					if(rede.rt[i]->position == 1){
						for(int k = 0; k < coreNumbers; k++){
							if(rede.rt[i]->position == gene.conexoes_rede[a][k].primeiro){
								rede.table[i].push_back({gene.conexoes_rede[a][k].segundo, NORTH, 0});
							}else if(rede.rt[i]->position == gene.conexoes_rede[a][k].segundo){
								if((gene.conexoes_rede[a][k].primeiro % 2 == 1)){
									rede.table[i].push_back({gene.conexoes_rede[a][k].primeiro, SOUTH, 0});
								}else if((gene.conexoes_rede[a][k].primeiro % 2 == 0)){
									rede.table[i].push_back({gene.conexoes_rede[0][k].primeiro, WEST, 0});
								}
							}else{
								rede.table[i].push_back({gene.conexoes_rede[a][k].primeiro, NORTH, 0});
							}
						}
					}else if((rede.rt[i]->position % 2) == 1){
						for(int k = 0; k < coreNumbers; k++){
							if(rede.rt[i]->position == gene.conexoes_rede[a][k].primeiro){
								rede.table[i].push_back({gene.conexoes_rede[a][k].segundo, NORTH, 0});
							}else if(rede.rt[i]->position == gene.conexoes_rede[a][k].segundo){
								if((gene.conexoes_rede[a][k].primeiro % 2 == 1)){
									rede.table[i].push_back({gene.conexoes_rede[a][k].primeiro, SOUTH, 0});
								}else if((gene.conexoes_rede[a][k].primeiro % 2 == 0)){
									rede.table[i].push_back({gene.conexoes_rede[a][k].primeiro, WEST, 0});
								}
							}else{
								rede.table[i].push_back({gene.conexoes_rede[a][k].primeiro, NORTH, 0});
							}
						}
					}else if((rede.rt[i]->position % 2) == 0){
						for(int k = 0; k < coreNumbers; k++){
							if(rede.rt[i]->position == gene.conexoes_rede[a][k].primeiro){
								rede.table[i].push_back({gene.conexoes_rede[a][k].segundo, EAST, 0});
							}else if(rede.rt[i]->position == gene.conexoes_rede[a][k].segundo){
								if((gene.conexoes_rede[a][k].primeiro % 2 == 1)){
									rede.table[i].push_back({gene.conexoes_rede[0][k].primeiro, SOUTH, 0});
								}else if((gene.conexoes_rede[a][k].primeiro % 2 == 0)){
									rede.table[i].push_back({gene.conexoes_rede[a][k].primeiro, WEST, 0});
								}
							}else{
								rede.table[i].push_back({gene.conexoes_rede[a][k].primeiro, EAST, 0});
							}
						}
					}	
				}
				if(rede.table[i].size() > (coreNumbers+1)){
					for(int m = rede.table[i].size(); m > (coreNumbers+1); m--){
						rede.table[i].pop_back();
					}
				}
			}


		sc_start(trafego[0].deadline, SC_NS);


		//Calculo dos pacotes que chegaram ao destino
		for(int i = 0; i < trafego.size(); i++){
			total_packets = total_packets + trafego[i].pacotes;
		}

		for(int i = 0; i < coreNumbers; i++){
			deadline_parcial = deadline_parcial + rede.rt[i]-> count;
		}
		
		for(int i = 0; i < coreNumbers; i++){
			latencia_parcial = latencia_parcial + rede.rt[i]->latencia_rt;
		}

		string simulacao = "saida_simulacao.txt";

		deadline = (deadline_parcial * 100) / total_packets;
		latencia_media = (latencia_parcial / total_packets);
		if(deadline > 100){
			deadline = 100;
		}

		saidaDados.open(std::to_string(a)+simulacao);
		saidaDados << deadline << endl;
		saidaDados << latencia_media;
		saidaDados.close();

		}

		gene.fitness();

		gene.crossover();


	}

	for(int i = 0; i < SIZE_POPULATION; i++){
		saidaDados.open(std::to_string(i)+conex);
		for(int j = 0; j < coreNumbers; j++){
			saidaDados << gene.conexoes_rede[i][j].primeiro << endl;
		}
		saidaDados.close();
	}

}