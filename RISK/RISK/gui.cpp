#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "gui.h"
#include "engine.h"

using namespace std;

GUI::GUI(){

}

GUI::GUI(string mapFile){

	//Base Screen Proportions
	SCREEN_WIDTH = 1000;
	SCREEN_HEIGHT = 792;
	SCREEN_BPP = 32;

	//Dashboard Attributes
	CAPTION_X = 625;
	CAPTION_Y = 10;

	//The button states in the sprite sheet
	CLIP_MOUSEOVER = 0;
	CLIP_MOUSEOUT = 1;
	CLIP_MOUSEDOWN = 2;
	CLIP_MOUSEUP = 3;

	//Button Attributes
	BUTTON_TEXT_OFFSET_X = 30;
	BUTTON_TEXT_OFFSET_Y = 20;
	BUTTON_W = 125;
	BUTTON_H = 60;
	FORTIFY_BUTTON_X = 20;
	FORTIFY_BUTTON_Y = 610;
	DONE_BUTTON_X = 20;
	DONE_BUTTON_Y = 680;

	//Images surfaces used in GUI
	iconList = new SDL_Surface*[6];
	iconPlayer1 = NULL;
	iconPlayer2 = NULL;
	buttonSheet = NULL;
	background = NULL;
	screen = NULL;
	newScreen = NULL;

	//The fonts used int GUI
	fontSmall = NULL;
	fontNormal = NULL;
	fontHuge = NULL;

	//Start-up the GUI!
	initGUI(mapFile);

	//Exit criteria
	quit=false;

}

GUI::~GUI(){

	cleanUp();

}

bool GUI::observeEvent(){

	return(SDL_PollEvent( &event ));

}

bool GUI::quitGame(){

	return(event.type == SDL_QUIT );

}

string GUI::getCommand(){

	//Base command is "NA" for "Not Applicable"
	string command = "NA";

	//Have all territories handle events
	for(int i=0;i<Nterritories;i++){
		territories[i]->handleEvents(event);
		if(territories[i]->isCommandIssued()){
			command = territories[i]->getCommand();
		}
	}

	//Have all buttons handle events
	fortify->handleEvents(event);
	if(fortify->isCommandIssued()){
		command = fortify->getCommand();
	}

	done->handleEvents(event);
	if(done->isCommandIssued()){
		command = done->getCommand();
	}

	return(command);
}


//Base GameState Display...just shows active Territory
void GUI::displayGameState(){

	//Set base text color to "white"
	SDL_Color textColor = {255,255,255};
	
	//Apply the background to the screen
	applySurface(0, 0, background, screen );

	//Display the active territory name in the dashboard
	for(int i=0;i<Nterritories;i++){
	
		//If mouse is within Territory box
		if(territories[i]->show()){
		
			//Display Name
			applySurface(
				territories[i]->getX(), 
				territories[i]->getY()-30, 
				territories[i]->getSurface(),
				screen);

			//Clean message
			territories[i]->clearSurface();
	
		}
	
	}

	//Update the visible screen
	SDL_Flip( screen );
	
}

