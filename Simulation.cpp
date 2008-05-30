#include "Simulation.h"
#include <cstring>

Simulation::Simulation()
{
	cellid = 0;
	running = true;
	
	mutex = new QSemaphore(1);
	count = 0;
	energyAdd = ENERGY_ADDED;
	nextSet = false;
	canExecuteNext = true;
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
	    mutex->acquire(1);
		round++;
		count++;
		if(nextSet){
			x = nextx;
			y = nexty;
			z = nextz;
			nextSet = false;
			canExecuteNext--;
		}else{
			//Select random cell
			x = qrand() % WORLD_X;
			y = qrand() % WORLD_Y;
			z = qrand() % WORLD_Z;
			canExecuteNext = MAX_EXECUTION_ROW;
		}
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
		
		
		if(cells[x][y][z].generation >= LIVING && cells[x][y][z].bad){
			double killValue = 64.0 /
				(( cells[x][y][z].bad) * ( cells[x][y][z].bad));
			if(((int) killValue) == 0 || !(qrand() % (int) killValue)){
				killCell(&cells[x][y][z]);
			}
		}
		
#ifdef DECREASE_ENERGY
		if(round % ENERGY_DECREASE == 0){
			qDebug() << "Decrease energy";
			energyAdd -= 100;
			if(energyAdd < 0){
				energyAdd = 0;
			}
		}
#endif

		//add energy every x rounds
		if(!(round % ENERGY_FREQUENCY)){
			regenerateEnergy();
		}
		
		mutex->release(1);
	}
}

void Simulation::killCell(struct Cell *cell){
	cell->parent = 0;
	cell->lineage = 0;
	cell->generation = 0;
	cell->id = 0;
	cell->genome_size = GENOME_SIZE;
	cell->reproduced = 0;
	cell->brain = 0;

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
	double x = qrand() % WORLD_X;
	double y = qrand() % WORLD_Y;
	double z = qrand() % WORLD_Z;
	
	double mod = 1.0;
	
#ifdef VARIED_ENERGY
	if(x <= WORLD_X / 2){
		mod *= x / (WORLD_X / 2.0);
	}else{
		mod *= 1.0 - (x / WORLD_X );
	}
	
	if(y <= WORLD_Y / 2){
		mod *= y / (WORLD_Y / 2.0);
	}else{
		mod *= 1.0 - (y / WORLD_Y );
	}
	
	mod += 0.1;
#endif
	struct Cell *cell = &cells[(int)x][(int)y][(int)z];
	cell->energy += energyAdd * mod;
	
	if(cell->bad){
		cell->bad--;
	}
}


/**
 * return true if a certain type of action is allowed from cell source to
 * cell dest.
 * The action can be specified with the parameter good
 * the chance of success can be better with the guess parameter
 */
