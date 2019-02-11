#include "RemotePlayerEnabler.h"
#include "SubEvents.h"
#include "MainEvent.h"
#include "NewEventHandling.h"
#include <random>

#define TX(x) (static_cast<void (Enabler::* )(SubtypeEvent *)>(&RemotePlayerEnabler::x))

RemotePlayerEnabler::RemotePlayerEnabler()
{
}

RemotePlayerEnabler::RemotePlayerEnabler(Networking * pkgSender_,EventsHandler * handler_)
{
	init();
	setPkgSender(pkgSender_);
	setHandler(handler_);
}


RemotePlayerEnabler::~RemotePlayerEnabler()
{
}

void RemotePlayerEnabler::localStarts()
{
	setWaitingMessage(string("Listo para empezar, jugador ") + localPlayer->getName() + " seleccione donde colocar su primer SETTLEMENT.");

	disableAll();
	enable(NET_SETTLEMENT, { TX(firstSettlement) });
	setDefaultRoutine(TX(genericDefault));
}

void RemotePlayerEnabler::remoteStarts()
{
	setWaitingMessage(string("Listo para empezar, el jugador ") + remotePlayer->getName() + " debe colocar su primer SETTLEMENT.");

	disableAll();
	enable(NET_SETTLEMENT, { TX(firstSettlement_) });
	setDefaultRoutine(TX(genericDefault));

}

void RemotePlayerEnabler::setUpForTurn()
{
	disableAll();
	enable(NET_DICES_ARE, { TX(checkDices) });
}

bool RemotePlayerEnabler::deleteCards(vector<ResourceType> descarte, Player * player_)
{
	for (auto carta : descarte)
	{
		if (!player_->useResource(carta)) // si no tiene el recurso error
		{
			return false;
		}
	}
	return true;
}

void RemotePlayerEnabler::enableRemoteActions()
{
	disableAllBut({NET_YEARS_OF_PLENTY,NET_ROAD_BUILDING,NET_MONOPOLY,NET_KNIGHT});
	enable(NET_OFFER_TRADE, {TX(evaluateOffer)});
	enable(NET_SETTLEMENT, {TX(checkRemoteSettlement)});
	enable(NET_CITY, {TX(checkRemoteCity)});
	enable(NET_ROAD, {TX(checkRemoteRoad)});
	enable(NET_BANK_TRADE, {TX(checkRemoteBankTrade)});
	enable(NET_DEV_CARDS, {TX(checkDevCards)});
	enable(NET_PASS, {TX(endTurn)});
}

void RemotePlayerEnabler::checkRemoteDevCards(SubtypeEvent * ev)
{
	if (playingWithDev)
	{
		if (remotePlayer->getDevCardAmount(DevCards::KNIGHT))
		{
			enable(NET_KNIGHT, { TX(remUsedKnight) });
		}
		else
		{
			disable(NET_KNIGHT);
		}

		if (remotePlayer->getDevCardAmount(DevCards::MONOPOLY))
		{
			enable(NET_MONOPOLY, { TX(remUsedMonopoly) });
		}
		else
		{
			disable(NET_MONOPOLY);
		}

		if (remotePlayer->getDevCardAmount(DevCards::YEARS_OF_PLENTY))
		{
			enable(NET_YEARS_OF_PLENTY, { TX(remUsedYoP) });
		}
		else
		{
			disable(NET_YEARS_OF_PLENTY);
		}

		if (remotePlayer->getDevCardAmount(DevCards::ROAD_BUILDING))
		{
			enable(NET_ROAD_BUILDING, { TX(remUsedRoadBuilding) });
		}
		else
		{
			disable(NET_ROAD_BUILDING);
		}

		if (remotePlayer->isThereDevCard(VICTORY_POINTS))
		{
			remotePlayer->useDevCard(VICTORY_POINTS);
			if (remotePlayer->hasWon(playingWithDev))
				enable(NET_I_WON, { TX(finDelJuego) });
		}
	}
}

void RemotePlayerEnabler::road1(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	RoadPkg* pkg = static_cast<RoadPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	if (localPlayer->checkRoadAvailability(position))
	{
		addRoadToRemote(position);
		disable(NET_ROAD);
		enable(NET_ROAD, { TX(road2) });
	}
	else
	{
		disableAll();
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("El rivala intenteto poner un road en una posicion incorrecta");
	}
}