//Full GameState Display...shows active Territory and Unit Quantities and Ownership
void GUI::displayGameState(int* ownerArray, int* armyCountArray, int currentPlayer, int gamePhase, int subGamePhase,
	string terrTarget, string terrBase, bool victory){


	//Output Graphics
	SDL_Color textColor = {255,255,255};

	//Apply the background to the newScreen
	applySurface(0, 0, background, newScreen );
		
	//Display the board state
	for(int i=0;i<Nterritories;i++){
		
		int owner = ownerArray[i];
		if(owner>=0){
		
			//Display the icon
			int plotX = territories[i]->getX();
			int plotY = territories[i]->getY();
			int offsetX = (territories[i]->getW()/2)-10;
			int offsetY = (territories[i]->getH()/2)-10;
			/*applySurface(plotX+offsetX,plotY+offsetY,iconList[owner],newScreen);
				//The number of units will render above the icon
				*/
			int plotX2 = territories[i]->getLocationX();
			int plotY2 = territories[i]->getLocationY();
			applySurface(plotX2, plotY2, territories[i]->getColor(owner), newScreen);
			
			char bufferN[4];
			char* gridN = itoa(armyCountArray[i],bufferN,10);
			char above[4];
			strcpy(above,gridN);
			SDL_Surface* unitsMessage = TTF_RenderText_Solid(fontSmall, above, textColor);
			applySurface(plotX+offsetX, plotY+offsetY-15, unitsMessage, newScreen);
			}
	}

	//Display the active territory name in the dashboard
	for(int i=0;i<Nterritories;i++){
		if(territories[i]->show()){

			//Display Name
			applySurface(
				territories[i]->getX(), 
				territories[i]->getY()-30, 
				territories[i]->getSurface(),
				newScreen);
	
			//Clean message
			territories[i]->clearSurface();

		}
	}

	//Display buttons
	applySurface(fortify->getX(),fortify->getY(),buttonSheet,newScreen,&clips[fortify->getClip()]);	
	applySurface(fortify->getX()+30,fortify->getY()+20,fortify->getSurface(),newScreen);
	applySurface(done->getX(),done->getY(),buttonSheet,newScreen,&clips[done->getClip()]);	
	applySurface(done->getX()+30,done->getY()+20,done->getSurface(),newScreen);


	//Display current player information to dashboard
	string iconMsg = "Current Icon: ";
	icon = TTF_RenderText_Solid( fontNormal, iconMsg.c_str(), textColor );
	applySurface ( CAPTION_X, CAPTION_Y+30, icon, newScreen);
	applySurface ( CAPTION_X+140, CAPTION_Y+30, iconList[currentPlayer], newScreen);

	//Display the current game phase in the dashboard
	string phaseID = "Current Phase: ";

	if(gamePhase==0){
		phaseID += "Initial Placement";
	}else{
		switch(subGamePhase){
		case 0:
			phaseID += "Cards";
			break;
		case 1:
			phaseID += "Placement";
			break;
		case 2:
			phaseID += "Attack";
			break;
		case 3:
			phaseID += "Fortify";
			break;
		}
	}
	phase = TTF_RenderText_Solid( fontNormal, phaseID.c_str(), textColor );
	applySurface ( CAPTION_X, CAPTION_Y+60, phase, newScreen );

	//Display Attack information in the dashboard
	if((subGamePhase==1) || (subGamePhase==2)){
		string targetString;
		string baseString;
		if(!(terrTarget.compare("NA"))){
			targetString = "Click on a target territory.";
			baseString = "";
		}else{
			targetString = "Target: " + terrTarget;	
			if(!(terrBase.compare("NA"))){
					baseString = "Click on a base territory.";
			}else{
				baseString = "Base: " + terrBase;
			}
		}
		target = TTF_RenderText_Solid( fontNormal, targetString.c_str(), textColor );
		base = TTF_RenderText_Solid( fontNormal, baseString.c_str(), textColor);

		applySurface ( CAPTION_X-350, CAPTION_Y+30, base, newScreen );					
		applySurface ( CAPTION_X-350, CAPTION_Y+60, target, newScreen );
	}

	if(victory){
		vbanner = TTF_RenderText_Solid( fontHuge, "Victory!", textColor );
		applySurface ( CAPTION_X-600, CAPTION_Y+30, vbanner, newScreen );		
	}

	//Update the visible newScreen
	screen = newScreen;
	SDL_Flip( screen );

	//Clean Up
	delete [] ownerArray;
	delete [] armyCountArray;


}

