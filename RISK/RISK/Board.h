#ifndef Board_H
#define Board_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>

//#include "agents"


#include "engine.h"
#include "Territory.h"
#include "gui.h"
#include "Card.h"
#include "RiskAgent.h"
#include "AgentFactory.h"

using namespace std;

//Forward declaration of classes with pointers (i think thats what causes it)
class Engine;
class GUI;
class RiskAgent;


class Board{

public:
	Board();
	Board(string, string);
	Board(string);
	~Board();
	
	vector<Territory> gameBoard;
	
	/**
	 *	GUI Methods
	 */
	int* getOwnerArray();
	int* getArmyCountArray();
	int getCurrentPlayer();
	int getGamePhase();
	int getSubGamePhase();
	string getTerrTarget();
	string getTerrBase();
	bool isVictory();

	//bool ownedByCurrentPlayer(Territory &);
	Territory & searchTerritoryName(string);
	void resetMoveableUnits();
	
	int getCurrentPlayerNumTerritoriesOwned();
	bool legalTerritoryName(string);	
	//bool placementPossible();
	bool attackPossible();	
	bool fortifyPossible();

	/**
	 *	Turn Methods
	 */
	bool cashCards( Card, Card, Card);
	void placeArmies( int, Territory); // country
	void placeArmies( int, int);       //code
	int attack( Territory, Territory, bool);
	int attack( int, int, bool);
	int fortifyArmies( int, Territory, Territory);
	int fortifyArmies( int, int, int);

	GUI * gui;

private:
	string stringToUpper(string);
	void setAgentPrefs();
	bool unownedTerritoryExsits();
	map<string, Factory*>  factory_map;
	RiskAgent * createAgent(string);
	int nGames;
	bool quit;
	ofstream saveFile;
	Engine * e;
	string Board::getGuiCommand();
	vector<int> setOptions(string);
	void init(string);
	bool currentPlayerAlive();
	int play();
	void playTurn(RiskAgent *);// /*AgentX, string, string, float, GUI&*/);
	void Board::playHumanTurn();
	
	/*
	 * Save Data Methods
	 */	
	void writeState();
	void writePhase();
	string convertInt(int);
	string getTime();

	void updateGUI();
	int supplyBaseUnits();

	const char * AGENT_DIR;

	RiskAgent* tmpAgnt; //TODO: use code to create map and/or factory to create AGENTS
	
	vector<RiskAgent*> players;
	vector<Card> cardDeck;
	vector<string> playerNames;
	bool searchDir(const char*);

};
#endif