bool Simulation::accessOk(struct Cell *source, struct Cell *dest, char guess,bool good){
	if(dest->generation <= LIVING ||
			dest->genome[0] == guess ||
			(dest->genome[0] == source->genome[0] && good)){
		return true;
	}
		
	return !(qrand() % ACCESS_CHANCE);
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
	
	memset(output_buffer, NO_REP_OPERATION, GENOME_SIZE * sizeof(uchar));
	
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
		if(!(qrand() % MUTATION_RATE_EXECUTION)){
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
			if(pointer >= GENOME_SIZE ){
				pointer = 0;
			}
			break;
		case 2: //pointer --
			pointer--;
			if(pointer < 0){
				pointer = GENOME_SIZE - 1;
			}
			break;
		case 3: //register ++
			reg++;
			if(reg >= GENOME_OPERATIONS ){
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
				while(cell->genome[genome_pointer] != 10 &&
						cell->energy &&
						!stop){
					
					genome_pointer++;
					/*if(genome_pointer >= GENOME_SIZE)
						genome_pointer = 0;*/
					
					cell->energy--;
					if(genome_pointer == tempP ||
					   genome_pointer >= GENOME_SIZE){
						stop = true;
					}
				}
				
				genome_pointer++;
				if(genome_pointer >= GENOME_SIZE)
					genome_pointer = 0;
			}
			break;
		case 10://}
			if(reg){
				int tempP = genome_pointer;
				while(cell->genome[genome_pointer] != 9 &&
						cell->energy &&
						!stop){
					genome_pointer--;
					/*if(genome_pointer < 0){
						genome_pointer = GENOME_SIZE-1;*/
					
					cell->energy--;
					
					if(genome_pointer == tempP || 
							genome_pointer < 0){
						stop = true;
					}
				}
				
				genome_pointer++;
				if(genome_pointer >= GENOME_SIZE)
					genome_pointer = 0;
			}
			break;
		case 11://NOP2, stops reproduction
			break;
		case 12:{ //move
			struct Position pos = getNeighbour(x,y,z,facing);
			tmpCell = &cells[pos.x][pos.y][pos.z];
			
			if(tmpCell != cell && accessOk(cell, tmpCell, reg,false) && cell->energy2 >= 5){
				cell->energy2 -= 5;
				int tempEnergy = cell->energy2;				
				cell->energy2 = cell->energy2 / 2 + tmpCell->energy2;				
				tmpCell->energy2 = tempEnergy / 2;
				
				tempEnergy = cell->energy;				
				cell->energy = cell->energy / 2 + tmpCell->energy;				
				tmpCell->energy = tempEnergy / 2;
				
				struct Cell tmp;
				/*memcpy(&tmp, tmpCell, sizeof(struct Cell));
				memcpy(tmpCell, cell, sizeof(struct Cell));
				memcpy(cell, &tmp, sizeof(struct Cell));
				*/
				tmp = *tmpCell;
				*tmpCell = *cell;
				*cell = tmp;				
				
				x = pos.x;
				y = pos.y;
				z = pos.z;
				cell = &cells[x][y][z];
				stop = true;
			}
			
		}break;
		case 13:{ // kill
			struct Position pos = getNeighbour(x,y,z,facing);
			tmpCell = &cells[pos.x][pos.y][pos.z];
			if(cell->generation >= LIVING  &&
					accessOk(cell, tmpCell, reg,false) &&
					cell->energy2 > 0){
				killCell(tmpCell);
				cell->energy2--;
			}
		}break;
		case 14:{//remove bad
			if(cell->bad && cell->energy >= 5){
				cell->bad--;
				cell->energy -= 5;
			}
		}break;
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
			cell->brain = reg;
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
		case 19:{//execute neigbour
			if(canExecuteNext && cell->energy2){
				int executeDir = reg % DIRECTIONS;
				struct Position pos = getNeighbour(x,y,z,executeDir);
				nextx = pos.x;
				nexty = pos.y;
				nextz = pos.z;
				nextSet = true;
				cell->energy2--;
			}
			stop = true;
		}break;
		case 20:{//NOP
			reg = cell->brain;
		}break;
		case 21:
			if(temp == reg){
				reg = 1;
			}else{
				reg = 0;
			}
			break;
		case 22:{ //seek most energy
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
		case 23:{//eat energy and modify neighbour genome
			struct Position pos = getNeighbour(x,y,z,facing);
			tmpCell = &cells[pos.x][pos.y][pos.z];
			if((!tmpCell->generation ||
					(cell->generation >= LIVING  && !(qrand() % 5))) &&
					accessOk(cell, tmpCell, reg,false)){
				if(tmpCell->genome[pointer] == reg && reg != 0 
						&& reg != GENOME_OPERATIONS - 1){
					tmpCell->genome[pointer] = 0;
					
					//eat energy
					cell->energy2 += EAT_ENERGY / 3;
					if(tmpCell->energy2 >= (2 * EAT_ENERGY) / 3){
						cell->energy2 += (2 * EAT_ENERGY) / 3;
						tmpCell->energy2 -= (2 * EAT_ENERGY) / 3;
					}
				}
			}
		}break;
		case 24:{
			if(cell->energy2){
				cell->energy2--;
				cell->energy += 20;
				if(!(qrand() % 10)){
					cell->bad++;
				}
			}
		}break;
		case 25:{//eject
			struct Position pos = getNeighbour(x,y,z,facing);
			tmpCell = &cells[pos.x][pos.y][pos.z];
			if(accessOk(cell, tmpCell, reg,true)){
				
				switch(reg){
					case 0:{ //eject bad particle
						if(cell->bad){
							cell->bad--;
							tmpCell->bad++;
						}
					}break;
					case 1:{
						if(tmpCell->bad){
							cell->bad++;
							tmpCell->bad--;
						}
					}break;
					default:{
						if(cell->energy >= reg){
							cell->energy -= reg;
							tmpCell->energy += reg;
						}
					}break;
				}
				
				uint tmpEnergy = tmpCell->energy + cell->energy;
				tmpCell->energy = tmpEnergy / 2;
				cell->energy = tmpEnergy / 2;
			}
		}break;
		case 26: //end
			stop = true;
			break;
		}
	}
	
	output_pointer = 0;
	
	//jeah, we can reproduce something
	if(output_buffer[0] != NO_REP_OPERATION){
		if(world[x][y][z].reproducable){
			struct Position pos = getNeighbour(x,y,z,facing);
			struct Cell *neighbour = &cells[pos.x][pos.y][pos.z];
			if(accessOk(cell, neighbour, reg,false)){
				reproduce(cell,neighbour,output_buffer);
			}
			cell->reproduced = 0;
		}
	}else{
		cell->reproduced++;
	}
	
	if(cell->generation >= LIVING && cell->reproduced > 3){
		killCell(cell);
	}
}

