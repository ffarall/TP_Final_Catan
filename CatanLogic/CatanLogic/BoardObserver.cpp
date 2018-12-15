#include "BoardObserver.h"
#include "BoardController.h"


#define MAP_FILE "mapaFinal.png"
#define HILLS_HEX_FILE "colinas.png"
#define WOODS_HEX_FILE "bosque.png"
#define MOUNTAINS_HEX_FILE "monta�a.png"
#define FIELDS_HEX_FILE "campo.png"
#define PASTURES_HEX_FILE "pastura.png"
#define DESERT_HEX_FILE "desierto.png"
#define SEA_3X1_FILE "sea3x1.png"
#define SEA_2TX1_FILE "sea2Tx1.png"
#define SEA_2OX1_FILE "sea2Ox1.png"
#define SEA_2LX1_FILE "sea2Lx1.png"
#define SEA_2PX1_FILE "sea2Px1.png"
#define SEA_2MX1_FILE "sea2Mx1.png"
#define SETTLEMENT_FILE "settlement.png"
#define CITY_FILE "city.png"
#define ROAD_FILE "road.png"
#define ROBBER_FILE "robber.png"

#define BOARD_POS_X 310
#define BOARD_POS_Y 88

BoardObserver::BoardObserver()
{
	init();
	update();
}


BoardObserver::~BoardObserver()
{
	delete decoder;
}

void BoardObserver::update()
{
	drawMap();
	for (auto edge : localPlayer->getMyRoads()) { drawRoad(edge, true); }
	for (auto edge : remotePlayer->getMyRoads()) { drawRoad(edge, false); }

	for (auto vertex : localPlayer->getMySettlements()) { drawBuilding(vertex, true, true); }
	for (auto vertex : localPlayer->getMySettlements()) { drawBuilding(vertex, true, false); }

	for (auto vertex : localPlayer->getMyCities()) { drawBuilding(vertex, false, true); }
	for (auto vertex : localPlayer->getMyCities()) { drawBuilding(vertex, false, false); }
}

void BoardObserver::init()
{
	decoder = new MapDecoder(MAP_FILE);

	bitmaps[HILLS_HEX_FILE] = al_load_bitmap(HILLS_HEX_FILE);
	bitmaps[WOODS_HEX_FILE] = al_load_bitmap(WOODS_HEX_FILE);
	bitmaps[MOUNTAINS_HEX_FILE] = al_load_bitmap(MOUNTAINS_HEX_FILE);
	bitmaps[FIELDS_HEX_FILE] = al_load_bitmap(FIELDS_HEX_FILE);
	bitmaps[PASTURES_HEX_FILE] = al_load_bitmap(PASTURES_HEX_FILE);
	bitmaps[DESERT_HEX_FILE] = al_load_bitmap(DESERT_HEX_FILE);
	bitmaps[SEA_3X1_FILE] = al_load_bitmap(SEA_3X1_FILE);
	bitmaps[SEA_2TX1_FILE] = al_load_bitmap(SEA_2TX1_FILE);
	bitmaps[SEA_2OX1_FILE] = al_load_bitmap(SEA_2OX1_FILE);
	bitmaps[SEA_2LX1_FILE] = al_load_bitmap(SEA_2LX1_FILE);
	bitmaps[SEA_2PX1_FILE] = al_load_bitmap(SEA_2PX1_FILE);
	bitmaps[SEA_2MX1_FILE] = al_load_bitmap(SEA_2MX1_FILE);
	bitmaps[CITY_FILE] = al_load_bitmap(CITY_FILE);
	bitmaps[ROAD_FILE] = al_load_bitmap(ROAD_FILE);
	bitmaps[SETTLEMENT_FILE] = al_load_bitmap(SETTLEMENT_FILE);
	bitmaps[ROBBER_FILE] = al_load_bitmap(ROBBER_FILE);
	bool allegOk = true;

	for (auto imagen : bitmaps) // me fijo si esta todo inicializado
	{
		if (imagen.second == NULL) { allegOk = false; }
	}
	if (!allegOk) // si hubo algun problema limpio
	{
		for (auto imagen : bitmaps)
		{
			if (imagen.second != NULL) { al_destroy_bitmap(imagen.second); }
		}
		// ver de agregar algo de error o nose
	}
}

