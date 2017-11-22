#ifndef _BUFFER_H_
#define _BUFFER_H_ 

#include <iostream>
#include "systemc.h"
#include "parameters.h"

using namespace std;


SC_MODULE (Buffer){

	
	
	fila_flits flits;

	flit din; //Entrada data + bop + eop
	flit dout; //Saída que vai para o roteamento

	sc_in<bool> clk;

	sc_signal<sc_int<32>, SC_MANY_WRITERS> wr; //Entrada que vem do controle de fluxo
	sc_signal<sc_int<32> > wok; //Saída que volta para o controle de fluxo
	sc_signal<sc_int<32>, SC_MANY_WRITERS> rd; //Entrada que vem do chaveamento
	sc_signal<sc_int<32> > rok; //Saída do chaveamento
	
	sc_int<32> length;


	void add(){
		//cout << wr << endl;
		if ((wr.read() == 1) && (flits.size() == length)){
			wok.write(0); // error, value not added
		}else if((wr.read() == 1) && (flits.size() < length)){	
			flits.push(din);
			wok.write(1);
		}	
	}

	void remove(){
		if (flits.size() < 1){
			rok.write(0); // queue is empty. Nothing to remove
		} else {
			rok.write(1);
			dout = flits.front();
			flits.pop();
		}
	}

	//Se estiver Vazio retorna 1, senão retorna 0
	int isEmpty(){
		return (flits.empty() == 1) ? 1: 0;
	}


	SC_CTOR(Buffer) {
		length = 50;
        SC_METHOD(add);
        sensitive << clk.pos();
       	SC_METHOD(remove);
        sensitive << clk.pos();
    }

	
};
#endif