#include "Simulation.h"
#include <cstring>

Simulation::Simulation()
{
	cellid = 0;
	running = true;
	
	mutex = new QSemaphore(1);
	count = 0;
	energyAdd = ENERGY_ADDED;
}

Simulation::~Simulation()
{
}

void Simulation::pause(){
	mutex->acquire(1);
}

void Simulation::resume(){
	count = 0;
	mutex->release(1);
}

/*
 * Main process
 * is a loop executing the main code of the program, till the user
 * sends through the graphical interface a stop command which sets the
 * running variable to false
 */
void Simulation::run(){
	mutex->acquire(1);
	qsrand(time(NULL));
	
	init();
	
	int x,y,z;
	
	round = 0;
	mutex->release(1);
	while(running){
	    //mutex->acquire(1);
		round++;
		count++;
		//Select random cell
		x = qrand() % WORLD_X;
		y = qrand() % WORLD_Y;
		z = qrand() % WORLD_Z;
		
		//if there is one make the cell mutate
		if(!cells[x][y][z].generation){
			mutateCell(&cells[x][y][z]);
		}
		
		//kills a cell if there is not energy left and it's a child
		if(!cells[x][y][z].energy && cells[x][y][z].generation >= LIVING){
			killCell(&cells[x][y][z]);
		}else{
			//call the execution of its code
			Simulation::executeCell(x,y,z);
		}
		
		if(round % ENERGY_DECREASE == 0){
			qDebug() << "Decrease energy";
			energyAdd -= 100;
			if(energyAdd < ENERGY_ADDED / 5){
				energyAdd = ENERGY_ADDED;
				qDebug() << "Energy restored";
			}
		}

		//add energy every x rounds
		if(!(round % ENERGY_FREQUENCY)){
			regenerateEnergy();
		}
		
		//mutex->release(1);
	}
}

void Simulation::killCell(struct Cell *cell){
	cell->parent = 0;
	cell->lineage = 0;
	cell->generation = 0;
	cell->id = 0;
	cell->genome_size = GENOME_SIZE;
	cell->activated = false;
	cell->reproduced = 0;

	int randStuff = GENOME_SIZE / 5;
	for(int i = 0; i < randStuff; i++){
		cell->genome[i] = randomOperation();
	}
	
	memset(&cell->genome[randStuff],
			GENOME_OPERATIONS - 1,
			(GENOME_SIZE - randStuff) * sizeof(uchar));
	
	cell->genome[GENOME_SIZE] = 0x00;
}

/**
 * regenarates the energy of one cell
 */
void Simulation::regenerateEnergy(){
	int x = qrand() % WORLD_X;
	int y = qrand() % WORLD_Y;
	int z = qrand() % WORLD_Z;
	
	cells[x][y][z].energy += energyAdd;
}


/**
 * return true if a certain type of action is allowed from cell source to
 * cell dest.
 * The action can be specified with the parameter good
 * the chance of success can be better with the guess parameter
 */
bool Simulation::accessOk(struct Cell *source, struct Cell *dest, char guess,bool good){
	if(dest->generation <= 2){
		return true;
	}
	
	if(dest->generation < LIVING && qrand() % 3 == 0){
		return true;
	}
	
	if(dest->genome[0] == guess){
		return true;
	}else{
		if(dest->genome[0] == source->genome[0] && good){
			return true;
		}
	}
	
	return qrand() % ACCESS_CHANCE == 0;
}

/**
 * Executes this cell
 */
