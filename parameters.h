#ifndef PARAMETERS_H_INCLUDED
#define PARAMETERS_H_INCLUDED
#include <vector>
#include <queue>

using namespace std;

#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4
#define LOCAL 5

#define FREE 0
#define SENDING 1
#define WAITING 1

#define ERROR 5

#define BEGIN_PACKET 1
#define INSTRUCTION 2
#define END_PACKET 3
#define SIZE_FLIT_PACKET 5

#define SIZE_POPULATION 50

struct table {
	int destiny;
	int port;
	int hops;
};

typedef vector <table> routing_table;

struct flits {
	int type;
	int payload;
	int destiny;
};

typedef flits flit;

typedef queue<flit> fila_flits;

struct packet {
	flit flit_packet[SIZE_FLIT_PACKET];
	int destiny;
};

struct trafegoRede {
	int origem;
	int destino;
	int pacotes;
	int deadline;
};

typedef vector <trafegoRede> trafego_rede; 

struct Conexoes{
	int primeiro;
	int segundo;
};

typedef vector <Conexoes> conexoes;


#endif 
