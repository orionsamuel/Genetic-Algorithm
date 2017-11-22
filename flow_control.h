#include "systemc.h"
#include <iostream>

using namespace std;

SC_MODULE(flow_control){
	sc_in<bool> clk;
	sc_signal<sc_int<32> > in_val; //Entrada pra ver se no buffer terá espaço 
	sc_signal<sc_int<32> > out_ack; //Retorno da solicitação se terá espaço

	
	void request(){
		
	}
	void response(){
		
	}

	SC_CTOR(flow_control){
		SC_METHOD(request);
		sensitive << clk.pos();
		SC_METHOD(response);
		sensitive << clk.pos();
	}
	
};