void RemotePlayerEnabler::road2(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	RoadPkg* pkg = static_cast<RoadPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	if (localPlayer->checkRoadAvailability(position))
	{
		addRoadToRemote(position);
		disable(NET_ROAD);
		enableRemoteActions();
	}
	else
	{
		disableAll();
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("El rivala intenteto poner un road en una posicion incorrecta");
	}
}

void RemotePlayerEnabler::finDelJuego(SubtypeEvent * ev)
{
	disableAll();
	emitEvent(I_WON);
}

void RemotePlayerEnabler::init()
{
	setDefaultRoutine(TX(noAct));
	localPlayer = nullptr;
	remotePlayer = nullptr;
	board = nullptr;
	pkgSender = nullptr;
	
}

void RemotePlayerEnabler::end()
{
}

void RemotePlayerEnabler::noAct(SubtypeEvent * ev)
{
}

void RemotePlayerEnabler::firstSettlement(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	SettlementPkg* pkg = static_cast<SettlementPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	if (localPlayer->checkSettlementAvailability(position))
	{
		addSettlementToRemote(position);
		disable(NET_SETTLEMENT);
		enable(NET_ROAD, { TX(firstRoad) });
	}
	else
	{
		disableAll();
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("El rival intenteto poner su primer Settlement en una posicion incorrecta");
	}
}

void RemotePlayerEnabler::firstRoad(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	RoadPkg* pkg = static_cast<RoadPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	if (localPlayer->checkRoadAvailability(position))
	{
		addRoadToRemote(position);
		disable(NET_ROAD);
		enable(NET_SETTLEMENT, { TX(secondSettlement) });				// Leaving everything ready for next turn.
	}
	else
	{
		disableAll();
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("El rival intenteto poner su primer road en una posicion incorrecta");
	}
}

void RemotePlayerEnabler::secondSettlement(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	SettlementPkg* pkg = static_cast<SettlementPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	if (localPlayer->checkSettlementAvailability(position))
	{
		addSettlementToRemote(position);
		board->addSettlementToTokens(position, remotePlayer);
		disable(NET_SETTLEMENT);
		enable(NET_ROAD, { TX(secondRoad_) });
	}
	else
	{
		disableAll();
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("El rival intenteto poner su segundo Settlement en una posicion incorrecta");
	}
}

void RemotePlayerEnabler::secondRoad(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	RoadPkg* pkg = static_cast<RoadPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	if (localPlayer->checkRoadAvailability(position))
	{
		addRoadToRemote(position);
		disable(NET_ROAD);
		setUpForTurn();
		emitEvent(TURN_FINISHED);
	}
	else
	{
		disableAll();
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("El rival intenteto poner su segundo Road en una posicion incorrecta");
	}
}

void RemotePlayerEnabler::firstSettlement_(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	SettlementPkg* pkg = static_cast<SettlementPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	if (localPlayer->checkSettlementAvailability(position))
	{
		addSettlementToRemote(position);
		disable(NET_SETTLEMENT);
		enable(NET_ROAD, { TX(firstRoad_) });
	}
	else
	{
		disableAll();
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("El rival intenteto poner su primer Settlement en una posicion incorrecta");
	}
}

void RemotePlayerEnabler::firstRoad_(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	RoadPkg* pkg = static_cast<RoadPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	
	if (remotePlayer->checkRoadAvailability(position))
	{
		addRoadToRemote(position);
		disable(NET_ROAD);
		enable(NET_SETTLEMENT, { TX(secondSettlement_) });				// Leaving everything ready for next turn.
		emitEvent(TURN_FINISHED);
	}
	else
	{
		disableAll();
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("El rival intenteto poner su primer road en una posicion incorrecta");
	}

}

void RemotePlayerEnabler::secondSettlement_(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	SettlementPkg* pkg = static_cast<SettlementPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	if (localPlayer->checkSettlementAvailability(position))
	{
		addSettlementToRemote(position);
		board->addSettlementToTokens(position, remotePlayer);
		disable(NET_SETTLEMENT);
		enable(NET_ROAD, { TX(secondRoad_) });
	}
	else
	{
		disableAll();
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("El rival intenteto poner su segundo Settlement en una posicion incorrecta");
	}
}

void RemotePlayerEnabler::secondRoad_(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	RoadPkg* pkg = static_cast<RoadPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	if(localPlayer->checkRoadAvailability(position))
	{
		addRoadToRemote(position);
		disable(NET_ROAD);
		setUpForTurn();
	}
	else
	{
		disableAll();
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("El rival intenteto poner su segundo Road en una posicion incorrecta");
	}
	
}

