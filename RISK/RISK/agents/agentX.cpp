#include <cstdlib>
#include <cmath>
#include <string>

#include "AgentX.h"
#include "../Card.h"

using namespace std;


//enum{ATTACKMODE,DEFENDMODE};
//enum{INITIAL,NORMAL,VICTORY};
//enum{CARDS,PLACEMENT,ATTACK,FORTIFY,DONE};
//enum{PUSH,WIN,LOSE,ERROR};

AgentX::AgentX(){
	readConfigFile();
}

//void AgentX::setPrefs(int id, Board b){
//	myId = id;
//	board = &b;
//}

AgentX::~AgentX(){

}

//BASIC AI FUNCTIONS
string AgentX::getPlaceCommand(){   //TODO: remove type not needed; seperate Agent_X from Strawman
	string command = "NA";
	//vector<Territory> terrOwned;	//Territories owned by currentPlayer
	//TODO: Declare vectors above in header; use terrOwned.clear(); to erase at end of func
	
	for ( unsigned i = 0; i < board->gameBoard.size();i++){
		if (board->gameBoard[i].getOwner() == ID){
			for (int j=0; j< board->gameBoard[i].getNumNeighbors();j++){
				if(board->searchTerritoryName(board->gameBoard[i].getNeighbor(j)).getOwner() != ID ){
					terrOwned.push_back(board->gameBoard[i]);
				}
			}
		}
	}
	int rnd = rand() % terrOwned.size();
	command = terrOwned[rnd].getName();
	terrOwned.clear();
	return (command);
}

bool AgentX::myContains(string terrName, vector<Territory> owned){
	//Not Needed 
	for(int i=0; i<owned.size();i++){
		if(owned[i].getName() == terrName){
			return true;
		}
	}
	return false;
}

string AgentX::getTargetCommand(){
	string command = "NA";
	for ( unsigned i = 0; i < board->gameBoard.size();i++){
		if (board->gameBoard[i].getOwner() == ID){
			terrOwned.push_back(board->gameBoard[i]);
		}else 
		{
			terrNotOwned.push_back(board->gameBoard[i]);
		}
	}

	while (command =="NA"){
		int rndTarget = rand() % terrNotOwned.size();		
		for (int i = 0 ; i < terrNotOwned[rndTarget].getNumNeighbors(); i++){
			Territory nbr = board->searchTerritoryName(board->gameBoard[terrNotOwned[rndTarget].getTerritoryCode()].getNeighbor(i));
			if (nbr.getOwner() == ID){
				if (attackTable(nbr.getNumUnits(), terrNotOwned[rndTarget].getNumUnits()) > atkConfidence  /*&& !type.compare("S")*/){
					command = (terrNotOwned[rndTarget]).getName();
				}
			}
		}
	}	
	terrOwned.clear();
	terrNotOwned.clear();
	tmpTarget = command;
	return command;
}

string AgentX::getBaseCommand(){
	string command = "NA";
	vector<Territory> nbrsOwned;	//Neighbors of the Current Target Territory Owned by Current Player

	for ( int i = 0; i < board->searchTerritoryName(tmpTarget).getNumNeighbors();i++){
		if (board->searchTerritoryName( board->searchTerritoryName(tmpTarget).getNeighbor(i)).getOwner() == ID){
			nbrsOwned.push_back( board->searchTerritoryName(board->searchTerritoryName(tmpTarget).getNeighbor(i)));
		}
	}

	while (command == "NA"){
		int rnd = rand() % nbrsOwned.size();
		//if (nbrsOwned[rnd].getNumUnits() > 1 /*&& !type.compare("R")*/){
		//	command = nbrsOwned[rnd].getName();
		//}
		//else
		if (attackTable(nbrsOwned[rnd].getNumUnits(), board->searchTerritoryName(tmpTarget).getNumUnits()) > atkConfidence){
				command = nbrsOwned[rnd].getName();			
		}
	}	
	return (command);
}

string AgentX::getFortifyCommand(){
	return "DONE";
}

int AgentX::pickCountry(){
	int command;
	//Find unowned territories
	for ( unsigned i = 0; i < board->gameBoard.size();i++){
		if (board->gameBoard[i].getOwner() == -1){
			terrUnOwned.push_back(board->gameBoard[i]);
		}
	}
	int rnd = rand() % terrUnOwned.size();
	command = terrUnOwned[rnd].getTerritoryCode();
	terrUnOwned.clear();
	return command;
}

