#pragma once
#include"ObserverAndController.h"
#include"allegro5/allegro.h"
#include"allegro5/allegro_font.h"
#include"GutenbergsPressAllegro.h"
#include"package.h"
#include<iostream>
#include <functional>

typedef std::function<GUIEnablerEvent(void)> ParseAction;
typedef std::function<void(MovableType*)> UpdateAction;

using uint = unsigned int;
class Button :public BasicController
{
public:
	Button(GutenbergsPressAllegro* press_, EventsHandler* handler_, uint xPos, uint Ypos,uint height, uint width, std::string label, std::string imagePath, std::string fontPath, int fontSize);

	bool clickIn(uint x_, uint y_);

	void toggleButton();
	bool isPressed();
	virtual void enable();
	virtual void disable();
	bool isEnabled();
	void setXPos(uint pos);
	void setYpos(uint pos);
	uint getXPos();
	uint getYPos();
	ALLEGRO_BITMAP* getBitmap();	
	bool setBitmap(std::string imagePath);
	bool setFont(std::string fontPath, int fontSize);

	void addUtility(const ParseAction& callback_);
	void addUpdate(const UpdateAction& callback_);

	virtual void update();

	virtual GUIEnablerEvent parseMouseDownEvent(uint32_t x, uint32_t y);
	virtual GUIEnablerEvent parseMouseUpEvent(uint32_t x, uint32_t y);
	virtual GUIEnablerEvent parseTimerEvent();

	package * getPackage();
	void setPackage(package * data);

private:
	GutenbergsPressAllegro* press;
	MovableType* type;
	uint buttonXPos;
	uint buttonYPos;
	uint buttonHeight;
	uint buttonWidth;
	std::string buttonText;
	bool buttonPressed;
	bool buttonEnabled;
	ALLEGRO_BITMAP* buttonBitmap;
	ALLEGRO_FONT* buttonFont;
	GutenbergsPressAllegro* press;
	package * info;

	UpdateAction updateCallback;
	ParseAction parseCallback;
};

/*

{  // como usar las funciones lambda

	Button a(...),b(...),c(...);

	a.turnUseful([&b]() // entre corchetes con el & le paso los botones, controles, etc, que se vallan a habilitar o desabilitar cuando se clikea el boton 
				{  	    // entre llaves el callback que se tiene que ejecutar (tiene que tener un retrun GUIEnablerevent)
					b.toggleAvaliability();
					return ...;
				}
				);

}

*/