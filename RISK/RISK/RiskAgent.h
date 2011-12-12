#ifndef RiskAgent_H
#define RiskAgent_H

#include <string>
#include <vector>


#include "enums.h"
#include "Board.h"
#include "Card.h"
#include "Territory.h"
//#include "AgentFactory.h"




//Forward declaration of classes with pointers (i think thats what causes it)
class Board;
class Card;
class Territory;

class RiskAgent {

public:
	
	//virtual ~RiskAgent() = 0;
	
	int ID;
	string name;
	virtual int pickCountry() = 0;
	virtual void placeInitialArmies() = 0;

	virtual void cardsPhase() = 0;
	virtual void placeArmies(int) = 0;
	virtual void attackPhase() = 0;
	virtual int moveArmiesIn(int, int) = 0;
	virtual void fortifyPhase() = 0;
	virtual void setPrefs(int, Board *);
	

protected:	
	
	Board * board;
	//Cards Methods
	void cashCardsIfPossible(vector<Card> cards);

	//Place Methods
	void setGoalToLeastBordersCont();
	int pickCountryInSmallContinent();
	int pickCountryInContinent(int continent);
	int pickCountryTouchingUs();
	int pickCountryNearCluster( Territory root );
	int getEasiestContToTake();
	bool weOwnContsArround(Territory center);
	void placeArmiesToTakeCont( int numberOfArmies, int wantCont );
	void placeArmiesOnClusterBorder( int numberOfArmies, Territory root );

	//Attack Methods

	//Fortify Method
	void fortifyCluster( Territory root );

	bool ownNeighbor(string);

};
#endif