void BoardObserver::drawMap()
{
	for (int i = 0; i < 6; i++) // dibujo el agua
	{
		ALLEGRO_BITMAP *  temp = NULL;
		switch (board->getPortType('0' + i))
		{
		case PortType::_2Lx1: temp = bitmaps[SEA_2LX1_FILE]; break;
		case PortType::_2Mx1:temp = bitmaps[SEA_2MX1_FILE]; break;
		case PortType::_2Ox1:temp = bitmaps[SEA_2OX1_FILE]; break;
		case PortType::_2Px1:temp = bitmaps[SEA_2PX1_FILE]; break;
		case PortType::_2Tx1:temp = bitmaps[SEA_2TX1_FILE]; break;
		case PortType::_3x1:temp = bitmaps[SEA_3X1_FILE]; break;
		}
		string letra = "0";
		letra[0] += i;
		pair<unsigned int, unsigned int> pos = decoder->getPositioningForToken(letra);
		al_draw_rotated_bitmap(temp,
			al_get_bitmap_width(temp) / 2, al_get_bitmap_height(temp) / 2,
			pos.first + BOARD_POS_X, pos.second + BOARD_POS_Y,
			ALLEGRO_PI*((((float)i) / 3) - 1), 0);
	}

	for (int i = 0; i < 19; i++) // dibujo los hex
	{
		ALLEGRO_BITMAP *  temp = NULL;
		switch (board->getResourceFromHex('A' + i))
		{
		case BOSQUE: temp = bitmaps[WOODS_HEX_FILE]; break;
		case COLINAS:temp = bitmaps[HILLS_HEX_FILE]; break;
		case MONTA�AS:temp = bitmaps[MOUNTAINS_HEX_FILE]; break;
		case CAMPOS:temp = bitmaps[FIELDS_HEX_FILE]; break;
		case PASTOS:temp = bitmaps[PASTURES_HEX_FILE]; break;
		case DESIERTO:temp = bitmaps[DESERT_HEX_FILE]; break;
		}

		string letra = "A";
		letra[0] += i;
		pair<unsigned int, unsigned int> pos = decoder->getPositioningForToken(letra);
		al_draw_rotated_bitmap(temp,
			al_get_bitmap_width(temp) / 2, al_get_bitmap_height(temp) / 2,
			pos.first + BOARD_POS_X, pos.second + BOARD_POS_Y,
			0, 0);
	}
}

void BoardObserver::drawRoad(string edge, bool player)
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

	pair<unsigned int, unsigned int> pos = decoder->getPositioningForEdge(edge);

	al_draw_tinted_rotated_bitmap(bitmaps[ROAD_FILE],
		player ? al_map_rgba_f(1, 0, 0, 1) : al_map_rgba_f(0, 0, 1, 1),
		al_get_bitmap_width(bitmaps[ROAD_FILE]) / 2, al_get_bitmap_height(bitmaps[ROAD_FILE]) / 2,
		pos.first + BOARD_POS_X, pos.second + BOARD_POS_Y,
		angle_rot, 0
	);
}

void BoardObserver::drawBuilding(string edge, bool type, bool player)
{
	ALLEGRO_BITMAP * temp = (type ? bitmaps[CITY_FILE] : bitmaps[SETTLEMENT_FILE]);
	pair<unsigned int, unsigned int> pos = decoder->getPositioningForVertex(edge);
	al_draw_tinted_rotated_bitmap(temp,
		player ? al_map_rgba_f(1, 0, 0, 1) : al_map_rgba_f(0, 0, 1, 1),
		al_get_bitmap_width(temp) / 2, al_get_bitmap_height(temp) / 2,
		pos.first + BOARD_POS_X, pos.second + BOARD_POS_Y,
		0, 0);
}
