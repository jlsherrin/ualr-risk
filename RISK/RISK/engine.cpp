#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cmath>

#include "engine.h"
#include "Board.h"
#include "Territory.h"

using namespace std;


Engine::Engine(){

}

Engine::Engine( Board * const  b, vector<int> & options){

	numPlayers = options[0]; 
	//cardsType = options[1];
	//contBonus = options[2];
	//numTerr = options[3];
	//numCont = options[4];
	
	maxAttackDice = 3;
	maxDefendDice = 2;
	minBaseSupplyUnits = 3;
	baseSupplyUnitsDivisor = 3;

	board = b;
	initializeAvailableUnits();
	initializeCurrentPlayer();

	for(int i=0;i<numPlayers;i++){
		gamePhases.push_back(INITIAL);
		subGamePhases.push_back(DONE);
	}

	targetSelected = false;
	baseSelected = false;
	terrTarget = "NA";
	terrBase = "NA";

}

Engine& Engine::operator=(const Engine eng){
	Engine * ePtr = new Engine();
	ePtr->numPlayers = eng.numPlayers;
	ePtr->cardsType = eng.cardsType;
	ePtr->contBonus = eng.contBonus;

	return *ePtr;
}

Engine::~Engine(){
	
}

void Engine::changeGameState(string _command){

	// Direct Phase Change By Player
	if(!_command.compare("Done")){
		if( (gamePhases[currentPlayer] != INITIAL) && (subGamePhases[currentPlayer] != PLACEMENT) &&subGamePhases[currentPlayer] !=CARDS ){
			subGamePhases[currentPlayer] = DONE;
		}
	}
	
	// Direct Phase Change By Player
	if(!_command.compare("Fortify")){
		if(subGamePhases[currentPlayer] == ATTACK){
			subGamePhases[currentPlayer] = FORTIFY;
		}
	}

	// Initial Placement Phase
	if(gamePhases[currentPlayer] == INITIAL){
		initialGamePhase(_command);
	}

	// Normal Game Play
	if(gamePhases[currentPlayer] == NORMAL){

		//Cards Phase:
		if(subGamePhases[currentPlayer] == CARDS){
			cardsGamePhase(_command);
		}else{	
			//Placement Phase: Always Generates a legal attack
			if(subGamePhases[currentPlayer] == PLACEMENT){
				placementGamePhase(_command);	
			}else{
				//Attack Phase:
				//  1) Attack can succeed leading to automatic fortify or victory
				//  2) Attack can fail leading to automatic done
				//  3) Attack can be skipped (see 'Fortify or Done Direct Phase Change')
				if(subGamePhases[currentPlayer] == ATTACK){
					attackGamePhase(_command);
				}
				//Fortification Phase:
				//  1) Fortification can be skipped (see 'Done Direct Phase Change')
				//  2) Fortification must be ended (see 'Done Direct Phase Change')
				if(subGamePhases[currentPlayer] == FORTIFY){
					fortifyGamePhase(_command);
				}

			}
		}

		// Done Phase:
		// Performs end-of-turn clean-up
		//   1) Sets current player subphase to PLACMENT
		//   2) Gives 
		if(subGamePhases[currentPlayer] == DONE){
			doneGamePhase(_command);	
		}

	}

}

int Engine::getAvailableUnits(){
	return availableUnits[currentPlayer];
}

void Engine::initializeCurrentPlayer(){
	currentPlayer = rand()%numPlayers;
}