void AgentX::placeInitialArmies(){
	int command;
	for ( unsigned i = 0; i < board->gameBoard.size();i++){
		if (board->gameBoard[i].getOwner() == board->getCurrentPlayer()){
			terrOwned.push_back(board->gameBoard[i]);
		}
	}
	int rnd = rand() % terrOwned.size();
	command = terrOwned[rnd].getTerritoryCode();
	terrOwned.clear();
	board->placeArmies(1,command);
}

double AgentX::attackTable(int atkUnits, int defUnits){
	double atkTbl[11][11] ={{0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0},
							{0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0},
							{1.0,	0.417,	0.106,	0.027,	0.007,	0.002,	0.0,	0.0,	0.0,	0.0,	0.0},
							{1.0,	0.754,	0.363,	0.206,	0.091,	0.049,	0.021,	0.011,	0.005,	0.003,	0.001},
							{1.0,	0.916,	0.656,	0.470,	0.315,	0.206,	0.134,	0.084,	0.054,	0.033,	0.021},
							{1.0,	0.972,	0.785,	0.642,	0.477,	0.359,	0.253,	0.181,	0.123,	0.086,	0.057},
							{1.0,	0.990,	0.890,	0.769,	0.638,	0.506,	0.397,	0.297,	0.224,	0.162,	0.118},
							{1.0,	0.997,	0.934,	0.857,	0.745,	0.638,	0.521,	0.423,	0.329,	0.258,	0.193},
							{1.0,	0.999,	0.967,	0.910,	0.834,	0.736,	0.640,	0.536,	0.446,	0.357,	0.287},
							{1.0,	1.0,	0.980,	0.947,	0.888,	0.818,	0.730,	0.643,	0.547,	0.464,	0.380},
							{1.0,	1.0,	0.990,	0.967,	0.930,	0.873,	0.808,	0.826,	0.646,	0.558,	0.480} };
	

	if (atkUnits > 10) atkUnits = atkUnits%10;  
	if (defUnits > 10) defUnits = defUnits%10;
	return atkTbl[atkUnits][defUnits];
}

bool AgentX::attackPossible(){
	bool attackPossible = false;

	//For all territories
	for(unsigned i=0;i<board->gameBoard.size();i++){
		Territory terr = board->gameBoard[i];

		//If player owns a territory    
		if(terr.getOwner() == ID){

			//...that has more than one army
			if(terr.getNumUnits()>1){

				int Nnbrs = terr.getNumNeighbors();

				for(int j=0;j<Nnbrs;j++){

					//...that also has a nbr not owned by player
					Territory nbr = board->searchTerritoryName(terr.getNeighbor(j));
					if(nbr.getOwner() !=ID && attackTable(terr.getNumUnits(), nbr.getNumUnits()) > atkConfidence){
						attackPossible=true;
					}
				}
			}
		}
	}
	return(attackPossible);
}

void AgentX::cardsPhase(){
	if (cardsInHand.size() > 2) cashCardsIfPossible(cardsInHand);
}

void AgentX::placeArmies(int nArmies){
	while( nArmies > 0){
		board->placeArmies(1, board->searchTerritoryName(getPlaceCommand()));
		nArmies--;
	}
}

void AgentX::attackPhase(){
	 int result;
	 if(attackPossible()){
		result = board->attack(board->searchTerritoryName(getBaseCommand()),
			                   board->searchTerritoryName(getTargetCommand()),
							   true);
	 }else{result = board->attack(*new Territory("Done"),*new Territory("Done"),false);}
}
int AgentX::moveArmiesIn(int oldTerr, int newTerr){
	return board->gameBoard[oldTerr].getNumUnits() - 1;
}
void AgentX::fortifyPhase(){
	board->fortifyArmies(1,board->searchTerritoryName(getFortifyBase()), board->searchTerritoryName(getFortifyTarget()));
}

string AgentX::getFortifyBase(){
	return "Done";
}


string AgentX::getFortifyTarget(){
	return "Done";
}

string AgentX::getCardsCommand(){
	// This Method is a place holder for where you would call
	// NO LONGER NEEDED ????
	return "";
}

void AgentX::readConfigFile(){
	//This method will read from an agent configFile 
	//filename = "./AgentX.conf";
	atkConfidence = 0.6;
}