/**
 * reproduce cell.
 * Mutations are introduced 
 */
void Simulation::reproduce(struct Cell *cell, struct Cell *neighbour,uchar *output_buffer){
	if(neighbour->energy == 0)
		return;
	
	int loop = 0;
	int i = 0;
	bool stoped = false;
	int copied = 0;
	
	for(i = 0; i < GENOME_SIZE && loop < GENOME_SIZE; i++){
		if(output_buffer[loop] == NO_REP_OPERATION){
			stoped = true;
			break;
		}
		
		if(!(qrand() % MUTATION_RATE_REPRODUCTION)){
			switch(qrand() % 5){
			case 0:
			case 1://command replacement
				neighbour->genome[i] = randomOperation();
				break;
			case 2://duplication or removal
				loop = qrand() % GENOME_SIZE;
				break;
			case 3:{//insert command
				loop++;
				}break;
			case 4:{//remove command
				loop--;
				}break;
			}
		}else{
			neighbour->genome[i] = output_buffer[loop];
		}
		
		loop++;
		copied++;
	}
	
	/*if(!stoped && i < GENOME_SIZE - 1){
		neighbour->parent = cell->id;
		neighbour->lineage = neighbour->id;
		neighbour->generation = 1;
	}*/
	
	if(i < GENOME_SIZE - 1 && i > GENOME_SIZE / 5){
		memset(neighbour->genome +i*sizeof(uchar),
				GENOME_OPERATIONS - 1 ,
				((GENOME_SIZE - 1) - i) * sizeof(uchar));
	}
	
	if(copied >= MIN_COPY){
		if(cell->id == 0){
			cell->id = ++cellid;
			cell->lineage = cell->id;
		}
		
		neighbour->id = ++cellid;
		
		neighbour->parent = cell->id;
		
		neighbour->generation = cell->generation + 1;
		
		neighbour->lineage = cell->lineage;
		
		neighbour->brain = 0;
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

				cells[x][y][z].energy = ENERGY_ADDED;
				cells[x][y][z].energy2 = 0;
				cells[x][y][z].bad = 1;
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
	
#ifdef OLDSTYLE_MUTATION
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
#else
	double prob =  GENOME_SIZE / MUTATION_RATE_NON_LIVING;
	
	register int max = cell->bad;
	if(max > GENOME_SIZE / 4){
		max = GENOME_SIZE / 4;
	}
	
	max = max * prob;
	
	if(max < 2){
		max = 2;
	}
	
	max = qrand() % max;
	
	for(int i = 0; i < max; i++){
		int pos = qrand() % GENOME_SIZE;
		cell->genome[pos] = randomOperation();
	}

#endif
}

/**
 * returns a random operation
 */
inline uchar Simulation::randomOperation(){
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
	case NORTH:{
		y--;
		if(y >= WORLD_Y){
			y = 0;
		}
	}
		break;
	case SOUTH:{
		y++;
		if(y < 0){
			y = WORLD_Y - 1;
		}
	}
		break;
	case WEST:{
		x--;
		if(x < 0){
			x = WORLD_X-1;
		}
	}
		break;
	case EAST:{
		x++;
		if(x >= WORLD_X){
			x = 0;
		}
	}
		break;
	case UP:{
		z++;
		if(z >= WORLD_Z){
			z = WORLD_Z - 1;
		}
	}
		break;
	case DOWN:{
		z--;
		if(z < 0){
			z = 0;
		}
	}
		break;
	}
	
	struct Position pos;
	pos.x = x;
	pos.y = y;
	pos.z = z;
	return pos;
}
