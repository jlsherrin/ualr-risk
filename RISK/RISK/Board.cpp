#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <sstream>
#include <ctime>
#include <Windows.h>
#include <iostream>
#include <ctype.h>

#include "Territory.h"
#include "Board.h"
#include "engine.h"
#include "enums.h"
#include "AgentX.h"
#include "gui.h"
#include "Card.h"
#include "Player.h"

using namespace std;

//enum{ATTACKMODE,DEFENDMODE};
//enum{INITIAL,NORMAL,VICTORY};
//enum{CARDS,PLACEMENT,ATTACK,FORTIFY,DONE};
//enum{PUSH,WIN,LOSE,INVALID};


Board::Board(string mapFile, string configFile){
	AGENT_DIR = "./agents/*.cpp";

	
	//ADD NEW AGENTS TO THIS MAP
	// factory_map["agent_name"] = new AgentFactory<class_name>();
	factory_map["AgentX"] = new AgentFactory<AgentX>();
	factory_map["Player"] = new AgentFactory<Player>();

	//Seed random number generator
	srand((unsigned)time(0)); 	
	
	//Initialize save file
	nGames = 1;
	string time = getTime();
	saveFile.open(time + ".dat", ios::app);   

	vector<int> options = setOptions(configFile);		
	init(mapFile);
	for (unsigned i = 0; i < gameBoard.size();i++){
		Card tmpCard(i,rand()%3);
		cardDeck.push_back(tmpCard);
	}
	Card wildCard(gameBoard.size()+2,3);
	cardDeck.push_back(wildCard);
	cardDeck.push_back(wildCard);
	e = new Engine(this, options);
	gui = new GUI(mapFile);
	setAgentPrefs();
	int gameCount = 0;
	while(gameCount < nGames){
		play();
		gameCount++;
	}
	saveFile.close();
}

Board::Board(string _pathToGameFile){
	init(_pathToGameFile);
}

Board::~Board(){
	//delete e;
	delete gui;
}

void Board::setAgentPrefs(){
	vector<RiskAgent*>::iterator cyclePlayers;
	
	for(unsigned i = 0; i < players.size();i++){
		players[i]->setPrefs(i,this);
	}
}

void Board::init(string _pathToGameFile){
	fstream inf;
	inf.open(_pathToGameFile.c_str(),ios::in);
	
	int Nterritories = 0;
	inf >> Nterritories;

	for(int i=0;i<Nterritories;i++){

		//string emptyline;
		int Nname;
		string name;
		string tmpName;
		string continent;
		int Nneighbors;
		string neighbor;

		//Build territory name
		inf >> Nname;
		tmpName = "";
		name = "";	
		for(int j=0;j<Nname;j++){
			inf >> tmpName;
			name+=tmpName;
		}
		Territory tmpTerr(name);
		tmpTerr.setTerritoryCode(i);

		//Build continent name
		inf >> Nname;
		continent = "";
		for(int j=0;j<Nname;j++){
			inf >> tmpName;
			continent+=tmpName;
		}
		tmpTerr.setContinent(continent);
		
		inf >> Nneighbors;
		for(int j=0;j<Nneighbors;j++){
			inf >> Nname;
			neighbor = "";
			for(int k=0;k<Nname;k++){
				inf >> tmpName;
				neighbor+=tmpName;
			}
			tmpTerr.addNeighbor(neighbor);
		}
		gameBoard.push_back(tmpTerr);

		//Skip over GUI coordinates
		int x, y, w, h, z, p;
 		inf >> x >> y >> w >> h >> z >> p;
	}

}
/**
 *	GUI Methods
 */
int* Board::getOwnerArray(){
	int* ownerArray = new int[gameBoard.size()];
	for(unsigned i=0;i<gameBoard.size();i++){
		ownerArray[i] = (gameBoard[i]).getOwner();
	}
	return(ownerArray);
}

int* Board::getArmyCountArray(){
	int* armyCountArray = new int[gameBoard.size()];
	for(unsigned i=0;i<gameBoard.size();i++){
		armyCountArray[i] = (gameBoard[i]).getNumUnits();
	}
	return(armyCountArray);
}