void Engine::initializeAvailableUnits(){

	int numUnits = 0;
	switch(numTerr){
	case 9: //board.gameBoard is North America ONLY
		switch(numPlayers){
		case 2:
			numUnits = 7;
			break;
		case 3:
			numUnits = 9;
			break;
		case 4:
			numUnits = 8;
			break;
		case 5:
			numUnits = 7;
			break;
		case 6:
			numUnits = 5;
			break;
		default:
			numUnits = 0;
		}
		break;
	case 42: //board.gameBoard is whole world board
		switch(numPlayers){
		case 2:
			numUnits = 25;
			break;
		case 3:
			numUnits = 35;
			break;
		case 4:
			numUnits = 30;
			break;
		case 5:
			numUnits = 25;
			break;
		case 6:
			numUnits = 20;
			break;
		default:
			numUnits = 0;
		}
		break;
	case 10: //board.gameBoard is South America and Africa ONLY
		switch(numPlayers){
		case 2:
			numUnits = 8;
			break;
		case 3:
			numUnits = 10;
			break;
		case 4:
			numUnits = 9;
			break;
		case 5:
			numUnits = 8;
			break;
		case 6:
			numUnits = 6;
			break;
		default:
			numUnits = 0;
		}
		break;
	default:
		switch(numPlayers){
		case 2:
			numUnits = 25;
			break;
		case 3:
			numUnits = 35;
			break;
		case 4:
			numUnits = 30;
			break;
		case 5:
			numUnits = 25;
			break;
		case 6:
			numUnits = 20;
			break;
		default:
			numUnits = 0;
		}
	}
	
	for(int i=0;i<numPlayers;i++){
		availableUnits.push_back(numUnits);
	}

}

int Engine::getMinBaseSupply(){
	return minBaseSupplyUnits;
}

int Engine::getSupplyDivisor(){
	return baseSupplyUnitsDivisor;
}

int Engine::supplyBaseUnits(){

  int nSupply = minBaseSupplyUnits;

  int nOwned = getCurrentPlayerNumTerritoriesOwned();
  int vOwned = (int)floor((double)nOwned/baseSupplyUnitsDivisor);

  if(vOwned>nSupply){
	nSupply=vOwned;
  }
  return(nSupply);
  
}

int Engine::getCurrentPlayerNumTerritoriesOwned(){

	int ownedCount = 0;
	vector<Territory>::iterator cycleBoard;
	for(cycleBoard = board->gameBoard.begin(); cycleBoard != board->gameBoard.end();cycleBoard++){
		if(currentPlayer == cycleBoard->getOwner()){
			ownedCount++;
		}
	}

	return(ownedCount);

}

bool Engine::legalTerritoryName(string _command){

	bool legal = false;
	vector<Territory>::iterator cycleBoard;
	for(cycleBoard = board->gameBoard.begin(); cycleBoard != board->gameBoard.end(); cycleBoard++){
		
		if(_command.compare( cycleBoard->getName() ) == 0){
			legal = true;
		}
	}
	return(legal);
}

Territory & Engine::searchTerritoryName(string _command){
	//TODO: FIX searchTerritoryName
	//vector<Territory>::iterator terrID;
	vector<Territory>::iterator cycleBoard;	
	
	for(cycleBoard = board->gameBoard.begin(); cycleBoard != board->gameBoard.end(); cycleBoard++){	//when this if statement is never true	
		if(_command.compare( cycleBoard->getName() ) == 0){								//this assignment statement returns a left over value of terrID	
			terrID = cycleBoard;														//Temporary fix: Edited the Data file 	
		}																				
	}	
	return(*terrID);

	/*int terrID = 0;
	
	for(unsigned i=0;i<board.gameBoard.size();i++){
		if(_command.compare( (board.gameBoard[i]).getName() ) == 0){
			terrID = i;
		}
	}	
	return(board.gameBoard[terrID]);*/
}

void Engine::cardsGamePhase(string _command){
	subGamePhases[currentPlayer] = PLACEMENT;
}

void Engine::changeSubGamePhase(int phase){
	subGamePhases[currentPlayer] = phase;
}

bool Engine::placeUnit(Territory & _territory){

	bool placeStatus=false;

	if(legalPlacement(_territory)){

		board->gameBoard[_territory.getTerritoryCode()].addUnit();
		board->gameBoard[_territory.getTerritoryCode()].setOwner(currentPlayer);
		availableUnits[currentPlayer]--;
		placeStatus=true;

	}

	return(placeStatus);

}

bool Engine::placementPossible(){

	bool placementPossible = false;
	if(availableUnits[currentPlayer]>0){
		placementPossible = true;
	}
	return(placementPossible);
}

bool Engine::legalPlacement(Territory & _territory){

	bool legal = false;
	if(!(_territory.isOccupied())){
		legal = true;
	}else{
		if(allTerritoriesOccupied() && ownedByCurrentPlayer(_territory) ){
			legal = true;
		}
	}

	return(legal);
}