void RemotePlayerEnabler::checkDices(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	DicePkg* pkg = static_cast<DicePkg*>(auxEv->getPackage());

	int rolled = pkg->getValue(false);
	rolled += pkg->getValue(true);

	board->assignResourcesForNum(rolled);

	disable(NET_DICES_ARE);
	
	if (rolled == 7)
	{
		if (localPlayer->totalResourcesAmount() >= 7)
		{
			enable(PLA_ROBBER_CARDS, { TX(SendsRobberCards) });
		}
		else
		{
			pkgSender->pushPackage(new package(headers::ACK));
			if (remotePlayer->totalResourcesAmount() >= 7)
			{
				enable(NET_ROBBER_CARDS, {TX(remoteLoseCards)});
			}
			else
			{
				enable(NET_ROBBER_MOVE, { TX(remoteMoveRobber) , TX(checkRemoteDevCards) });
			}
		}
	}
	else
	{
		pkgSender->pushPackage(new package(headers::ACK));
		checkRemoteDevCards(ev);
		enableRemoteActions();
	}
}

void RemotePlayerEnabler::SendsRobberCards(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	RobberCardsPkg* pkg = new RobberCardsPkg(*static_cast<RobberCardsPkg*>(auxEv->getPackage()));

	if (!deleteCards(pkg->getCards(),localPlayer)) // si no puedo eliminar todas las cartas
	{
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("Error al eliminar las cartas al mover el robber");
	}
	else
	{
		pkgSender->pushPackage(pkg); // si uedo eliminarlas envio el paquete
		disable(PLA_ROBBER_CARDS); // deshabilito este evento

		if (remotePlayer->totalResourcesAmount() >= 7) // si el otro tiene mas de 7 habilito la espera de esas cartas
		{
			enable(NET_ROBBER_CARDS, { TX(remoteLoseCards) });
		}
		else
		{
			enable(NET_ROBBER_MOVE, { TX(remoteMoveRobber) });
		}
	}
}

void RemotePlayerEnabler::remoteLoseCards(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	RobberCardsPkg* pkg = static_cast<RobberCardsPkg*>(auxEv->getPackage()) ;

	if(!deleteCards(pkg->getCards(), remotePlayer)) // si no puedo eliminar todas las cartas
	{
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("Error al eliminar las cartas del rival al mover el robber");
	}
	else
	{
		pkgSender->pushPackage(new package(headers::ACK));
		disable(NET_ROBBER_CARDS);
		enable(NET_ROBBER_MOVE, { TX(remoteMoveRobber) });
	}
}

void RemotePlayerEnabler::remoteMoveRobber(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	RobberMovePkg* pkg =new  RobberMovePkg(*static_cast<RobberMovePkg*>(auxEv->getPackage()));

	board->moveRobber(pkg->getPos());
	pkgSender->pushPackage(new package(headers::ACK));

	disable(NET_ROBBER_MOVE);
	enableRemoteActions();

}

void RemotePlayerEnabler::evaluateOffer(SubtypeEvent * ev)
{
	disableAllBut({ NET_KNIGHT,NET_YEARS_OF_PLENTY,NET_MONOPOLY,NET_ROAD_BUILDING });
	setErrMessage("");
	setWaitingMessage(""); // falta aclarar cual es la oferta ver como amarla para mostrarla!!! Acepta la oferta de: 

	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	OfferTradePkg* pkg = static_cast<OfferTradePkg*>(auxEv->getPackage());

	if (validateOffer(pkg))
	{
		pendingOffer = *pkg;													// Saving offer for response.
		map< ResourceType, char> oferta, pedido;								//armando el mensaje para mostrarle al jugador
		for (auto recurso : pendingOffer.getMyOnes())
		{
			oferta[recurso] += 1; // armo una lista con los recursos y cantidades
		}
		for (auto recurso : pendingOffer.getOpponentOnes())
		{
			pedido[recurso] += 1; // armo una lista con los recursos y cantidades
		}
		string mensaje("Acepta la oferta de: ");
		for (auto par : oferta)
		{
			mensaje += to_string(par.second);
			switch (par.first)
			{
			case BOSQUE : 
				mensaje += " de madera - ";
			case COLINAS :
				mensaje += " de ladrillo - ";
			case MONTA�AS :
				mensaje += " de piedra - ";
			case CAMPOS :
				mensaje += " de trigo - ";
			case PASTOS :
				mensaje += " de oveja - ";
			default:break;
			}
		}
		mensaje += " Por: ";
		for (auto par : pedido)
		{
			mensaje += to_string(par.second);
			switch (par.first)
			{
			case BOSQUE:
				mensaje += " de madera - ";
			case COLINAS:
				mensaje += " de ladrillo - ";
			case MONTA�AS:
				mensaje += " de piedra - ";
			case CAMPOS:
				mensaje += " de trigo - ";
			case PASTOS:
				mensaje += " de oveja - ";
			default:break;
			}
		}
		setWaitingMessage(mensaje); // cargo el mensaje con la oferta
		enable(PLA_YES, { TX(exchangeResources), TX(enableRemoteActions) });
		enable(PLA_NO, { TX(rejectOffer),TX(enableRemoteActions) });
	}
	else
	{
		setErrMessage("La oferta de trade ingresada es inv�lida. La misma debe constar de entre 1 y 9 recursos ofrecidos, y entre 1 y 9 pedidos.");
	}
}