int Board::getCurrentPlayer(){
	return(e->getCurrentPlayer());
}

int Board::getGamePhase(){
	return(e->getGamePhase());
}

int Board::getSubGamePhase(){
	return(e->getSubGamePhase());
}

string Board::getTerrTarget(){
	return(e->getTerrTarget());
}

string Board::getTerrBase(){
	return(e->getTerrBase());
}

bool Board::isVictory(){

	return e->isVictory();
}

bool Board::searchDir(const char * name){	
	WIN32_FIND_DATA data;
	HANDLE h = FindFirstFile(AGENT_DIR,&data);
	
	if( h!=INVALID_HANDLE_VALUE ) 
	{
		do
		{
			char*   nPtr = new char [lstrlen( data.cFileName ) + 1];
			for( int i = 0; i < lstrlen( data.cFileName ); i++ )
				nPtr[i] = char( data.cFileName[i] );

			nPtr[lstrlen( data.cFileName )] = '\0';
			if (nPtr == name){
				return true;
			}
		} while(FindNextFile(h,&data));
	} 
	else {
		cout << "Error: No such folder." << endl;
	}
	
	FindClose(h);
	
	return false;
}

vector<int> Board::setOptions(string conf){
	/* Reads the configFile and sets the corresponding options to create an engine.
		options[0] = number of players
		options[1] = CardMode
		options[2] = ContBonusMode
		options[3] = number of territories
		options[4] = number of continents

		options[1] = number of players
		options[1] = number of players
		options[1] = number of players
	*/
	vector<int> ops;
	fstream inf;
	int temp = 0;
	string tempName;
	inf.open(conf.c_str(),ios::in);
	inf >> nGames;	
	int nPlayers = 0;
	inf >> nPlayers;
	ops.push_back(nPlayers);
	for (int i = 0; i < nPlayers ;i++){
		inf >> tempName;
		const char * file = tempName.c_str();
		players.push_back(factory_map[tempName]->create());	
		players[players.size()-1]->name = stringToUpper(tempName);
	}
	for ( int i = 0; !inf.eof();i++){
		inf >> temp;
		ops.push_back(temp);
	}
	return ops;
}

int Board::play(){

	int gameCount = 0;

	//While there are games to be played
	while (gameCount < nGames){
		//Initialize Termination Criteria
		bool quit = false;
		
		//While the game is not over
		//while(!engine.isVictory()){
		while (quit == false && !isVictory()){
			if(gui->observeEvent()){
				//GUI transmits quit event
				if(gui->quitGame()){
					quit=true;
				}
			}
			while (!isVictory()){
				if (currentPlayerAlive()){
					if(players[e->getCurrentPlayer()]->name == "PLAYER"){
						playHumanTurn();
					}else{
						playTurn(players[e->getCurrentPlayer()]);
					}
				}
			}
		}
		gameCount++;
		
	}		

	//Return
	return 0;

}