void Simulation::executeCell(int x, int y, int z){
	struct Cell *cell = &cells[x][y][z]; //current cell
	uchar inst; //current instruction
	int genome_pointer = 0; //pointer to the current genome instruction
	int output_pointer = 0; //pointer to the outputbuffer
	uchar output_buffer[GENOME_SIZE]; //outputbuffer, needed for reproducing
	bool stop = false;
	struct Cell *tmpCell; //temporary cell
	
	memset(output_buffer, 22, GENOME_SIZE * sizeof(uchar));
	
	genome_pointer = 0;
	int pointer = 0;//general pointer
	uchar facing = WEST;
	int reg = 0; //internal register to be used for anything
	int temp = 0; //temp register
	
	//Execute cell until no more energy is left
	while(cell->energy && !stop){
		
		inst = cell->genome[genome_pointer];
		genome_pointer++;
		if(genome_pointer > GENOME_SIZE-1){
			genome_pointer = 0;
		}
		cell->energy--;
		
		//execution perturbation
		if(qrand() % MUTATION_RATE_EXECUTION == 0){
			switch(qrand() % 3){
			case 0:
				inst = randomOperation();
				break;
			case 1:
				reg = randomOperation();
				break;
			case 2:
				pointer = qrand() % GENOME_SIZE;
				break;
			}
		}
		
		switch(inst){
		case 0:
			pointer = 0;
			reg = 0;
			temp = 0;
			facing = WEST;
			break;
		case 1: //pointer ++
			pointer++;
			if(pointer > GENOME_SIZE - 1){
				pointer = 0;
			}
			break;
		case 2: //pointer --
			pointer--;
			if(pointer <= 0){
				pointer = GENOME_SIZE - 1;
			}
			break;
		case 3: //register ++
			reg++;
			if(reg > GENOME_OPERATIONS -1 ){
				reg = 0;
			}
			break;
		case 4: //register --
			reg--;
			if(reg < 0 ){
				reg = GENOME_OPERATIONS - 1;
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
			if(!reg){
				int tempP = genome_pointer;
				while(cell->genome[genome_pointer] != 10 && cell->energy){
					genome_pointer= (genome_pointer+1)%GENOME_SIZE;
					
					cell->energy--;
					if(genome_pointer == tempP){
						stop = true;
						break;
					}
				}
				
				genome_pointer = (genome_pointer+1) % GENOME_SIZE;
			}
			break;
		case 10://}
			if(reg){
				int tempP = genome_pointer;
				while(cell->genome[genome_pointer] != 9 && cell->energy){
					genome_pointer--;
					if(genome_pointer< 0){
						genome_pointer = GENOME_SIZE-1;
					}
					cell->energy--;
					
					if(genome_pointer == tempP){
						stop = true;
						break;
					}
				}
				genome_pointer = (genome_pointer+1) % GENOME_SIZE;
			}
			break;
		case 11:{ //seek most energy
				uint max = 0;
				for(int i = 0; i < DIRECTIONS; i++){
					struct Position pos = getNeighbour(x,y,z,facing);
					tmpCell = &cells[pos.x][pos.y][pos.z];
					if(max < tmpCell->energy){
						reg = i;
						max = tmpCell->energy;
					}
				}
			}
			break;
		case 12:{ //move
			struct Position pos = getNeighbour(x,y,z,facing);
			tmpCell = &cells[pos.x][pos.y][pos.z];
			
			if(tmpCell != cell){
				struct Cell tmp;
				tmp = *tmpCell;
				*tmpCell = *cell;
				*cell = tmp;
			}
			stop = true;
		}break;
		case 13:{ // kill
			struct Position pos = getNeighbour(x,y,z,facing);
			tmpCell = &cells[pos.x][pos.y][pos.z];
			if(cell->generation >= LIVING  && accessOk(cell, tmpCell, reg,false)){
				killCell(tmpCell);
			}
		}break;
		case 14://nop
			break;
		case 15:{//share
			struct Position pos = getNeighbour(x,y,z,facing);
			tmpCell = &cells[pos.x][pos.y][pos.z];
			if(accessOk(cell, tmpCell, reg,true)){
				uint tmpEnergy = tmpCell->energy + cell->energy;
				tmpCell->energy = tmpEnergy / 2;
				cell->energy = tmpEnergy / 2;
			}
		}break;
		case 16:{//swap temp
			int t = temp;
			temp = reg;
			reg = t;
			}
			break;
		case 17://reset registers
			reg = 0;
			temp = 0;
			break;
		case 18:{//neigbour type
			struct Position pos = getNeighbour(x,y,z,facing);
			tmpCell = &cells[pos.x][pos.y][pos.z];
			if(!tmpCell->generation){
				reg = 0; //registry is 0 if neighbour is very young
			}else if(tmpCell->genome[0] == cell->genome[0]){
				reg = 1; //1 if same logo, friend
			}else{
				reg = 2; //2 if enemy
			}
		}break;
		case 19://activate signal
			cell->activated = !cell->activated;
			break;
		case 20:{
			struct Position pos = getNeighbour(x,y,z,facing);
			tmpCell = &cells[pos.x][pos.y][pos.z];
			if(accessOk(cell, tmpCell, reg,true)){
				if(tmpCell->activated){
					reg = 2;
				}else{
					reg = 1;
				}
			}
			reg = 0;
		}break;
		case 21:
			if(temp == reg){
				reg = 1;
			}else{
				reg = 0;
			}
			break;
		case 22: //NOP2, stops reproduction
			if(temp == reg){
				reg = 1;
			}else{
				reg = 0;
			}
			break;
		case 23: //end
			stop = true;
			break;
		}
	}
	
	output_pointer = 0;
	
	//jeah, we can reproduce something
	if(output_buffer[output_pointer] != 22){
		cell->reproduced = 0;
		if(world[x][y][z].reproducable){
			struct Position pos = getNeighbour(x,y,z,facing);
			struct Cell *neighbour = &cells[pos.x][pos.y][pos.z];
			if(accessOk(cell, neighbour, reg,false)){
				reproduce(cell,neighbour,output_buffer);
			}
		}
	}else if(cell->generation >= LIVING && cell->reproduced > 5){
		killCell(cell);
	}else{
		cell->reproduced++;
	}
}

/**
 * reproduce cell.
 * Mutations are introduced 
 */
void Simulation::reproduce(struct Cell *cell, struct Cell *neighbour,uchar *output_buffer){
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
	
	cell->activated = false;
	
	int loop = 0;
	int i = 0;
	for(i = 0; i < GENOME_SIZE && loop < GENOME_SIZE; i++){
		if(output_buffer[loop] == 22){
			break;
		}
		
		if(qrand() % MUTATION_RATE_REPRODUCTION == 0){
			switch(qrand() % 3){
			case 0:
			case 1://command replacement
				neighbour->genome[i] = randomOperation();
				break;
			case 2://duplication or removal
				loop = qrand() % GENOME_SIZE;
				break;
			}
		}else{
			neighbour->genome[i] = output_buffer[loop];
		}
		loop++;
	}
	
	if(i < GENOME_SIZE - 1){
		memset(cell->genome +i*sizeof(uchar),
				GENOME_OPERATIONS - 1 ,
				((GENOME_SIZE - 1) - i) * sizeof(uchar));
	}
}

/**
 * Initialize each cell
 */
void Simulation::init(){
	int x = 0;
	int y = 0;
	int z = 0;
	
	for(x = 0; x < WORLD_X; x++){
		for(y = 0; y < WORLD_Y; y++){
			for(z = 0; z < WORLD_Z; z++){
				killCell(&cells[x][y][z]);

				cells[x][y][z].energy = 0;
			}
		}
	}
	
	struct Place *place;
	for(x = 0; x < WORLD_X; x++){
		for(y = 0; y < WORLD_Y; y++){
			for(z = 0; z < WORLD_Z; z++){
				place = &world[x][y][z];
				place->reproducable = true;
			}
		}
	}
}

/**
 * mutates the whole genome
 */
void Simulation::mutateCell(struct Cell *cell){	
	int stops = 0;
	for(int i = 0; i < GENOME_SIZE; i++){
		if(cell->genome[i] != GENOME_OPERATIONS-1){
			stops = 0;
			if(qrand() % MUTATION_RATE_NON_LIVING == 0){
				cell->genome[i] = randomOperation();
			}
		}else{
			stops++;
			if(stops == 4){
				break;
			}
		}
	}
}

/**
 * returns a random operation
 */
uchar Simulation::randomOperation(){
	return (uchar)(qrand() % GENOME_OPERATIONS);
}

/**
 * returns the cell at the specified position
 */
struct Cell *Simulation::cell(int x, int y, int z){
	return &cells[x][y][z];
}

/**
 * returns the neighbour in the specified direction
 * wraps around edges
 */
struct Position Simulation::getNeighbour(int x, int y, int z, uchar direction){
	
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
		x = 0;
	}else if(x < 0){
		x = WORLD_X-1;
	}
	
	if(y >= WORLD_Y){
		y = 0;
	}else if(y < 0){
		y = WORLD_Y - 1;
	}
	
	if(z >= WORLD_Z){
		z = WORLD_Z - 1;
	}else if(z < 0){
		z = 0;
	}
	
	struct Position pos;
	pos.x = x;
	pos.y = y;
	pos.z = z;
	return pos;
}