void RemotePlayerEnabler::checkRemoteSettlement(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	SettlementPkg* pkg = static_cast<SettlementPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	if (localPlayer->checkSettlementAvailability(position))
	{
		remotePlayer->addToMySettlements(position);
		localPlayer->addToRivalsSettlements(position);
		board->addSettlementToTokens(position, remotePlayer);
		pkgSender->pushPackage(new package(headers::ACK));
		if (remotePlayer->hasWon(playingWithDev))
			enable(NET_I_WON, {TX(finDelJuego)});
	}
	else
	{
		emitEvent(ERR_IN_COM);
		setErrMessage("La coordenada donde se quiso ubicar el nuevo Settlement no est� disponible.");
	}
}

void RemotePlayerEnabler::checkRemoteRoad(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	RoadPkg* pkg = static_cast<RoadPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	if (localPlayer->checkRoadAvailability(position))
	{
		remotePlayer->addToMyRoads(position);
		localPlayer->addToRivalsRoads(position);
		board->addRoadToTokens(position, remotePlayer);
		pkgSender->pushPackage(new package(headers::ACK));
	}
	else
	{
		emitEvent(ERR_IN_COM);
		pkgSender->pushPackage(new package(headers::ERROR_));
		setErrMessage("La coordenada donde se quiso ubicar el nuevo Road no est� disponible.");
	}
}

void RemotePlayerEnabler::checkRemoteCity(SubtypeEvent * ev)
{
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	CityPkg* pkg = static_cast<CityPkg*>(auxEv->getPackage());
	string position = pkg->getPos();

	if (localPlayer->checkPromotionOfCity(position))
	{
		remotePlayer->promoteToMyCity(position);
		localPlayer->promoteToRivalsCity(position);
		board->addCityToTokens(position, remotePlayer);
		pkgSender->pushPackage(new package(headers::ACK));
		if (remotePlayer->hasWon(playingWithDev))
			enable(NET_I_WON, { TX(finDelJuego) });
	}
	else
	{
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("La coordenada donde se quiso promover la nueva City no es aceptada.");
	}
}

void RemotePlayerEnabler::checkRemoteBankTrade(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	BankTradePkg* pkg = static_cast<BankTradePkg*>(auxEv->getPackage());
	int amountOfResourcesPaid = pkg->getLength();
	bool isOk = false;

	switch (amountOfResourcesPaid)
	{
	case 4:
		isOk = true;
		break;
	case 3:
		if (remotePlayer->checkForAnyPort(board, PortType::_3x1)) { isOk = true; }
		break;
	case 2:
		switch (pkg->getResouceBougth())
		{
		case PortType::_2Lx1:
			if (remotePlayer->checkForAnyPort(board, PortType::_2Lx1)) { isOk = true; }
			break;
		case PortType::_2Ox1:
			if (remotePlayer->checkForAnyPort(board, PortType::_2Ox1)) { isOk = true; }
			break;
		case PortType::_2Mx1:
			if (remotePlayer->checkForAnyPort(board, PortType::_2Mx1)) { isOk = true; }
			break;
		case PortType::_2Px1:
			if (remotePlayer->checkForAnyPort(board, PortType::_2Px1)) { isOk = true; }
			break;
		case PortType::_2Tx1:
			if (remotePlayer->checkForAnyPort(board, PortType::_2Tx1)) { isOk = true; }
			break;
		}
		break;
	default:
		isOk = false; // hay algun error en el paquete
		break;
	}
	/* despues de fijarme si es posible la cuestien de los puertos, me fijo si tiene los recursos suficientes*/
	if (isOk)
	{
		for (ResourceType a : pkg->getResoucesPaid())
		{
			if (!(remotePlayer->useResource(a)))
			{
				isOk = false;
			}
		}
	}

	if (isOk)
	{
		remotePlayer->addResource(pkg->getResouceBougth(),1);
		pkgSender->pushPackage(new package(headers::ACK));
	}
	else
	{
		emitEvent(ERR_IN_COM);
		pkgSender->pushPackage(new package(headers::ERROR_));
		setErrMessage("Error al intentar concretar el Banck trade del rival");
	}
}

