#include "Simulation.h"

Simulation::Simulation()
{
	cellid = 0;
	running = true;
	
	mutex = new QMutex();
}

Simulation::~Simulation()
{
}

void Simulation::pause(){
	mutex->lock();
}

void Simulation::resume(){
	mutex->unlock();
}

void Simulation::run(){
	qsrand(100);
	
	init();
	
	int x,y,z;
	
	while(running){
		mutex->lock();
		x = qrand() % WORLD_X;
		y = qrand() % WORLD_Y;
		z = qrand() % WORLD_Z;
		Simulation::executeCell(x,y,z);
		
		if(!world[x][y][z].generation){
			mutateCell(x,y,z);
		}
		
		if(!world[x][y][z].energy){
			killCell(x,y,z);
		}
		
		regenerateEnergy();
		mutex->unlock();
	}	
}

void Simulation::killCell(int x, int y, int z){
	struct Cell *cell;
		
	cell = &world[x][y][z];
	cell->parent = 0;
	cell->lineage = 0;
	cell->generation = 0;
	cell->energy = 0;
	cell->id = 0;
	
	for(int i = 0; i < GENOME_SIZE; i++){
		cell->genome[i] = randomOperation();
	}
}

void Simulation::regenerateEnergy(){
	int x = qrand() % WORLD_X;
	int y = qrand() % WORLD_Y;
	int z = qrand() % WORLD_Z;
	
	world[x][y][z].energy += 20;
}

/**
 * Executes this cell
 */
void Simulation::executeCell(int x, int y, int z){
	struct Cell *cell = &world[x][y][z]; //current cell
	uchar inst; //current instruction
	int genome_pointer = 0; //pointer to the current genome instruction
	int output_pointer = 0; //pointer to the outputbuffer
	uchar output_buffer[GENOME_SIZE]; //outputbuffer, needed for reproducing
	bool stop = false;
	
	for(genome_pointer = 0; genome_pointer < GENOME_SIZE; genome_pointer++){
		output_buffer[genome_pointer] = GENOME_OPERATIONS;
	}
	
	genome_pointer = 0;
	int pointer = 0;//general pointer
	uchar facing = WEST;
	int reg = 0; //internal register to be used for anything
	
	//Execute cell until no more energy is left
	while(cell->energy && !stop){
		genome_pointer++;
		
		if(genome_pointer > GENOME_SIZE-1){
			genome_pointer = 1;
		}
		
		inst = cell->genome[genome_pointer];
		cell->energy--;
		
		switch(inst){
		case 0:
			pointer = 0;
			reg = 0;
			facing = WEST;
			break;
		case 1: //pointer --
			pointer++;
			if(pointer > GENOME_SIZE - 1){
				pointer = 1;
			}
			break;
		case 2: //pointer ++
			pointer--;
			if(pointer <= 1){
				pointer = GENOME_SIZE - 1;
			}
			break;
		case 3: //register ++
			reg++;
			if(reg > GENOME_SIZE -1 ){
				reg = 0;
			}
			break;
		case 4: //register --
			reg--;
			if(reg < 0 ){
				reg = GENOME_SIZE - 1;
			}
			break;
		case 5: //read genome to register
			reg = cell->genome[pointer];
			break;
		case 6: //write register to outputbuffer
			output_buffer[pointer] = (uchar)reg ;
			break;
		case 7: //read output buffer to register
			reg = output_buffer[pointer];
			break;
		case 8:
			facing = reg % DIRECTIONS;
			break;
		case 9://while(register){
			if(reg){
				for(; genome_pointer < GENOME_SIZE; genome_pointer++){
					if(cell->genome[genome_pointer] == 10){
						break;
					}
				}
			}
			break;
		case 10://}
			if(reg){
				for(; genome_pointer >= 0; genome_pointer--){
					if(cell->genome[genome_pointer] == 9){
						break;
					}
				}
			}
			break;
		case 11:
			break;
		case 12:
			break;
		case 13:
			break;
		case 14://nop
			break;
		case 15: //end
			stop = true;
			break;
		}
	}
	
	output_pointer = 0;
	
	//jeah, we can reproduce something
	if(output_buffer[output_pointer] != GENOME_OPERATIONS){
		
		struct Cell *neighbour = getNeighbour(x,y,z,facing);
		neighbour->id = ++cellid;
		if(cell->id){
			neighbour->parent = cell->id;
		}else{
			neighbour->parent = neighbour->id;
		}
		
		neighbour->generation = cell->generation + 1;
		
		if(!cell->lineage){
			neighbour->lineage = cell->lineage;
		}else{
			neighbour->lineage = neighbour->id;
		}
		
		for(; output_pointer < GENOME_SIZE; output_pointer++){
			neighbour->genome[output_pointer] = cell->genome[output_pointer];
		}
	}
}

/**
 * Initialize each cell
 */
void Simulation::init(){
	int x = 0;
	int y = 0;
	int z = 0;
	int i = 0;
	struct Cell *cell;
	
	for(x = 0; x < WORLD_X; x++){
		for(y = 0; y < WORLD_Y; y++){
			for(z = 0; z < WORLD_Z; z++){
				cell = &world[x][y][z];
				cell->parent = 0;
				cell->lineage = 0;
				cell->generation = 0;
				cell->energy = 0;
				cell->id = 0;
				
				for(i = 0; i < GENOME_SIZE; i++){
					cell->genome[i] = randomOperation();
				}
			}
		}
	}
}

void Simulation::mutateCell(int x, int y, int z){
	struct Cell *cell = &world[x][y][z];
	
	for(int i = 0; i < GENOME_SIZE; i++){
		if(qrand() % MUTATION_RATE == 0){
			cell->genome[i] = randomOperation();
		}
	}
}

/**
 * returns a random operation
 */
uchar Simulation::randomOperation(){
	uchar temp = ((uchar)qrand()) % GENOME_OPERATIONS;
	return temp;
}

struct Cell *Simulation::cell(int x, int y, int z){
	return &world[x][y][z];
}

struct Cell *Simulation::getNeighbour(int x, int y, int z, uchar direction){
	
	switch(direction){
	case NORTH:
		y--;
		break;
	case SOUTH:
		y++;
		break;
	case WEST:
		x--;
		break;
	case EAST:
		x++;
		break;
	case UP:
		z++;
		break;
	case DOWN:
		z--;
		break;
	}
	
	if(x >= WORLD_X){
		x = x - WORLD_X;
	}else if(x < 0){
		x = WORLD_X + x;
	}
	
	if(y >= WORLD_Y){
		y = y - WORLD_Y;
	}else if(x < 0){
		y = WORLD_Y + y;
	}
	
	if(z >= WORLD_Z){
		z = z - WORLD_Z;
	}else if(x < 0){
		z = WORLD_Z + z;
	}
	
	return &world[x][y][z];
}
