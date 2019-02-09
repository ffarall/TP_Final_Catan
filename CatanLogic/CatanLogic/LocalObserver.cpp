#include "LocalObserver.h"
#include "PosDef.h"

#define LADRILLO "ladrillo.png"
#define PASTO "pasto.png"
#define PIEDRA "piedra.png"
#define TRONCO "tronco.png"
#define LANA "lana.png"
#define ROAD "road.png"
#define CITY "city.png"
#define SETTLE "setllement.png"
#define ICONO "LocalPla.png"
#define FONT "catanFont.otf"
#define LROAD "LongestRoad.png"
#define LARMY "LargestArmy.png"

#define CANCEL "cruz.png"
#define ACCEPT "tick.png"
#define ROAD_BUILDING "RoadBuilding.png"
#define VICTORY_POINT "VictoryPoint.png"
#define Y_OF_PLENTY "YofP.png"
#define MONOPOLY "Monopoly.png"
#define KNIGHT "Knight.png"
#define ROBBER "robber.png"

#define FONT_SIZE 10 // ver 
#define BOARD_POS_X 310
#define BOARD_POS_Y 88

#define D_ALTO 700
#define D_ANCHO 1200

LocalObserver::LocalObserver(EventsHandler* handler_, GutenbergsPressAllegro * printer, Player * local,LocalPlayerEnabler * playerEn) : BasicController(handler_), toDraw("mapaFinal.png")
{
	working = true;
	localPlayer = local;
	localEnabler = playerEn;
	impresora = printer;
		
	dibujo[ICONO] = al_load_bitmap(ICONO);
	dibujo[ROAD] = al_load_bitmap(ROAD);
	dibujo[CITY] = al_load_bitmap(CITY);
	dibujo[SETTLE] = al_load_bitmap(SETTLE);
	dibujo[LROAD] = al_load_bitmap(LROAD);
	dibujo[LARMY] = al_load_bitmap(LARMY);
	dibujo[CANCEL] = al_load_bitmap(CANCEL);
	dibujo[ACCEPT] = al_load_bitmap(ACCEPT);
	dibujo[VICTORY_POINT] = al_load_bitmap(VICTORY_POINT);
	dibujo[ROAD_BUILDING] = al_load_bitmap(ROAD_BUILDING);
	dibujo[MONOPOLY] = al_load_bitmap(MONOPOLY);
	dibujo[Y_OF_PLENTY] = al_load_bitmap(Y_OF_PLENTY);
	dibujo[KNIGHT] = al_load_bitmap(KNIGHT);
	dibujo[ROBBER] = al_load_bitmap(ROBBER);

	for (auto each : dibujo) { if (each.second == NULL) { working = false; } }
	if (!working)
	{
		for (auto each : dibujo)
		{
			if (each.second != NULL)
			{
				al_destroy_bitmap(each.second);
			}
		}
	}

	fuente = al_load_font(FONT, FONT_SIZE, 0);

	if (fuente == NULL)
	{
		working = false;
		al_destroy_font(fuente); 
	}

	cartasfotos[COLINAS] = al_load_bitmap(LADRILLO);
	cartasfotos[PASTOS] = al_load_bitmap(PASTO);
	cartasfotos[CAMPOS] = al_load_bitmap(LANA);
	cartasfotos[BOSQUE] = al_load_bitmap(TRONCO);
	cartasfotos[MONTA�AS] = al_load_bitmap(PIEDRA);
	for (auto a : cartasfotos) { if (a.second == NULL) { working = false; } }
	if (!working) 
	{
		for (auto a : cartasfotos) 
		{
			if (a.second != NULL) 
			{ 
				al_destroy_bitmap(a.second); 
			}
		}
	}

	pair<int, MovableType *> temp(0, NULL);
	cartas[MONTA�AS] = temp;
	cartas[PASTOS] = temp;
	cartas[COLINAS] = temp;
	cartas[BOSQUE] = temp;
	cartas[CAMPOS] = temp;
	
}


LocalObserver::~LocalObserver()
{
	if (working)
	{
		for (auto each : dibujo)
		{
			if (each.second != NULL) { al_destroy_bitmap(each.second); }
		}
		if (fuente != NULL) { al_destroy_font(fuente); }
	}
}

