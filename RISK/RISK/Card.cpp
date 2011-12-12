#include <vector>
#include <cmath>

#include "Card.h"
#include "Territory.h"

using namespace std;
//class Territory;
//class Card;

Card::Card( int assocCode, int pic ){
	symbol = pic;
	if ( symbol == 3 )
		{
		territoryCode = -1;
		}
	else
		{
		territoryCode = assocCode;
		}
}

/** Returns the country code associated with this card. It will return -1 if the card is wild. */
int Card::getCode(){
	return territoryCode;
}

/** Returns the symbol on the card. Normal symbols are 0,1,2 while wildcards are 3.*/
int Card::getSymbol(){
	return symbol;
}

bool Card::isASet( Card card1, Card card2, Card card3 ){
	// Cycle through the cards and count the number of each symbol:
	int symbolCount[4];
	symbolCount[ card1.getSymbol() ]++;
	symbolCount[ card2.getSymbol() ]++;
	symbolCount[ card3.getSymbol() ]++;

	if ( symbolCount[0] + symbolCount[3] == 3 || symbolCount[1] + symbolCount[3] == 3 || symbolCount[2] + symbolCount[3] == 3 )
		return true;

	if ( ( min( symbolCount[0], 1 ) + min( symbolCount[1], 1 ) + min( symbolCount[2], 1 ) + symbolCount[3] ) > 2 )
		return true;

	// Otherwise, they have no set:
	return false;
}


vector<Card> Card::getBestSet( vector<Card> cards, int player, vector<Territory> territories )
    {	
	// First look for a set using non wildcards:
	vector<Card> noWildList;		
	vector<Card> wildList;	
	for (int i = 0; i < cards.size(); i++)
		{
		if (cards[i].getSymbol() != 3)
			noWildList.push_back(cards[i]);
		else
			wildList.push_back(cards[i]);
		}
	if (cards.size() > noWildList.size())
		{
		// Then there were some wildcards. Look for a set amongst the non-wildcards:
		vector<Card> noWildCards;
		for (int i = 0; i < noWildList.size(); i++)
			noWildCards[i] = noWildList[i];
			
		vector<Card> bestNoWildSet = getBestSet(noWildCards, player, territories);
		if (bestNoWildSet.size() == 3)
			{
			// We found a set using no wildcards, return it:
			return bestNoWildSet;
			}
			
		// No set was found using none of our wildcards. try with only one if we have multiple wildcards:
		if (cards.size() > noWildList.size() + 1)
			{
			vector<Card> oneWildCardList = noWildList;
			for (int i = 0; i < noWildList.size(); i++)
				oneWildCardList[i] = noWildList[i];
			oneWildCardList.push_back(wildList[0]);
			
			vector<Card> bestOneWildSet = getBestSet(oneWildCardList, player, territories);
			if (bestOneWildSet.size() == 3)
				{
				// We found a set using no wildcards, return it:
				return bestOneWildSet;
				}			
			}
		}
		
	// our attempts at finding a set with no wildcards have failed (or we have no wildcards).
			
    // populate an array with the ownercodes of the cards
    int * owners = new int[cards.size()];
    int owned = 0;

    for (int i = 0; i < cards.size(); i++){
		if (cards[i].getCode() == -1)
			owners[i] = -1;
		else
			{
			owners[i] = territories[cards[i].getCode()].getOwner();
			if (owners[i] == player)
				owned++;
			}
    }

    // start by getting an array with all the owned cards
    vector<Card> ownedCards;
    int count = 0;
    
    for (int i = 0; i < cards.size(); i++){
        if (owners[i] == player)
            {
            ownedCards[count] = cards[i];
            count++;
            }
    }

    if (owned > 2)
        {
        if (cards[0].containsASet( ownedCards ))
            {
            return getRandomSet( ownedCards );
            }
        }

    // OK, they have no sets made up of three of their cards.
    // look for sets with two now
    if (owned > 1)
        {
        vector<Card> ownedPlus;
        for (int i = 0; i < owned; i++)
            ownedPlus[i] = ownedCards[i];
 
        // now loop over all the non-owned cards looking for a set using only one of them
        for (int c = 0; c < cards.size(); c++)
            {
            if (owners[c] != player)
                {
                ownedPlus[owned] = cards[c];
                if (containsASet( ownedPlus ))
                    {
                    return getRandomSet( ownedPlus );
                    }
                }
            }
        }

    // ok, there is no set using two of the player's cards
    if (owned > 0)
        {
        vector<Card> ownedPlusPlus;
        for (int i = 0; i < owned; i++)
            ownedPlusPlus[i] = ownedCards[i];
 
        for (int c = 0; c < cards.size(); c++)
        for (int d = 0; d < cards.size(); d++)
            {
            if (owners[c] != player && owners[d] != player && c != d)
                {
                ownedPlusPlus[owned] = cards[c];
                ownedPlusPlus[owned+1] = cards[d];
                if (containsASet( ownedPlusPlus ))
                    {
                    return getRandomSet( ownedPlusPlus );
                    }
                }
            }
        }

	return getRandomSet(cards);
}

