#pragma once
#include "PlayerEnabler.h"
#include <string>
#include <cstdint>
#include "Player.h"
#include "Board.h"
#include "package.h"

using namespace std;

class LocalPlayerEnabler :
	public PlayerEnabler
{
public:
	LocalPlayerEnabler();
	LocalPlayerEnabler(Networking* pkgSender_, PlayerEnabler* remoteEnabler_);
	virtual ~LocalPlayerEnabler();

	// Enables events for the user to select when local starts.
	void localStarts(string nameLocal, string nameRemote);
	// Enables events when remote starts.
	void remoteStarts(string nameLocal, string nameRemote);

	void setRemoteEnabler(PlayerEnabler* remoteEnabler_);

private:
	void init();
	void end();

	// To be used to set a common board, localPlayer and remotePlayer.
	PlayerEnabler* remoteEnabler;

	/* ROUTINES TO BE EXECTUED IN CYCLE */
	void noAct(SubtypeEvent* ev);
	void firstSettlement(SubtypeEvent* ev);
	void firstRoad(SubtypeEvent* ev);
	void secondSettlement(SubtypeEvent* ev);
	void secondRoad(SubtypeEvent* ev);
	void checkDices(SubtypeEvent* ev);
	void remoteSendsRobberCards(SubtypeEvent* ev);
	void checkLocalResources(SubtypeEvent* ev);
	void enablePlayerActions(SubtypeEvent* ev);
	void discardLocalResources(SubtypeEvent* ev);
	void moveRobber(SubtypeEvent* ev);
	void checkOffer(SubtypeEvent* ev);
	void checkSettlement(SubtypeEvent* ev);
	void checkRoad(SubtypeEvent* ev);
	void checkCity(SubtypeEvent* ev);
	void checkBankTrade(SubtypeEvent* ev);
	void drawDevCard(SubtypeEvent* ev);
	void useKnight(SubtypeEvent* ev);
	void useMonopoly(SubtypeEvent* ev);
	void useYearsOfPlenty(SubtypeEvent* ev);
	void useRoadBuilding(SubtypeEvent* ev);

	/* DEFAULT ROUTINES */
	void genericDefault(SubtypeEvent* ev);

	/* AUXILIARY METHODS */
	void emitEvent(EventTypes type);
	void emitSubEvent(EventTypes type, EventSubtypes subtype, package* pkg=nullptr);
	void getResourceFromSettlement(string position, Player* who);
	void checkDevCards();

	// Adds settlement to localPlayer and board from both Enablers
	void addSettlementToLocal(string position);
	// Adds road to localPlayer and board from both Enablers
	void addRoadToLocal(string position);
	// Enables events for beginning of turn.
	void setUpForTurn();
};