void Board::playHumanTurn(){
	updateGUI();
	//Initial Game Phase
	if (getGamePhase() == INITIAL){
		writePhase();
		e->changeGameState(getGuiCommand());
		writeState();
	}
	
	
	//Cards Turn Phase
	if(e->getSubGamePhase() == CARDS){
		writePhase();
		e->changeSubGamePhase(PLACEMENT);	
		writeState();
		updateGUI();
	}

	//Placement Turn Phase
	if(e->placementPossible() && getSubGamePhase() == PLACEMENT){
		writePhase();
		e->changeGameState(getGuiCommand());	
		writeState();
		updateGUI();
	}

	//Attack Turn Phase
	while ( attackPossible() && e->getSubGamePhase() == ATTACK){
		//Write the current Phase to the DataFile
		writePhase();
		string target = "";
		//getGuiCommand();
		string command = "";
		while (target ==""){
			if(gui->observeEvent()){
				if(gui->quitGame()){	//GUI transmits quit event
					quit=true;
				}else{					//Otherwise, GUI transmits commands
					/*
					 * Receive Command from GUI to Modify the Game State
					 */
					target = gui->getCommand();					
				}
			}
		}
		//return command;
		e->changeGameState(target);
		updateGUI();

		string base = "";
		//getGuiCommand();
		while (base ==""){
			if(gui->observeEvent()){
				if(gui->quitGame()){	//GUI transmits quit event
					quit=true;
				}else{					//Otherwise, GUI transmits commands
					/*
					 * Receive Command from GUI to Modify the Game State
					 */
					base = gui->getCommand();

					
				}
			}
		}
		e->changeGameState(base);
		updateGUI();
		//int result = attack(searchTerritoryName(base),searchTerritoryName(target),true);	
		//if (result == WIN){
		//	int nUnits = searchTerritoryName(target).getNumUnits() + searchTerritoryName(base).getNumUnits();
		//	searchTerritoryName(target).setNumMoveableUnits(searchTerritoryName(target).getNumUnits());
		//	searchTerritoryName(base).setNumMoveableUnits(searchTerritoryName(base).getNumUnits());
		//	while (nUnits > 0){
		//		e->changeGameState(getGuiCommand());
		//		nUnits--;
		//	}
		//}
		writeState();
		updateGUI();
		
	}

	//Skip the FORTIFY Phase and update the GUI
	e->changeGameState("Done");//	e->changeGameState("Done");	
	updateGUI();
}

string Board::getGuiCommand(){
	string command = "";
	while (command ==""){
		if(gui->observeEvent()){

			//GUI transmits quit event
			if(gui->quitGame()){

				quit=true;

			//Otherwise, GUI transmits commands
			}else{

				/*
				 * Receive Command from GUI to Modify the Game State
				 */
				command = gui->getCommand();
			}
		}
		//updateGUI();
	}
	return command;
}

bool Board::unownedTerritoryExsits(){
	for (unsigned i =0; i < gameBoard.size();i++){
		if (gameBoard[i].getOwner() == -1){
			return true;
		}
	}
	return false;
}

void Board::playTurn(RiskAgent * agnt){
	updateGUI();
	//Initial Game Phase
	if (getGamePhase() == INITIAL){
		if(unownedTerritoryExsits()){
			writePhase();
			e->changeGameState(gameBoard[agnt->pickCountry()].getName());
			writeState();
			updateGUI();
		}else{
			writePhase();
			agnt->placeInitialArmies();
			writeState();
			updateGUI();
		}
	}
	
	
	//Cards Turn Phase
	if(e->getSubGamePhase() == CARDS){
		writePhase();
		agnt->cardsPhase();
		writeState();
		e->changeSubGamePhase(PLACEMENT);
		updateGUI();		
	}

	//Placement Turn Phase
	if(e->placementPossible() && getSubGamePhase() == PLACEMENT){
		writePhase();
		agnt->placeArmies(supplyBaseUnits());
		writeState();
		updateGUI();
				
	}

	//Attack Turn Phase
	while ( attackPossible() && e->getSubGamePhase() == ATTACK){
		//Write the current Phase to the DataFile
		updateGUI();
		writePhase();
		agnt->attackPhase();
		writeState();
		updateGUI();
	}

	//Skip the FORTIFY Phase and update the GUI
	e->changeGameState("Done");	
	updateGUI();



}

void Board::updateGUI(){
	gui->displayGameState(getOwnerArray(), getArmyCountArray(), getCurrentPlayer(), getGamePhase(),
						getSubGamePhase(), getTerrTarget(), getTerrBase(), isVictory());
}

void Board::writePhase(){
	string phase("");
	
	if (e->getGamePhase() == 0){
		phase = "-1";
	}else{
		phase = convertInt(e->getSubGamePhase());
	}
	phase += " ";
	saveFile << phase;
}

void Board::writeState(){
	string state("");

	///* Lists all owners and numArmies in order of Territories */
	//for (unsigned i = 0; i < gameBoard.size(); i++) {		
	//	state += convertInt(gameBoard[i].getOwner());
	//	state += " ";
	//	state += convertInt(gameBoard[i].getNumUnits());
	//	state += " ";
	//}

	/* Lists all owners then all numArmies */
	for (unsigned i = 0; i < gameBoard.size(); i++) {		
		state += convertInt(gameBoard[i].getOwner());
		state += " ";
	}
	for (unsigned i = 0; i < gameBoard.size(); i++) {		
		state += convertInt(gameBoard[i].getNumUnits());
		state += " ";
	}

	state += "\n";

	saveFile << state;
}

