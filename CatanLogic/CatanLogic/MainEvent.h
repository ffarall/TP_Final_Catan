#pragma once
#include "NewEventHandling.h"
#include "SubEvents.h"

enum MainTypes : EventTypes { NETWORK, PLAY_AGAIN, GAME_OVER, START_GAME, PLAYER_ACTION, QUIT, TICKS, LOCAL_STARTS, REMOTE_START, TURN_FINISHED, I_WON, TIME_OUT, ERR_IN_COM };

class MainEvents : GenericEvent
{
public:
	MainEvents() {}
	MainEvents(EventTypes tipo_) { setEvent(tipo_); }
	~MainEvents() {}
	EventTypes getType() { return type; }
	void setEvent(EventTypes tipo_) { type = static_cast<MainTypes>(tipo_); }
protected:
	MainTypes type;
};
