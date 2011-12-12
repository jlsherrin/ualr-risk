#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <vector>

#include "Territory.h"
#include "Board.h"

using namespace std;
//Forward declaration of classes with pointers (i think thats what causes it)
class Board;

class Engine{

public:

	//Important Engine Methods
	Engine();
	Engine( Board * const , vector<int> &);
	Engine& operator=(const Engine);
	~Engine();
	void changeGameState(string);

	int getMinBaseSupply();
	int getSupplyDivisor();
	int getBaseUnits();

	// GUI METHODS
	int* getOwnerArray();		//TODO: move to Board
	int* getArmyCountArray();	//TODO: move to Board
	int getCurrentPlayer();
	int getGamePhase();
	int getSubGamePhase();
	string getTerrTarget();		//TODO: move to Board ????
	string getTerrBase();		//TODO: move to Board ????
	bool isVictory();

	//Moved
		//vector<Territory> gameBoard;            //TODO: move to Board
		bool ownedByCurrentPlayer(Territory &); //TODO: move to Board      Territory.getOwner()
		Territory & searchTerritoryName(string);//TODO: move to Board
						//TODO: move to Board or Agent
		int getAvailableUnits();

		bool placementPossible();
		bool attackPossible();
		int resolveBattle(Territory &, Territory &);
		bool fortifyPossible();

		void changeSubGamePhase(int);
		void setTerrTarget(string);
		void setTerrBase(string);

private:

	/*
	 * Private Members
	 */
	int currentPlayer;
	int maxAttackDice;
	int maxDefendDice;
	int minBaseSupplyUnits;
	int baseSupplyUnitsDivisor;

	/*
	 * Options
	 */
	int numPlayers;	
	int cardsType;
	int contBonus;
	int numTerr;
	int numCont;

	Board* board;
	//Board b;
	bool targetSelected;
	bool baseSelected;

	string terrTarget;
	string terrBase;

	vector<int> gamePhases;
	vector<int> subGamePhases;
	vector<int> availableUnits;
	
	vector<int>::iterator cycleUnits;
	vector<Territory>::iterator terrID;

	/*
	 * Private Methods
	 */

	//Initialization Methods
	void initializeGameBoard(string);   //TODO: Board() constructor
	void initializeCurrentPlayer();
	void initializeAvailableUnits();
	int supplyBaseUnits();

	//Turn methods
	void initialGamePhase(string);
	void cardsGamePhase(string);		//TODO: ADD logic for this here? or maybe in Board???
	void placementGamePhase(string);
	void attackGamePhase(string);
	void fortifyGamePhase(string);
	void doneGamePhase(string);

	//General Rule Manipulation Methods
	bool legalTerritoryName(string);	
	bool allTerritoriesOccupied();
	
	void updateCurrentPlayer();
	int getCurrentPlayerNumTerritoriesOwned();
	void targetBaseReset();

	//Placement Methods
	
	bool legalPlacement(Territory &);
	bool placeUnit(Territory &);

	//Attack Methods
	
	bool legalAttack(Territory &, Territory &);

	//Battle Methods
	
	int computeCasualties(int, vector<int> &, vector<int> &);
	void sortVolleys(vector<int> &);
	vector<int> attack(int);
	vector<int> defend(int);
	vector<int> volley(int);
	int shot();

	//Fortify Functions
	
	bool legalFortification(Territory &, Territory &);
	void resolveFortification(Territory &, Territory &);

};

#endif