bool Engine::ownedByCurrentPlayer(Territory & _territory){

	bool owned = false;
	if( (_territory.getOwner()) == currentPlayer){
		owned = true;
	}
	return(owned);

}

bool Engine::allTerritoriesOccupied(){
	vector<Territory>::iterator cycleBoard;
	bool allOccupied = true;
	for(cycleBoard = board->gameBoard.begin(); cycleBoard != board->gameBoard.end(); cycleBoard++){
		if(!cycleBoard->isOccupied()){
			allOccupied = false;
		}
	}
	return(allOccupied);
}

void Engine::updateCurrentPlayer(){
	
	currentPlayer++;
	currentPlayer%=numPlayers;
}

void Engine::initialGamePhase(string _command){

	if(placementPossible()){
	
		if(legalTerritoryName(_command)){
			
			if(placeUnit(searchTerritoryName(_command))){
									
				if(!placementPossible())
				{																
					gamePhases[currentPlayer]=NORMAL;
					subGamePhases[currentPlayer]=DONE;

				}
				else
				{				
					updateCurrentPlayer();				
				}
			}
		}
	}
}

void Engine::placementGamePhase(string _command){

	if(legalTerritoryName(_command)){

		//The command is a placement command
		if(placeUnit(searchTerritoryName(_command))){
											
			//If the placement exhausts
			if(!placementPossible()){
					
				subGamePhases[currentPlayer] = ATTACK;
				targetBaseReset();

			}
		}
	}
}

void Engine::attackGamePhase(string _command){

	if(attackPossible()){
			
		if(!targetSelected){

			if(legalTerritoryName(_command) && !( searchTerritoryName(_command).getOwner() == currentPlayer)){
				terrTarget = _command;
				targetSelected = true;
			}

		}else{ 

			if(!baseSelected){

				//Select all but one units of base for attack
				if(legalTerritoryName(_command)){
					terrBase = _command;
					baseSelected = true;
				}else{
					
					if( _command.compare("NA") !=0 ){
						targetBaseReset();
					}

				}

			}else{

				//Handle Attack Request
				if(legalAttack(searchTerritoryName(terrTarget),searchTerritoryName(terrBase))){
						
					resolveBattle(searchTerritoryName(terrTarget),searchTerritoryName(terrBase));

					//If this battle exhauted potential attacks, change subphase
					if(!attackPossible()){
						//subGamePhases[currentPlayer] = DONE;
						subGamePhases[currentPlayer] = FORTIFY;
					}

				}
				
				//Reset Target Base for (new Attack/new Fortify)
				targetBaseReset();

			}

		}
			
	}else{
				
		if(fortifyPossible()){
			//subGamePhases[currentPlayer] = DONE;
			subGamePhases[currentPlayer] = FORTIFY;
		}else{
			subGamePhases[currentPlayer] = DONE;
		}

	}//end else

}

void Engine::targetBaseReset(){
	terrBase = "NA";
	terrTarget = "NA";
	baseSelected = false;
	targetSelected = false;
}

bool Engine::attackPossible(){

	bool attackPossible = false;

	//For all territories
	for(unsigned i=0;i<board->gameBoard.size();i++){
		Territory terr = board->gameBoard[i];

		//If player owns a territory    
		if(terr.getOwner() == currentPlayer){

			//...that has more than one army
			if(terr.getNumUnits()>1){

				int Nnbrs = terr.getNumNeighbors();

				for(int j=0;j<Nnbrs;j++){

					//...that also has a nbr not owned by player
					Territory nbr = searchTerritoryName(terr.getNeighbor(j));
					if(nbr.getOwner() != currentPlayer){
						attackPossible=true;
					}
				}
			}
		}
	}

	return(attackPossible);

}

bool Engine::legalAttack(Territory & _target, Territory & _base){

	bool legalAttack = false;

	// Check for:
	// 1) Current player owns the base territory
	// 2) The target territory is a neighbor of base
	// 3) The base has enough available units to attack
	if(currentPlayer == _base.getOwner()){

		if( (_base.getNumUnits()-1) > 0 ){
	  
			if(_base.isNeighborOf(_target.getName())){
					
				//Territories neighbor each other
				legalAttack = true;

			}
		}
	}
	return(legalAttack);

}