void RemotePlayerEnabler::checkDevCards(SubtypeEvent * ev)
{
	setErrMessage("");
	setWaitingMessage("");

	if (remotePlayer->checkResourcesForDevCard())
	{
		remotePlayer->getNewDevCard(board);
		pkgSender->pushPackage(new package(headers::ACK));
	}
	else
	{
		setErrMessage("Error, el rival no tinene suficientes recursos para comprar una Dev Card");
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
	}
}

void RemotePlayerEnabler::endTurn(SubtypeEvent * ev)
{
	disableAll();
	enable(NET_DICES_ARE, { TX(checkDices) });
	emitEvent(TURN_FINISHED);
}

void RemotePlayerEnabler::remUsedKnight(SubtypeEvent * ev)
{
	disableAll();
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	RobberMovePkg* pkg = static_cast<RobberMovePkg*>(auxEv->getPackage());
	
	random_device rd;
	mt19937_64 generator{ rd() };
	uniform_int_distribution<> dist{ 0,4}; // para tener un numero bien aleatorio (mejor que rand()%)
	ResourceType randCard;

	board->moveRobber(pkg->getPos());
	if (localPlayer->isThereSetOrCity(pkg->getPos()))
	{
		do{
			switch (dist(generator))
			{
			case 0: randCard = ResourceType::BOSQUE; break;
			case 1:randCard = ResourceType::CAMPOS; break;
			case 2:randCard = ResourceType::COLINAS; break;
			case 3:randCard = ResourceType::MONTA�AS; break;
			case 4:randCard = ResourceType::PASTOS; break;
			default:randCard = ResourceType::DESIERTO; break; // para que se sortee otro nuevo
			}
		} while (localPlayer->getResourceAmount(randCard) == 0);
		localPlayer->useResource(randCard);
		remotePlayer->addResource(randCard);
		pkgSender->pushPackage(new CardIsPkg(static_cast<char>(randCard)));
	}
	else
	{
		pkgSender->pushPackage(new package(headers::ACK));
	}
	enableRemoteActions();
}

void RemotePlayerEnabler::remUsedMonopoly(SubtypeEvent * ev)
{
	disableAll();
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	MonopolyPkg* pkg = static_cast<MonopolyPkg*>(auxEv->getPackage());

	pkgSender->pushPackage(new package(headers::ACK)); // respondo el pauqete
	ResourceType recurso = pkg->getResource();
	size_t amount = localPlayer->getResourceAmount(recurso); // busco la cantidad de recursos que el local tiene para agregarle al remoto
	
	remotePlayer->addResource(recurso, amount); // transfiero los recursos al oponenete
	localPlayer->useResource(recurso, amount); // saco los recuros
	remotePlayer->useDevCard(DevCards::MONOPOLY); // uso la carta
	enableRemoteActions();
}

void RemotePlayerEnabler::remUsedYoP(SubtypeEvent * ev)
{
	disableAll();
	setErrMessage("");
	setWaitingMessage("");
	SubEvents* auxEv = static_cast<SubEvents*>(ev);
	YearsOfPlentyPkg* pkg = static_cast<YearsOfPlentyPkg*>(auxEv->getPackage());

	pkgSender->pushPackage(new package(headers::ACK)); // respondo el pauqete
	ResourceType recurso = pkg->getResource(true);
	size_t amount = localPlayer->getResourceAmount(recurso); // busco la cantidad de recursos que el local tiene para agregarle al remoto

	remotePlayer->addResource(recurso, amount); // transfiero los recursos al oponenete
	localPlayer->useResource(recurso, amount); // saco los recuros

	recurso = pkg->getResource(false);
	amount = localPlayer->getResourceAmount(recurso); // busco la cantidad de recursos que el local tiene para agregarle al remoto

	remotePlayer->addResource(recurso, amount); // transfiero los recursos al oponenete
	localPlayer->useResource(recurso, amount); // saco los recuros

	remotePlayer->useDevCard(DevCards::YEARS_OF_PLENTY); // uso la carta
	enableRemoteActions();
}

