#ifndef Card_H
#define Card_H

#include <string>
#include <vector>

#include "Territory.h"

class Territory;

class Card{

public:
	Card(int, int);

	int getCode();
	int getSymbol();
	bool isASet( Card, Card, Card );
	bool containsASet( vector<Card> );
	vector<Card> getBestSet( vector<Card>, int, vector<Territory> );
	vector<Card> getRandomSet(  vector<Card> );

private:
	// The territory code of the country that is on this card:
	int territoryCode;

	// The symbol on the card (we just assign the three different 
	// pictures to be 0 for horses, 1 for cannons, 2 for men, and 3 
	// for wildcard (on which there will be a country code of -1):
	int symbol;
};
#endif