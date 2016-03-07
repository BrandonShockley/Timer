#pragma once
#include "Entity.h"

enum TileType
{
	NONSOLID,
	SOLID,
	LIGHT
};

struct Tile
{
	TileType type;
	Entity* entity;
};