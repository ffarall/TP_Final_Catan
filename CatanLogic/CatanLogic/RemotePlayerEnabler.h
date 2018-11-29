#pragma once
#include "PlayerEnabler.h"
#include <string>
#include <cstdint>
#include "Player.h"
#include "Board.h"

using namespace std;

class RemotePlayerEnabler :
	public PlayerEnabler
{
public:
	RemotePlayerEnabler();
	virtual ~RemotePlayerEnabler();

	void init();
	void end();

	// Enables events for when local starts.
	void localStarts();
	// Enables events when remote starts.
	void remoteStarts();
	// Enables events for beginning of turn.
	void setUpForTurn();

private:
	// To be used to set a common board, localPlayer and remotePlayer.
	PlayerEnabler* localEnabler;

	/* ROUTINES TO BE EXECTUED IN CYCLE */
};