void LocalObserver::update()
{
	bool anyChange = false;
	map<string, bool> buildings;

	if (sellos[ICONO] == NULL)
	{
		sellos[ICONO] = impresora->createType(dibujo[ICONO], al_map_rgb(255, 255, 255),
			D_ANCHO * 0.05, D_ALTO * 0.05
		);
		impresora->addType(sellos[ICONO]);
	}

	for (auto ciudad : localPlayer->getMyCities())
	{
		buildings[ciudad] = true;
	}
	
	for (auto settlement : localPlayer->getMySettlements())
	{
		buildings[settlement] = false;
	}
	
	for (auto a : buildings) // primero me fijo que esten bien todos los MovabeType de city o settlements
	{
		ALLEGRO_BITMAP *  temp = a.second ? dibujo[CITY] : dibujo[SETTLE];
		auto itr = sellos.find(a.first);
		if (itr == sellos.end())
		{
			ALLEGRO_BITMAP *  temp = a.second ? dibujo[CITY] : dibujo[SETTLE];
			pair<unsigned int, unsigned int> pos = toDraw.getPositioningForVertex(a.first);
			sellos[a.first] = impresora->createType(temp,
				al_map_rgba_f(1.0, 0.0, 0.0, 1.0), pos.first+BOARD_POS_X, pos.second+BOARD_POS_Y,
				al_get_bitmap_width(temp) / 2, al_get_bitmap_height(temp) / 2
			);
			impresora->addType(sellos[a.first]);
			anyChange = true;
		}
		else if (itr->second->getBitmap() != temp)
		{
			itr->second->setBitmap(temp); // el caso de que haya cambiado un settlement a city
			anyChange = true;
		}
	}

	for (auto edge : localPlayer->getMyRoads())
	{
		auto itr = roads.find(edge);
		if (itr == roads.end()) // si no esta lo tengo que crear
		{
			float angle_rot;
			if (edge.length() == 2)
			{
				if ((edge[1] - edge[0]) == 1)
				{
					angle_rot = 0;
				}
				else if (edge[0] <= '9' && edge[0] >= '0')
				{
					angle_rot = ALLEGRO_PI * (((float)(1 - 2 * (edge[0] - '0'))) / 3);
				}
				else
				{
					float sing = ((edge[1] - edge[0]) % 2 ? -1 : 1); // si la diferencia es par va inclinado para un lado
					if (edge[0] >= 'D' && edge[0] <= 'L') { sing = (sing * -1); }
					angle_rot = sing * ALLEGRO_PI / 3;
				}
			}
			else // aristas de 3 letras
			{
				float desv = 0;
				switch (edge[0] - '0')
				{
				case 0:desv = ((edge[1] - edge[2]) > 0 ? 1.0 / 3.0 : -1.0 / 3.0); break;
				case 1:desv = ((edge[1] - edge[2]) > 0 ? -1.0 / 3.0 : 0); break;
				case 2:desv = ((edge[1] - edge[2]) > 0 ? 0 : 1.0 / 3.0); break;
				case 3:desv = ((edge[1] - edge[2]) == 1 ? -1.0 / 3.0 : 1.0 / 3.0); break;
				case 4:desv = ((edge[1] - edge[2]) & 0xF0 ? -1.0 / 3.0 : 0); break;
				case 5:desv = ((edge[1] - edge[2]) & 0xF0 ? 0 : 1.0 / 3.0); break;
				}
				angle_rot = ALLEGRO_PI * desv;
			}

			pair<unsigned int, unsigned int> pos = toDraw.getPositioningForEdge(edge);

			roads[edge] = impresora->createType(dibujo[ROAD], al_map_rgba_f(1.0, 0, 0, 1.0),
				pos.first+BOARD_POS_X, pos.second+BOARD_POS_Y,
				al_get_bitmap_width(dibujo[ROAD]) / 2, al_get_bitmap_height(dibujo[ROAD]) / 2,
				1, 1, angle_rot
			);
		}
		anyChange = true;
	}//listo los roads
	
	list<ResourceType> util = { CAMPOS,MONTA�AS,PASTOS,COLINAS,BOSQUE };
	int i = 0;
	for (auto a : util)
	{
		if (localPlayer->getResourceAmount(a) == 0)
		{
			if (cartas[a].second != NULL) // si no tiene cartas de este tipo saco el sello que le corresponde
			{
				impresora->delType(cartas[a].second);
				delete cartas[a].second;
				cartas[a].second = NULL;
				cartas[a].first = 0;
				anyChange = true;
			}
		}
		else if (localPlayer->getResourceAmount(a) != cartas[a].first) // si cambio la cantidad de cartas tengo que hacer un nuevo bitmap
		{
			if (cartas[a].second != NULL) // si ya existe el sello sobreescribo el birtmap
			{
				ALLEGRO_DISPLAY * tempDisplay = al_get_current_display();
				ALLEGRO_BITMAP * temp = cartas[a].second->getBitmap();
				al_set_target_bitmap(temp);
				al_clear_to_color(al_map_rgb(255, 255, 255));
				al_draw_bitmap(cartasfotos[a], 0, 0, 0);
				al_draw_text(fuente, al_map_rgb(0, 0, 0), al_get_bitmap_width(temp) / 2, 57, ALLEGRO_ALIGN_CENTRE, to_string(localPlayer->getResourceAmount(a)).c_str());
				al_set_target_backbuffer(tempDisplay);
				anyChange = true;
			}
			else // si no existe lo tengo que crear
			{
				ALLEGRO_DISPLAY * tempDisplay = al_get_current_display();
				ALLEGRO_BITMAP * temp = al_create_bitmap(35,65);// numeros magicos
				al_set_target_bitmap(temp);
				al_clear_to_color(al_map_rgb(255, 255, 255));
				al_draw_bitmap(cartasfotos[a], 0, 0, 0);
				al_draw_text(fuente, al_map_rgb(0, 0, 0), al_get_bitmap_width(temp) / 2, 57, ALLEGRO_ALIGN_CENTRE, to_string(localPlayer->getResourceAmount(a)).c_str());
				al_set_target_backbuffer(tempDisplay);

				cartas[a].second = impresora->createType(temp,
					al_map_rgb(255, 255, 255), D_ANCHO * 0.05 + al_get_bitmap_width(temp) * 1.05 * i, D_ALTO*0.3
				);
				anyChange = true;
			}
		}
		//si son iguales no hago nada, ya se va a imprimir bien 
	}

	if (localPlayer->hasLargestArmy())
	{
		if (sellos[LARMY] == NULL)
		{
			sellos[LARMY] = impresora->createType(dibujo[LARMY], al_map_rgb(255, 255, 255), D_ANCHO * 0.2, D_ALTO * 0.15);
		}
	}
	else if (sellos[LARMY] != NULL)
	{
		impresora->delType(sellos[LARMY]);
		delete sellos[LARMY];
	}

	if (localPlayer->hasLongestRoad())
	{
		if (sellos[LROAD] == NULL)
		{
			sellos[LROAD] = impresora->createType(dibujo[LROAD], al_map_rgb(255, 255, 255), D_ANCHO * 0.25, D_ALTO * 0.15);
		}
	}
	else if (sellos[LROAD] != NULL)
	{
		impresora->delType(sellos[LROAD]);
		delete sellos[LROAD];
	}
	
}