string Board::convertInt(int n ){

	stringstream ss;
	ss << n;
	return ss.str();
}

bool Board::currentPlayerAlive(){
	bool alive = false;
	if(e->getGamePhase() == INITIAL){
		alive = true;
	}
	else{
		for ( unsigned i = 0; i < gameBoard.size();i++){
			if (gameBoard[i].getOwner() == e->getCurrentPlayer()){
				alive = true;
			}
		}
	}
	return alive;
}

int Board::getCurrentPlayerNumTerritoriesOwned(){

	int ownedCount = 0;
	vector<Territory>::iterator cycleBoard;
	for(cycleBoard = gameBoard.begin(); cycleBoard != gameBoard.end();cycleBoard++){
		if(e->getCurrentPlayer() == cycleBoard->getOwner()){
			ownedCount++;
		}
	}

	return(ownedCount);
	
}

bool Board::legalTerritoryName(string _command){
	//return e.legalTerritryName(_command);
	bool legal = false;
	vector<Territory>::iterator cycleBoard;
	for(cycleBoard = gameBoard.begin(); cycleBoard != gameBoard.end(); cycleBoard++){
		
		if(_command.compare( cycleBoard->getName() ) == 0){
			legal = true;
		}
	}
	return(legal);
}

bool Board::attackPossible(){

	return(e->attackPossible());

}

bool Board::fortifyPossible(){
	return e->fortifyPossible();
}

void Board::resetMoveableUnits(){
	for (unsigned i = 0; i < gameBoard.size();i++){
		gameBoard[i].setNumMoveableUnits(gameBoard[i].getNumUnits());
	}
}

/** Cashes in the given card set. Each parameter must be a reference to a different Card instance sent via cardsPhase(). 
It returns true if the set was cashed, false otherwise. */
bool Board::cashCards( Card card, Card card2, Card card3 ){
	return true;
}
	
/** Places numberOfArmies armies in the given country. */
void Board::placeArmies( int numberOfArmies, Territory terr ){
	for(unsigned i = 0; i < numberOfArmies; i++){
		writePhase();
		e->changeGameState(terr.getName());
		updateGUI();
		writeState();
	}
}

/** Places numberOfArmies armies in the given country. */
 void Board::placeArmies( int numberOfArmies, int terr ){
	for(unsigned i = 0; i < numberOfArmies; i++){
		writePhase();
		e->changeGameState(gameBoard[terr].getName());
		updateGUI();
		writeState();
	}
}
	
			
/** If <i>attackTillDead</i> is true then perform attacks until one side or the other has been defeated. 
Otherwise perform a single attack.<P>
This method may only be called from within an agent's attackPhase() method. */	
 int Board::attack( Territory attacker, Territory defender, bool attackTillDead){
	int result = ERROR;
	if (attacker.getName() == "Done"){
		e->changeGameState("Done");
		updateGUI();
	}else if (attackTillDead){
			result = e->resolveBattle(defender, attacker);
			updateGUI();
			writePhase();writeState();
			while(result == PUSH){
				 result = e->resolveBattle(defender, attacker);
				 updateGUI();
				 writePhase();writeState();
			}
			if(result == ERROR){//Do SOMETHING
			}
			if(result == WIN){
				gameBoard[defender.getTerritoryCode()].setOwner(attacker.getOwner());
				updateGUI();
				if (players[e->getCurrentPlayer()]->name == "PLAYER"){						
					int nUnits = attacker.getNumUnits();
					gameBoard[defender.getTerritoryCode()].setNumMoveableUnits(defender.getNumUnits());
					gameBoard[attacker.getTerritoryCode()].setNumMoveableUnits(attacker.getNumUnits());
					for (int i = nUnits; i > 0; i--){
						gameBoard[defender.getTerritoryCode()].deleteUnit();
						gameBoard[attacker.getTerritoryCode()].deleteUnit();
					}
					while(nUnits > 0){
						string command = getGuiCommand();
						if(searchTerritoryName(command).getOwner() == players[e->getCurrentPlayer()]->ID){
							gameBoard[searchTerritoryName(command).getTerritoryCode()].addUnit();
							nUnits--;
						}
					}		
				}else{					
					int armiesToMove = players[attacker.getOwner()]->moveArmiesIn(attacker.getTerritoryCode(), defender.getTerritoryCode());
					for (int i = 0; i < armiesToMove; i++){
						gameBoard[defender.getTerritoryCode()].addUnit();
						gameBoard[attacker.getTerritoryCode()].deleteUnit();
					}
					updateGUI();writePhase();writeState();
				}
			}
		}else{
			result = e->resolveBattle(defender, attacker);
			updateGUI();writePhase();writeState();
			if(result == WIN){
				if (players[e->getCurrentPlayer()]->name == "PLAYER"){

				}else{
					gameBoard[defender.getTerritoryCode()].setOwner(attacker.getOwner());
					int armiesToMove = players[attacker.getOwner()]->moveArmiesIn(attacker.getTerritoryCode(), defender.getTerritoryCode());
					for (int i = 0; i < armiesToMove; i++){
						gameBoard[defender.getTerritoryCode()].addUnit();
						gameBoard[attacker.getTerritoryCode()].deleteUnit();
					}
					updateGUI();writePhase();writeState();
				}
			}
		}
	return result;
}
	