/** A test of whether the array of cards contains at least one cashable set. */
bool Card::containsASet( vector<Card> cards ){
	if (cards.size() < 3)
		return false;
	// A player has a match if he has either three cards with the same symbol, or one of each symbol.
	// Wildcard's symbol is 3, it can be anything.

	// Cycle through the cards and count the number of each symbol:
	int symbolCount[4];
	for (int i = 0; i < cards.size(); i++)
		{
		symbolCount[ cards[i].getSymbol() ]++;
		}

	// See if they have three of a kind (3 counts for anything):
	for (int i = 0; i < 3; i++)
		{
		if ( symbolCount[i] + symbolCount[3] > 2 )
			{
			return true;
			}
		}

	// Now see if they have one of each (3 counts for anything):
	if ( ( min( symbolCount[0], 1 ) + min( symbolCount[1], 1 ) + min( symbolCount[2], 1 ) + symbolCount[3] ) > 2 )
		{
		return true;
		}

	// Otherwise, we have no set:
	return false;
	}

 vector<Card> Card::getRandomSet( vector<Card> cards ){
	vector<Card> toReturn;

	// Cycle through the cards and count the number of each symbol:
	int symbolCount[4];
	for (int i = 0; i < cards.size(); i++){
		symbolCount[ cards[i].getSymbol() ]++;
	}

	// See if they have three of a kind (3 counts for anything):
	for (int i = 0; i < 3; i++)
		{
		if ( symbolCount[i] + symbolCount[3] > 2 )
			{
			int setCounter = 0;
			// Then they have a set with symbol i, let's get it:
			for (int j = 0; setCounter != 3 && j < cards.size(); j++)
				{
				if ( cards[j].getSymbol() == i || cards[j].getSymbol() == 3 ){
					toReturn[ setCounter ] = cards[j];
					setCounter++;
				}
			}
			return toReturn;
		}
	}

	// OK, so they don't have three symbols of a kind. that means that they must have one of each.
	// Let's find them:
	for (int j = 0; j < cards.size(); j++)
		{
		if ( toReturn.size() < 1 && cards[j].getSymbol() == 0 )
			{
			toReturn[0] = cards[j];
			}
		else if (  toReturn.size() < 2 && cards[j].getSymbol() == 1 )
			{
			toReturn[1] = cards[j];
			}
		else if ( toReturn.size() < 3 && cards[j].getSymbol() == 2 )
			{
			toReturn[2] = cards[j];
			}
		else if ( cards[j].getSymbol() == 3 )
			{
			if ( symbolCount[0] == 0 &&  toReturn.size() < 1 )
				{
				toReturn[0] = cards[j];
				}
			else if ( symbolCount[1] == 0 &&  toReturn.size() < 2)
				{
				toReturn[1] = cards[j];
				}
			else if ( symbolCount[2] == 0 &&  toReturn.size() < 3 )
				{
				toReturn[2] = cards[j];
				}
			}
		}
	return toReturn;
	}