int Engine::resolveBattle(Territory & _target, Territory & _base){

	int nAttack = _base.getNumUnits()-1;
	int nDefend = _target.getNumUnits();

	//Compute attack/defend volleys
	vector<int> attackVolleys = attack(_base.getNumUnits());
	vector<int> defendVolleys = defend(_target.getTerritoryCode());

	//Sort attack/defend volleys
	sortVolleys(attackVolleys);
	sortVolleys(defendVolleys);

	//Delete defender casualties
	for(int i=0;i<computeCasualties(DEFENDMODE,attackVolleys,defendVolleys);i++){
		_target.deleteUnit();
	}
		
	//Delete attacker casualties
	for(int i=0;i<computeCasualties(ATTACKMODE,attackVolleys,defendVolleys);i++){
		_base.deleteUnit();
	}

	nAttack  = _base.getNumUnits()-1;
	nDefend = _target.getNumUnits();
	
	//if (board->players[currentPlayer]->getName() == "PLAYER");

	if(nAttack>0){
		_target.setOwner(_base.getOwner());
		//moveInArmies()
		return WIN;
	}else if (nAttack == 0){
		return LOSE;
	}else if (nAttack > 0 && nDefend >0){
		return PUSH;
	}else{
		return ATK_ERROR;
	}
}

vector<int> Engine::attack(int atkUnits){
	
	return(volley(min(maxAttackDice,atkUnits-1)));

}

vector<int> Engine::defend(int defender){

	return(volley(min(maxDefendDice,board->gameBoard[defender].getNumUnits())));

}

vector<int> Engine::volley(int nVolleys){

	vector<int> volleys;
	for(int i=0;i<nVolleys;i++){
		volleys.push_back(shot());
	}
	return(volleys);

}

int Engine::shot(){
	return(6*(double)rand()/RAND_MAX+1);
}

void Engine::sortVolleys(vector<int> & volleys){ 
  
	//Sort in descending order (bubblesort)
	for(unsigned i=0;i<volleys.size();i++){
		for(unsigned j=0;j<(volleys.size()-1);j++){
			if(volleys[j+1]>volleys[j]){
				int tmp = volleys[j];
				volleys[j] = volleys[j+1];
				volleys[j+1] = tmp;
			}
		}
	}
}

int Engine::computeCasualties(int mode, vector<int> & attackVolleys, vector<int> & defendVolleys){

	int nMaxcasualties = min(defendVolleys.size(),attackVolleys.size());
  	int Ncasualties = 0;

	switch(mode){
		
	case ATTACKMODE:
    
		//Compute Attack Casualties
		for(int i=0;i<nMaxcasualties;i++){
      
			if(defendVolleys[i]>=attackVolleys[i]){
				Ncasualties++;
			}
		}
		break;

	case DEFENDMODE:
    
		//Compute Defend Casualties
		for(int i=0;i<nMaxcasualties;i++){
      
			if(defendVolleys[i]<attackVolleys[i]){
				Ncasualties++;
			}
		}
		break;

	default:
		break;
	}

  return(Ncasualties);

}

void Engine::fortifyGamePhase(string _command){

	if(!fortifyPossible()){

		//End the turn
		subGamePhases[currentPlayer] = DONE;

	}else{

		if(!targetSelected){

			if(legalTerritoryName(_command)){
				terrTarget = _command;
				targetSelected = true;
			}

		}else{ 

			if(!baseSelected){

				//Select all but one units of base for attack
				if(legalTerritoryName(_command)){
					terrBase = _command;
					baseSelected = true;
				}else{
					
					if( _command.compare("NA") !=0 ){
						targetBaseReset();
					}

				}

			}else{

				//Handle Attack Request
				if(legalFortification(searchTerritoryName(terrTarget),searchTerritoryName(terrBase))){
					resolveFortification(searchTerritoryName(terrTarget),searchTerritoryName(terrBase));
				}
				
				//Reset Target Base for (new Attack/new Fortify)
				targetBaseReset();

			}
		}		
	}
}

