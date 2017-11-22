#include "systemc.h"
#include "parameters.h"
#include <iostream>

using namespace std;

SC_MODULE(arbiter){
	sc_in<bool> clk;
	sc_signal<sc_int<32> > rd[5];
	sc_int<32> priority;
	sc_int<32> bufferCircular[5];
	sc_int<32> pointer;

	//Verifica em qual buffer se encontra a prioridade no momento
	void checkPriority(){			
		if(this->pointer < 5){
			this->pointer++;
		}else{
			this->pointer = 1;
		}
		if ((bufferCircular[0] == 1)  && (this->pointer == NORTH)){
			priority = NORTH;
			rd[0].write(1);
		}
		if ((bufferCircular[1] == 1)  && (this->pointer == NORTH)){
			priority = EAST;
			rd[1].write(1);
		}
		if ((bufferCircular[2] == 1)  && (this->pointer == NORTH)){
			priority = SOUTH;
			rd[2].write(1);
		}
		if ((bufferCircular[3] == 1)  && (this->pointer == NORTH)){
			priority = WEST;
			rd[3].write(1);
		}
		if ((bufferCircular[4] == 1)  && (this->pointer == NORTH)){
			priority = LOCAL;
			rd[4].write(1);
		}
		//cout << this->pointer << endl;
	}


	SC_CTOR(arbiter){
		pointer = 0;
		SC_METHOD(checkPriority);
		sensitive << clk.pos();
	}
};