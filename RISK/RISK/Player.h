#ifndef Player_H
#define Player_H

#include "RiskAgent.h"
#include "gui.h"

class RiskAgent;
class Board;
class Card;
class GUI;

class Player : public RiskAgent{

public:
	Player();

	void setGUI(GUI *);

	virtual int pickCountry() ;
	virtual void placeInitialArmies();

	virtual void cardsPhase() ;
	virtual void placeArmies(int);
	virtual void attackPhase();
	virtual int moveArmiesIn(int, int);
	virtual void fortifyPhase();

private:
	string getCommand();

	GUI * gui;

};
#endif