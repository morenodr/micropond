#ifndef SIMULATION_H_
#define SIMULATION_H_

#include <QtCore>

#define VARIED_ENERGY //does not give the whole playing field the same energy
//#define OLDSTYLE_MUTATION   //older and slower way to mutate
//#define DECREASE_ENERGY //decreases energy over time
#define DEAD_MUTATION //mutates dead cells
//#define EXECUTION_ERRORS //creates random execution errors
#define REPRODUCTION_ERRORS //mutates genome when reproducing
#define BAD_KILLS

#define WORLD_X 800
#define WORLD_Y 600
#define WORLD_Z 1
#define GENOME_SIZE 100 //number of operations in a genome

#define GENOME_OPERATIONS 33 //number of different operations
#define NO_REP_OPERATION 11 //id of the NO REPRODUCE operation

#define EAT_ENERGY GENOME_SIZE //amount of energy gained from eating

#define LIVING 3 //minimum generation to be considered alive

#define MAX_EXECUTION_ROW 5 //how many cells can be executed in a row

#define DIRECTIONS 4 //change if you want 3d

#define NORTH 0
#define WEST 1
#define SOUTH 2
#define EAST 3
#define UP 4
#define DOWN 5

#define MUTATION_RATE_REPRODUCTION 7000
#define MUTATION_RATE_EXECUTION 100000
#define MUTATION_RATE_NON_LIVING 200
//#define MAX_MUTATIONS_NON_LIVING 3

#define ENERGY_ADDED 5000
#define ENERGY_FREQUENCY 25

#define ENERGY_DECREASE 5000000

#define ENERGY2_CONVERSION_GAIN 12

#define MIN_COPY 5

#define DISASTER_CHANCE 6000000

#define LANDSCAPE_LINES 6

#define MAX_EXECUTING 4000

#define SPECIAL_COMMANDS 5 //number of allowed special commands like kill, move

struct Cell{
	uint genome_size;
	qlonglong id;
	qlonglong parent;
	uint generation;
	uint energy;
	uint energy2;
	qlonglong lineage;
	uchar genome[GENOME_SIZE+1];
	uchar reproduced;
	uint bad;
	uint brain;
	struct Place *place;
	uint size;
	uchar facing;
}; 

struct Place{ 
	bool dead; //can we reproduce at this place?
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
	Simulation(int id);
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
	void setEnergyAdd(uint value){ energyAdd = value;};
	uint getEnergyAdd(){return energyAdd;};
	uint getMaxEnergyAdd(){ return ENERGY_ADDED;};
	
	void saveWorld(QString file);
	void loadWorld(QString file);
	int id(){return myId;}
	int executed();
	
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
	inline uchar randomOperation();
	void executeCell(int x, int y, int z);
	void mutateCell(struct Cell *cell);
	void killCell(struct Cell *cell);
	bool reproduce(struct Cell *cell, struct Cell *neighbour,uchar *output_buffer);
	
	bool accessOk(struct Cell *source, struct Cell *dest, char guess,bool good);
	struct Position getNeighbour(int x, int y, int z, uchar direction);
	void disaster();
	
	int nextx;
	int nexty;
	int nextz;
	bool nextSet;
	int canExecuteNext;
	
	unsigned long mutated;
	bool initialized;
	int myId;
};

#endif /*SIMULATION_H_*/
