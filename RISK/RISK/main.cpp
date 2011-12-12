#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <sstream>
#include <ctime>
#include <Windows.h>
#include <iostream>

#include "Territory.h"
#include "Board.h"
#include "engine.h"
#include "enums.h"
#include "AgentX.h"
#include "gui.h"
#include "Card.h"
#include "Player.h"

int main(int argc, char *argv[]){
	//take some command line argurments pass to new board
	Board b("./data/maps/map_large_markup.txt","./data/conf/test1.txt");

	return 0;
}