bool LocalObserver::isOK()
{
	return working;
}


void LocalObserver::attachAllButtons(vector<Button*>& lista)
{
	for (auto boton : lista)
	{
		allButtons.push_back(boton);
	}
}

GUIEnablerEvent LocalObserver::parseMouseDownEvent(uint32_t x, uint32_t y)
{
	if (isMouseDownActive())
	{

	}
	return NO_EV;
}

GUIEnablerEvent LocalObserver::parseTimerEvent()
{
	if (isTimerActive())
	{

	}
	return NO_EV;
}

bool LocalObserver::isThrowDiceActive()
{
	return throwDiceActivated;
}

bool LocalObserver::isPassActive()
{
	return passActivated;
}

bool LocalObserver::isAcceptActive()
{
	return acceptActivated;
}

bool LocalObserver::isCancelActive()
{
	return cancelActivated;
}

bool LocalObserver::isNewRoadActive()
{
	return newRoadActivated;
}

bool LocalObserver::isNewSettlementActive()
{
	return newSettlementActivated;
}

bool LocalObserver::isNewCityActive()
{
	return newCityActivated;
}

bool LocalObserver::isBuyDevCardActive()
{
	return buyDevCardActivated;
}

bool LocalObserver::isNewBankTradeActive()
{
	return newBankTradeActivated;
}

bool LocalObserver::isNewOfferTradeActive()
{
	return newOfferTradeActivated;
}

bool LocalObserver::isUseKnightActive()
{
	return useKnightActivated;
}