//Initialize SDL Components
void GUI::initGUI(string mapFile){

	//Initialize all SDL subsystems
    SDL_Init( SDL_INIT_EVERYTHING );	//Initialize SDL subsystems
    
	TTF_Init();	

	//Construct Basic GUI components
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
	newScreen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

	//Open fonts
	fontSmall = TTF_OpenFont ("./fonts/Caligula.ttf", 16);	//Small-size font for the board
	fontNormal = TTF_OpenFont("./fonts/Caligula.ttf", 24);	//Large-size font for the buttons/caption
	fontHuge = TTF_OpenFont( "./fonts/Caligula.ttf", 60);	//Huge font for victory!

	//Load images
	iconList[0] = loadImage( "./images/PLAYER1.png", 255);
    iconList[1] = loadImage( "./images/PLAYER2.png", 255);
	iconList[2] = loadImage( "./images/PLAYER3.png", 255);
    iconList[3] = loadImage( "./images/PLAYER4.png", 255);
	iconList[4] = loadImage( "./images/PLAYER5.png", 255);
    iconList[5] = loadImage( "./images/PLAYER6.png", 255);

    iconPlayer1 = loadImage( "./images/short_sword.png", 255);
    iconPlayer2 = loadImage( "./images/crossed_swords.png", 255);
	background = loadImage( "./images/risk_board_gray.png", 0); 
	buttonSheet = loadImage("./images/button_sprite_small.png", 255);
    //Clip the sprite sheet
    clips[ CLIP_MOUSEOVER ].x = 0;
    clips[ CLIP_MOUSEOVER ].y = 0;
    clips[ CLIP_MOUSEOVER ].w = BUTTON_W;
    clips[ CLIP_MOUSEOVER ].h = BUTTON_H;

    clips[ CLIP_MOUSEOUT ].x = BUTTON_W;
    clips[ CLIP_MOUSEOUT ].y = 0;
    clips[ CLIP_MOUSEOUT ].w = BUTTON_W;
    clips[ CLIP_MOUSEOUT ].h = BUTTON_H;

    clips[ CLIP_MOUSEDOWN ].x = 0;
    clips[ CLIP_MOUSEDOWN ].y = BUTTON_H+1;
    clips[ CLIP_MOUSEDOWN ].w = BUTTON_W;
    clips[ CLIP_MOUSEDOWN ].h = BUTTON_H;

    clips[ CLIP_MOUSEUP ].x = BUTTON_W;
    clips[ CLIP_MOUSEUP ].y = BUTTON_H+1;
    clips[ CLIP_MOUSEUP ].w = BUTTON_W;
    clips[ CLIP_MOUSEUP ].h = BUTTON_H;

	//Set the window caption
    SDL_WM_SetCaption( "Risk", NULL );

	//Load Territory Elements
	loadTerritories(mapFile);

	//Construct Buttons
	fortify = new Button("Fortify",fontNormal,FORTIFY_BUTTON_X,
											 FORTIFY_BUTTON_Y,
											 BUTTON_W,
											 BUTTON_H);
	done = new Button("Done",fontNormal,DONE_BUTTON_X,
											 DONE_BUTTON_Y,
											 BUTTON_W,
											 BUTTON_H);

}

// For debugging map data
SDL_Surface* GUI::renderPositionMessage(int _r, int _g, int _b, int x, int y){
	
	char bufferX[4];
	char bufferY[4];
	SDL_Color textColor = {_r,_g,_b};
	
	char* gridX = itoa(x,bufferX,10);
	char* gridY = itoa(y,bufferY,10);

	char msg[8];
	strcpy(msg,gridX);
	strcat(msg,",");
	strcat(msg,gridY);
	SDL_Surface* coordMessage = TTF_RenderText_Solid( fontSmall, msg, textColor );

	return(coordMessage);
}


void GUI::cleanUp(){

	//Free the graphics
	SDL_FreeSurface( iconPlayer1 );
	SDL_FreeSurface( iconPlayer2 );
	SDL_FreeSurface( buttonSheet );
	SDL_FreeSurface( background ); 
	SDL_FreeSurface( screen );
	SDL_FreeSurface( newScreen );
    
	//Close the font that was used
    TTF_CloseFont( fontNormal );
	TTF_CloseFont( fontSmall );

    //Quit SDL
    SDL_Quit();

	//Quit SDL_ttf
    TTF_Quit();

}


SDL_Surface* GUI::loadImage(string filename, int mapkey ) 
{

	//The image that's loaded
    SDL_Surface* loadedImage = NULL;
    
    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;
    
    //Load the image
    loadedImage = IMG_Load( filename.c_str() );
    
    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );
        
        //Free the old image
        SDL_FreeSurface( loadedImage );


		//If the image was optimized just fine
        if( optimizedImage != NULL )
        {
            //Map the color key
            Uint32 colorkey = SDL_MapRGB( optimizedImage->format, mapkey, mapkey, mapkey );
 
			//Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
        }
	}
	
	//Return the optimized image
    return optimizedImage;

}