bool Engine::fortifyPossible(){

	bool fortifyPossible = false;
	vector<Territory>::iterator cycleBoard;
	//For all territories
	for(cycleBoard = board->gameBoard.begin(); cycleBoard != board->gameBoard.end(); cycleBoard++){
		Territory terr = *cycleBoard;

		//If player owns a territory    
		if(terr.getOwner() == currentPlayer){

			//...that has more than one army
			if(terr.getNumUnits()>1){

				int Nnbrs = terr.getNumNeighbors();

				for(int j=0;j<Nnbrs;j++){

					//that also has a nbr owned by player
					Territory nbr = searchTerritoryName(terr.getNeighbor(j));
					if(nbr.getOwner() == currentPlayer){
						fortifyPossible=true;
					}
				}
			}
		}
	}

	return(fortifyPossible);

}

bool Engine::legalFortification(Territory & _target, Territory & _base){

	bool legalFortify = false;

	if( (_base.getOwner()==currentPlayer) && (_target.getOwner()==currentPlayer) ){

		if((_base.getNumUnits()-1)>0){
	  
			if(_base.isNeighborOf(_target.getName())){
					
				legalFortify = true;

			}
		}
	}

	return(legalFortify);

}

void Engine::resolveFortification(Territory & _target, Territory & _base){

	_base.deleteUnit();
	_base.setNumMoveableUnits(_base.getNumMoveableUnits() - 1 );
	_target.addUnit();
	
}

void Engine::doneGamePhase(string _command){

	if(!isVictory()){		
		subGamePhases[currentPlayer] = CARDS;
		updateCurrentPlayer();
		while (getCurrentPlayerNumTerritoriesOwned() < 1) updateCurrentPlayer();
			
							
		if(gamePhases[currentPlayer] == INITIAL){
			//Initialize an initial turn (i.e., do nothing)
		}else{
			//Initialize a normal turn
			availableUnits[currentPlayer] = supplyBaseUnits();	
		}
	}
}

int Engine::getBaseUnits(){
	return supplyBaseUnits();
}

/*
* Instructor Defined Function for GUI
*/
int* Engine::getOwnerArray(){
	int* ownerArray = new int[board->gameBoard.size()];
	for(unsigned i=0;i<board->gameBoard.size();i++){
		ownerArray[i] = (board->gameBoard[i]).getOwner();
	}
	return(ownerArray);
}

/*
* Instructor Defined Function for GUI
*/
int* Engine::getArmyCountArray(){
	int* armyCountArray = new int[board->gameBoard.size()];
	for(unsigned i=0;i<board->gameBoard.size();i++){
		armyCountArray[i] = (board->gameBoard[i]).getNumUnits();
	}
	return(armyCountArray);
}

/*
* Instructor Defined Function for GUI
*/
int Engine::getCurrentPlayer(){
	return(currentPlayer);
}

/*
* Instructor Defined Function for GUI
*/
int Engine::getGamePhase(){
	return(gamePhases[currentPlayer]);
}

/*
* Instructor Defined Function for GUI
*/
int Engine::getSubGamePhase(){
	return(subGamePhases[currentPlayer]);
}

/*
* Instructor Defined Function for GUI
*/
string Engine::getTerrTarget(){
	return(terrTarget);
}

/*
* Instructor Defined Function for GUI
*/
string Engine::getTerrBase(){
	return(terrBase);
}

/*
* Instructor Defined Function for GUI 
*/
//TODO: REWRITE THIS FOR MULTIPLE PLAYERS
bool Engine::isVictory(){

	bool victory = true;
	vector<Territory>::iterator cycleBoard;
	for(cycleBoard = board->gameBoard.begin(); cycleBoard != board->gameBoard.end(); cycleBoard++){
		if(currentPlayer != cycleBoard->getOwner() || cycleBoard->getOwner() == -1){
			victory = false;
		}
	}
	if (victory) gamePhases[currentPlayer] = VICTORY;
	
	return(victory);
}


void Engine::setTerrTarget(string terr){
	terrTarget = terr;
}
void Engine::setTerrBase(string terr){
	terrBase = terr;
}