bool LocalObserver::isUseYearsOfPlentyActive()
{
	return useYearsOfPlentyActivated;
}

bool LocalObserver::isUseMonopolyActive()
{
	return useMonopolyActivated;
}

bool LocalObserver::isUseRoadBuildingActive()
{
	return useRoadBuildingActivated;
}

bool LocalObserver::isSelectWoolActive()
{
	return selectWoolActivated;
}

bool LocalObserver::isSelectOreActive()
{
	return selectOreActivated;
}

bool LocalObserver::isSelectLumberActive()
{
	return selectLumberActivated;
}

bool LocalObserver::isSelectGrainActive()
{
	return selectGrainActivated;
}

bool LocalObserver::isSelectBrickActive()
{
	return selectBrickActivated;
}

bool LocalObserver::isSelect4x1Active()
{
	return select4x1Activated;
}

bool LocalObserver::isSelect3x1Active()
{
	return select3x1Activated;
}

bool LocalObserver::isSelect2Tx1Active()
{
	return select2Tx1Activated;
}

bool LocalObserver::isSelect2Ox1Active()
{
	return select2Ox1Activated;
}

bool LocalObserver::isSelect2Lx1Active()
{
	return select2Lx1Activated;
}

bool LocalObserver::isSelect2Px1Active()
{
	return select2Px1Activated;
}

bool LocalObserver::isSelect2Mx1Active()
{
	return select2Mx1Activated;
}

void LocalObserver::setThrowDice(bool value)
{
	throwDiceActivated = value;
}

void LocalObserver::setPass(bool value)
{
	passActivated = value;
}

void LocalObserver::setAccept(bool value)
{
	acceptActivated = value;
}

void LocalObserver::setCancel(bool value)
{
	cancelActivated = value;
}

void LocalObserver::setNewRoad(bool value)
{
	newRoadActivated = value;
}

void LocalObserver::setNewSettlement(bool value)
{
	newSettlementActivated = value;
}

void LocalObserver::setNewCity(bool value)
{
	newCityActivated = value;
}

void LocalObserver::setBuyDevCard(bool value)
{
	buyDevCardActivated = value;
}

void LocalObserver::setNewBankTrade(bool value)
{
	newBankTradeActivated = value;
}

void LocalObserver::setNewOfferTrade(bool value)
{
	newOfferTradeActivated = value;
}

void LocalObserver::setUseKnight(bool value)
{
	useKnightActivated = value;
}

void LocalObserver::setUseYearsOfPlenty(bool value)
{
	useYearsOfPlentyActivated = value;
}

void LocalObserver::setUseMonopoly(bool value)
{
	useMonopolyActivated = value;
}

void LocalObserver::setUseRoadBuilding(bool value)
{
	useRoadBuildingActivated = value;
}

void LocalObserver::setSelectWool(bool value)
{
	selectWoolActivated = value;
}

void LocalObserver::setSelectOre(bool value)
{
	selectOreActivated = value;
}

void LocalObserver::setSelectLumber(bool value)
{
	selectLumberActivated = value;
}

void LocalObserver::setSelectGrain(bool value)
{
	selectGrainActivated = value;
}

void LocalObserver::setSelectBrick(bool value)
{
	selectBrickActivated = value;
}

void LocalObserver::setSelect4x1(bool value)
{
	select4x1Activated = value;
}

void LocalObserver::setSelect3x1(bool value)
{
	select3x1Activated = value;
}

void LocalObserver::setSelect2Tx1(bool value)
{
	select2Tx1Activated = value;
}

void LocalObserver::setSelect2Ox1(bool value)
{
	select2Ox1Activated = value;
}

void LocalObserver::setSelect2Lx1(bool value)
{
	select2Lx1Activated = value;
}

void LocalObserver::setSelect2Px1(bool value)
{
	select2Px1Activated = value;
}

void LocalObserver::setSelect2Mx1(bool value)
{
	select2Mx1Activated = value;
}

ClickZone::ClickZone(uint32_t fromX, uint32_t fromY, uint32_t toX, uint32_t toY)
{
	from.first = fromX;
	from.second = fromY;
	to.first = toX;
	to.second = toY;
}

ClickZone::~ClickZone()
{
}

bool ClickZone::isInZone(int x, int y)
{
	return (x >= from.first) && (x <= to.first) && (y <= from.second) && (y >= to.second);
}