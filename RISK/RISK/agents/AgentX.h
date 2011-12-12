#ifndef AgentX_H
#define AgentX_H

#include <string>

//TODO: ADD HEADERS TO PRECOMPILER SCRIPT FOR SIMPLICITY
//#include "../Board.h"
#include "../RiskAgent.h"
//#include "../Card.h"

using namespace std;

//Forward declaration of classes with pointers (i think thats what causes it)
class Board;
class RiskAgent;
class Card;

class AgentX : public RiskAgent{

public:
		
	AgentX();
	~AgentX();
		
	int pickCountry();
	void placeInitialArmies();
	void cardsPhase();

	//The board calls this method giving you a # of armies to place
	//Tell the board where to put them using one of the board's placeArmies methods
	//	board->placeArmies(int numberOfArmies, Territory target) or
	//	board->placeArmies(int numberOfArmies, int targetTerritoryCode)
	void placeArmies(int);
	void attackPhase();
	int moveArmiesIn(int, int);
	void fortifyPhase();

	string getCardsCommand();
	string getPlaceCommand(); //TODO: remove string
	string getBaseCommand();
	string getTargetCommand();
	string getFortifyBase();
	string getFortifyTarget();
	string getFortifyCommand();
	

	//TODO: Remove the following three methods deprecated
	/*string getGreedyPlaceCommand(Engine &);
	string getSmartBaseCommand(Engine &);
	string getSmartTargetCommand(Engine &);*/

	bool attackPossible();

private:
	//void initState(Engine &);
	bool ownNeighbor(string);
	double attackTable(int, int);

	float atkConfidence;
	//Board* board;

	vector<Card> cardsInHand;

	//int myId;

	void readConfigFile();
	bool myContains(string, vector<Territory>);

	vector<Territory> terrOwned;	//Territories owned by currentPlayer
	vector<Territory> terrUnOwned;  //Territories no one owns
	vector<Territory> terrNotOwned; //Territories owned by opponent(s)
	string tmpTarget;
};


#endif
