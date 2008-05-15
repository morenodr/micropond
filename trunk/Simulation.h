#ifndef SIMULATION_H_
#define SIMULATION_H_

#include <QtCore>

#define WORLD_X 640
#define WORLD_Y 480
#define WORLD_Z 1
#define GENOME_SIZE 100

#define GENOME_OPERATIONS 24

#define LIVING 1 //minimum generation to be considered alive

#define DIRECTIONS 4 //change if you want 3d

#define NORTH 0
#define WEST 1
#define SOUTH 2
#define EAST 3
#define UP 4
#define DOWN 5

#define MUTATION_RATE_REPRODUCTION 40000
#define MUTATION_RATE_EXECUTION 90000
#define MUTATION_RATE_NON_LIVING 5000

#define ENERGY_ADDED 4000
#define ENERGY_FREQUENCY 50

#define ENERGY_DECREASE 5000000

#define ACCESS_CHANCE 15

struct Cell{
	uint genome_size;
	unsigned long long id;
	unsigned long long parent;
	uint generation;
	uint energy;
	unsigned long long lineage;
	uchar genome[GENOME_SIZE+1];
	bool activated;
	uchar reproduced;
};

struct Place{
	bool reproducable; //can we reproduce at this place?
};

struct Position{
	int x;
	int y;
	int z;
};

class Simulation: public QThread
{
	Q_OBJECT
public:
	Simulation();
	virtual ~Simulation();
	void run();
	void stopIt(){running = false;};
	
	int x(){ return WORLD_X;}
	int y(){ return WORLD_Y;}
	int z(){ return WORLD_Z;}
	
	struct Cell *cell(int x, int y, int z);
	
	int genomeSize(){ return GENOME_SIZE;}
	
	void regenerateEnergy();
	void pause();
	void resume();
	uint counter(){return count;};
private:
	struct Cell cells[WORLD_X][WORLD_Y][WORLD_Z];
	struct Place world[WORLD_X][WORLD_Y][WORLD_Z];
	unsigned long long cellid; //used to track new cells
	bool running; //stop exection when false
	QSemaphore *mutex;
	unsigned long long round; //total number of rounds
	uint count; //stat value
	
	uint energyAdd;
	
	void init();
	uchar randomOperation();
	void executeCell(int x, int y, int z);
	void mutateCell(struct Cell *cell);
	void killCell(struct Cell *cell);
	void reproduce(struct Cell *cell, struct Cell *neighbour,uchar *output_buffer);
	
	bool accessOk(struct Cell *source, struct Cell *dest, char guess,bool good);
	struct Position getNeighbour(int x, int y, int z, uchar direction);
};

#endif /*SIMULATION_H_*/
