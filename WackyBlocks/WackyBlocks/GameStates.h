#pragma once
#ifndef GAMESTATES_H
#define GAMESTATES_H

#include <SFML/Graphics.hpp>

enum class GameState
{
	MAIN_MENU,
	LEVEL_EDITOR,
	OPTIONS,
	GAMEPLAY
};

enum class GameMode
{
	NONE,
	SINGLE_PLAYER,
	MULTIPLAYER
};

#endif