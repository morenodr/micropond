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
	
	uint round = 0;
	
	while(running){
		mutex->lock();
		round++;
		//Select random cell
		x = qrand() % WORLD_X;
		y = qrand() % WORLD_Y;
		z = qrand() % WORLD_Z;
		Simulation::executeCell(x,y,z);
		
		if(!world[x][y][z].generation){
			mutateCell(&world[x][y][z]);
		}
		
		//kills a cell if there is not energy left and it's a child
		if(!world[x][y][z].energy && world[x][y][z].generation){
			killCell(&world[x][y][z]);
		}
		
		//add energy every x rounds
		if(!(round % ENERGY_FREQUENCY)){
			regenerateEnergy();
		}
		mutex->unlock();
	}
}

void Simulation::killCell(struct Cell *cell){
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
	
	world[x][y][z].energy += ENERGY_ADDED;
}


/**
 * return true if a certain type of action is allowed from cell source to
 * cell dest.
 * The action can be specified with the parameter good
 * the chance of success can be better with the guess parameter
 */
bool Simulation::accessOk(struct Cell *source, struct Cell *dest, char guess,bool good){
	if(!dest->generation){
		return true;
	}
	
	if(dest->genome[0] == guess){
		return true;
	}else{
		if(dest->genome[0] == source->genome[0] && good){
			return true;
		}
	}
	
	return false;
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
	struct Cell *tmpCell; //temporary cell
	
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
			output_buffer[pointer] = reg;
			break;
		case 7: //read output buffer to register
			reg = output_buffer[pointer];
			break;
		case 8: //look into direction specified in the register
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
		case 11:{ //seek most energy
				uint max = 0;
				for(int i = 0; i < DIRECTIONS; i++){
					tmpCell = getNeighbour(x,y,z,i);
					if(max < tmpCell->energy){
						reg = i;
						max = tmpCell->energy;
					}
				}
			}
			break;
		case 12: //move
			tmpCell = getNeighbour(x,y,z,facing);
			struct Cell tmp;
			tmp = *tmpCell;
			*tmpCell = *cell;
			*cell = tmp;
			break;
		case 13: // kill
			tmpCell = getNeighbour(x,y,z,facing);
			if(accessOk(cell, tmpCell, reg,false)){
				killCell(tmpCell);
			}
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
		if(accessOk(cell, neighbour, reg,false)){
			reproduce(cell,neighbour);
		}
	}
}

/**
 * reproduce cell.
 * Mutations are introduced 
 */
void Simulation::reproduce(struct Cell *cell, struct Cell *neighbour){
	neighbour->id = ++cellid;
	if(cell->id){
		neighbour->parent = cell->id;
	}else{
		neighbour->parent = neighbour->id;
	}
	
	neighbour->generation = cell->generation + 1;
	
	if(!cell->lineage){
		neighbour->lineage = neighbour->id;
	}else{
		neighbour->lineage = cell->lineage;
	}
	int loop = 0;
	for(int i = 0; i < GENOME_SIZE && loop < GENOME_SIZE; i++){
		if(qrand() % MUTATION_RATE == 0){
			switch(qrand() % 2){
			case 0://command replacement
				neighbour->genome[i] = randomOperation();
				break;
			case 1://duplication or removal
				loop = qrand() % GENOME_SIZE;
				break;
			}
		}else{
			neighbour->genome[i] = cell->genome[loop];
		}
		loop++;
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

void Simulation::mutateCell(struct Cell *cell){	
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
