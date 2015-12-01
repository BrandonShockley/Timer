#pragma once
#include "Entity.h"

enum TileType
{
	NONSOLID,
	SOLID,
	GATE
};

struct Tile
{
	TileType type;
	Entity* entity;
};