#ifndef SIMULATION_H_
#define SIMULATION_H_

#include <QtCore>

#define WORLD_X 500
#define WORLD_Y 500
#define WORLD_Z 1
#define GENOME_SIZE 100

#define GENOME_OPERATIONS 16

#define DIRECTIONS 6

#define NORTH 0
#define WEST 1
#define SOUTH 2
#define EAST 3
#define UP 4
#define DOWN 5

struct Cell{
	uint id;
	uint parent;
	uint generation;
	uint energy;
	uint lineage;
	uchar genome[GENOME_SIZE];
};

class Simulation
{
public:
	Simulation();
	virtual ~Simulation();
	void run();
private:
	struct Cell world[WORLD_X][WORLD_Y][WORLD_Z];
	uint cellid;
	void init();
	uchar randomOperation();
	void executeCell(int x, int y, int z);
	struct Cell *getNeighbour(int x, int y, int z, uchar direction);
};

#endif /*SIMULATION_H_*/