void GUI::loadTerritories(string mapFile){

	cout << "LOAD TERRITORIES" << endl;

	fstream inf;
	inf.open(mapFile.c_str(),ios::in);
	 
	Nterritories = 0;
	inf >> Nterritories;

	territories = new ActiveTerritory*[Nterritories];

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

		//Build continent name
		inf >> Nname;
		continent = "";
		for(int j=0;j<Nname;j++){
			inf >> tmpName;
			continent+=tmpName;
		}
		
		inf >> Nneighbors;
		for(int j=0;j<Nneighbors;j++){
			inf >> Nname;
			neighbor = "";
			for(int k=0;k<Nname;k++){
				inf >> tmpName;
				neighbor+=tmpName;
			}
		}
		int x, y, w, h, x2, y2;
 		inf >> x >> y >> w >> h;
		inf >> x2 >> y2;

		
 		cout << x << y << w << h << endl;

		territories[i] = new ActiveTerritory(name,fontNormal,x,y,w,h,x2,y2);

	
	for(int j=0;j<6;j++){
		string tmpFile = "";
		string num0 = "0";
		string num1 = "1";
		string num2 = "2";
		string num3 = "3";
		string num4 = "4";
		string num5 = "5";
		string file = "./images/";
		string end = ".png";
			tmpFile += file;
			tmpFile += name;
			if(j==0)
				tmpFile += num0;
			if(j==1)
				tmpFile += num1;
			if(j==2)
				tmpFile += num2;
			if(j==3)
				tmpFile += num3;
			if(j==4)
				tmpFile += num4;
			if(j==5)
				tmpFile += num5;
			tmpFile += end;
			
			territories[i]->setImage(loadImage(tmpFile, 255), j);
			}
		}
	}



void GUI::applySurface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

/*
********************************************************
********************************************************
********************************************************
*/

ActiveTerritory::ActiveTerritory(string name, TTF_Font* font, int x, int y, int w, int h, int x2, int y2 ){

    terrName = name;
	terrMessageFont = font;
	terrMessageColor.r = 255;
	terrMessageColor.g = 255;
	terrMessageColor.b = 255;
	territoryColor = new SDL_Surface*[6];

    box.x = x;
    box.y = y;
    box.w = w;
    box.h = h;
	locationX = x2;
	locationY = y2;

	command = "NA";
	commandIssued = false;
	mouseOver = false;
}


void ActiveTerritory::handleEvents(SDL_Event event){

    //The mouse offsets
    int x = 0, y = 0;

    //If the mouse moved
    if( event.type == SDL_MOUSEMOTION ){
		
        //Get the mouse offsets
        x = event.motion.x;
        y = event.motion.y;

        //If the mouse is over the button
        if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) ){

			mouseOver = true;
			command = "NA";
			commandIssued = false;
        }


    }

    //If a mouse button was pressed
    if( event.type == SDL_MOUSEBUTTONDOWN ){

        //If the left mouse button was pressed
        if( event.button.button == SDL_BUTTON_LEFT ){

            //Get the mouse offsets
            x = event.button.x;
            y = event.button.y;

            //If the mouse is over the button
            if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) ){				
	
				//Set command
				mouseOver = true;
				command = terrName;
				commandIssued = true;

            }
        }
    }

    //If a mouse button was released
    if( event.type == SDL_MOUSEBUTTONUP ){

        //Get the mouse offsets
        x = event.button.x;
        y = event.button.y;

		//If the mouse is over the button
		if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) ){
						
			mouseOver = true;
			command = "NA";
			commandIssued = false;

        }
    }
}

bool ActiveTerritory::isCommandIssued(){
	return(commandIssued);
}

string ActiveTerritory::getCommand(){
	return(command);
}