/** If <i>attackTillDead</i> is true then perform attacks until one side or the other has been defeated.
Otherwise perform a single attack.<P>
This method may only be called from within an agent's attackPhase() method. */	

// this is the one that actually calls world.attack()
 int Board::attack( int attackerCode, int defenderCode, bool attackTillDead){
	 e->changeGameState(gameBoard[attackerCode].getName());
	 e->changeGameState(gameBoard[defenderCode].getName());
	 return 0;
}
	

/** Order a fortification move.	
This method may only be called from within an agent's 'fortifyPhase()' method. It returns 1 on a successful fortify, 0 if no armies could be fortified (countries must always keep 1 army on them) and a negative number on failure.	*/
 int Board::fortifyArmies( int numberOfArmies, Territory base, Territory target){
	e->changeGameState(base.getName());
	e->changeGameState(target.getName());
	return 0;
}
	
	
/** Order a fortification move.	
This method may only be called from within an agent's 'fortifyPhase()' method. 
It returns 1 on a successful fortify, 
0 if no armies could be fortified (countries must always keep 1 army on them) 
and a negative number on failure.	*/
 int fortifyArmies( int numberOfArmies, int countryCodeOrigin, int countryCodeDestination){
	 return 0;
}

int Board::supplyBaseUnits(){
	return(e->getBaseUnits());  
}

string Board::getTime(){
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime (buffer,80,"%Y%m%d_%H%M",timeinfo);
	string time = buffer;
	return time;
}

Territory & Board::searchTerritoryName(string _command){
	////TODO: FIX searchTerritoryName
	//vector<Territory>::iterator terrID;
	//vector<Territory>::iterator cycleBoard;	
	//
	//for(cycleBoard = gameBoard.begin(); cycleBoard != gameBoard.end(); cycleBoard++){	//when this if statement is never true	
	//	if(_command.compare( cycleBoard->getName() ) == 0){								//this assignment statement returns a left over value of terrID	
	//		terrID = cycleBoard;														//Temporary fix: Edited the Data file 	
	//	}																				
	//}	
	//return(*terrID);

	int terrID = 0;
	
	for(unsigned i=0;i<gameBoard.size();i++){
		if(_command.compare( (gameBoard[i]).getName() ) == 0){
			terrID = i;
		}
	}	
	return(gameBoard[terrID]);
}

string Board::stringToUpper(string strToConvert)
{//change each element of the string to upper case
   for(unsigned int i=0;i<strToConvert.length();i++)
   {
      strToConvert[i] = toupper(strToConvert[i]);
   }
   return strToConvert;//return the converted string
}