void RemotePlayerEnabler::remUsedRoadBuilding(SubtypeEvent * ev)
{
	disableAll();
	pkgSender->pushPackage(new package(headers::ACK));
	enable(NET_ROAD, { TX(firstRoad) });
}

void RemotePlayerEnabler::enableRemoteActions(SubtypeEvent * ev)
{
	disableAllBut({ NET_KNIGHT,NET_YEARS_OF_PLENTY,NET_MONOPOLY,NET_ROAD_BUILDING }); 
	if (remotePlayer->totalResourcesAmount())
	{
		enable(NET_OFFER_TRADE, { TX(evaluateOffer) });
		enable(NET_BANK_TRADE, { TX(checkRemoteBankTrade) });
	}
	if (remotePlayer->checkSettlementResources())
	{
		enable(NET_SETTLEMENT, { TX(checkRemoteSettlement) });
	}
	if (remotePlayer->checkCityResources())
	{
		enable(NET_CITY, { TX(checkRemoteCity) });
	}
	if (remotePlayer->checkRoadResources())
	{
		enable(NET_ROAD, { TX(checkRemoteRoad) });
	}
	if (remotePlayer->checkResourcesForDevCard())
	{
		enable(NET_DEV_CARDS, { TX(checkDevCards) });
	}
	enable(NET_PASS, { TX(endTurn) });
}

void RemotePlayerEnabler::rejectOffer(SubtypeEvent * ev)
{
	pkgSender->pushPackage(new package(headers::NO));
}

void RemotePlayerEnabler::exchangeResources(SubtypeEvent * ev)
{
	pkgSender->pushPackage(new package(headers::YES));
	for (auto resource : pendingOffer.getMyOnes())
	{
		localPlayer->addResource(resource);					// Adding the resources coming from opponent.
		remotePlayer->useResource(resource);
	}
	for (auto resource : pendingOffer.getOpponentOnes())
	{
		remotePlayer->addResource(resource);					// Giving resources to opponent.
		localPlayer->useResource(resource);
	}
}

void RemotePlayerEnabler::genericDefault(SubtypeEvent * ev)
{
	emitEvent(ERR_IN_COM);
}

void RemotePlayerEnabler::emitEvent(EventTypes type)
{
	GenericEvent* ev = new MainEvents(type);
	handler->enqueueEvent(ev);
}

void RemotePlayerEnabler::emitSubEvent(EventTypes type, EventSubtypes subtype, package * pkg)
{
	GenericEvent* ev = new SubEvents(type, subtype, pkg);
	handler->enqueueEvent(ev);
}

void RemotePlayerEnabler::addSettlementToRemote(string position)
{
	if (remotePlayer->checkSettlementAvailability(position))
	{
		remotePlayer->addToMySettlements(position);
		localPlayer->addToRivalsSettlements(position);
		pkgSender->pushPackage(new package(headers::ACK));
	}
	else
	{
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("La posici�n donde se quiere colocar el Settlement es inv�lida.");
		return;
	}

	if (!board->addSettlementToTokens(position, remotePlayer))
	{
		setErrMessage("El casillero del tablero donde se quiere agregar el Settlement est� lleno.");
		return;
	}
}

void RemotePlayerEnabler::addRoadToRemote(string position)
{
	if (remotePlayer->checkRoadAvailability(position))
	{
		remotePlayer->addToMyRoads(position);
		localPlayer->addToRivalsRoads(position);
		pkgSender->pushPackage(new package(headers::ACK));
	}
	else
	{
		pkgSender->pushPackage(new package(headers::ERROR_));
		emitEvent(ERR_IN_COM);
		setErrMessage("La posici�n donde se quiere colocar el Road es inv�lida.");
		return;
	}

	if (!board->addRoadToTokens(position, remotePlayer))
	{
		setErrMessage("El casillero del tablero donde se quiere agregar el Road est� lleno.");
		return;
	}
}