bool ActiveTerritory::show(){

	bool result = mouseOver;

	if(result){
		terrMessage = TTF_RenderText_Solid(terrMessageFont, terrName.c_str(), terrMessageColor);
	}

	mouseOver = false;
	return(result);

}

int ActiveTerritory::getX(){
	return(box.x);
}

int ActiveTerritory::getY(){
	return(box.y);
}

int ActiveTerritory::getW(){
	return(box.w);
}

int ActiveTerritory::getH(){
	return(box.h);
}

int ActiveTerritory::getLocationX(){
	return(locationX);
}

int ActiveTerritory::getLocationY(){
	return(locationY);
}

void ActiveTerritory::setImage(SDL_Surface* _image, int player){
	if(player == 0) 
		territoryColor[0] = _image;
	if(player == 1)
		territoryColor[1] = _image;
	if(player == 2) 
		territoryColor[2] = _image;
	if(player == 3)
		territoryColor[3] = _image;
	if(player == 4) 
		territoryColor[4] = _image;
	if(player == 5)
		territoryColor[5] = _image;
}

SDL_Surface* ActiveTerritory::getColor(int _owner){
	return(territoryColor[_owner]);
}

string ActiveTerritory::getName(){
	return(terrName);
}

SDL_Surface* ActiveTerritory::getSurface(){
	return(terrMessage);
}

void ActiveTerritory::clearSurface(){
	terrMessage = NULL;
}



/*
********************************************************
********************************************************
********************************************************
*/

Button::Button(string name, TTF_Font* font, int x, int y, int w, int h ){

	//The button states in the sprite sheet
	CLIP_MOUSEOVER = 0;
	CLIP_MOUSEOUT = 1;
	CLIP_MOUSEDOWN = 2;
	CLIP_MOUSEUP = 3;

    buttonName = name;
	buttonMessageFont = font;
	buttonMessageColor.r = 0;
	buttonMessageColor.g = 0;
	buttonMessageColor.b = 0;

	buttonMessage = TTF_RenderText_Solid(buttonMessageFont, buttonName.c_str(), buttonMessageColor);

    box.x = x;
    box.y = y;
    box.w = w;
    box.h = h;

	command = "NA";
	commandIssued = false;
	mouseOver = false;

    clip = CLIP_MOUSEOUT;

}



void Button::handleEvents(SDL_Event event){

    //The mouse offsets
    int x = 0, y = 0;

    //If the mouse moved
    if( event.type == SDL_MOUSEMOTION ){
		
        //Get the mouse offsets
        x = event.motion.x;
        y = event.motion.y;

        //If the mouse is over the button
        if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) ){

			mouseOver = true;
			command = "NA";
			commandIssued = false;
			clip = CLIP_MOUSEOVER;

		}else{
			clip = CLIP_MOUSEOUT;
		}


    }

    //If a mouse button was pressed
    if( event.type == SDL_MOUSEBUTTONDOWN ){

        //If the left mouse button was pressed
        if( event.button.button == SDL_BUTTON_LEFT ){

            //Get the mouse offsets
            x = event.button.x;
            y = event.button.y;

            //If the mouse is over the button
            if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) ){				
	
				//Set command
				mouseOver = true;
				command = buttonName;
				commandIssued = true;
				clip = CLIP_MOUSEDOWN;

            }
        }
    }

    //If a mouse button was released
    if( event.type == SDL_MOUSEBUTTONUP ){

        //Get the mouse offsets
        x = event.button.x;
        y = event.button.y;

		//If the mouse is over the button
		if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) ){
						
			mouseOver = true;
			command = "NA";
			commandIssued = false;
			clip = CLIP_MOUSEOVER;

        }


    }
}

bool Button::isCommandIssued(){
	return(commandIssued);
}

string Button::getCommand(){
	return(command);
}

int Button::getX(){
	return(box.x);
}

int Button::getY(){
	return(box.y);
}

int Button::getW(){
	return(box.w);
}

int Button::getH(){
	return(box.h);
}

string Button::getName(){
	return(buttonName);
}

SDL_Surface* Button::getSurface(){
	return(buttonMessage);
}

int Button::getClip(){
	return(clip);
}

void Button::clearSurface(){
	buttonMessage = NULL;
}
