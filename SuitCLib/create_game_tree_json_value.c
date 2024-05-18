#include "header.h"


void game_tree_add(char* gameTreeJsonValue, char* s)
{
	strncat(gameTreeJsonValue, s, 1024);
}

void game_tree_add_char(char* gameTreeJsonValue, char c)
{
	c = toupper(c);
	strncat(gameTreeJsonValue, &c, 1);
}

void construct_game_tree(struct TREE_ELEMENT* treeElements, int treeElementsCount, char* gameTreeJsonValue, int* pElementNo)
{
	if (*pElementNo >= treeElementsCount)
	{
		return;
	}
	int pos = treeElements[*pElementNo].pos;
	int type = treeElements[*pElementNo].type;

	game_tree_add(gameTreeJsonValue, "{\"T\":\"");
	game_tree_add_char(gameTreeJsonValue, treeElements[*pElementNo].content);
	(*pElementNo)++;
	if (type == 0)
	{
		//Seat
		game_tree_add_char(gameTreeJsonValue, ':');
		if (*pElementNo < treeElementsCount)
		{
			game_tree_add_char(gameTreeJsonValue, treeElements[*pElementNo].content);
			(*pElementNo)++;
		}
	}
	else
	{
		//Card(s)
		int lastPos = pos;
		while (*pElementNo < treeElementsCount && treeElements[*pElementNo].pos == lastPos + 1)
		{
			game_tree_add_char(gameTreeJsonValue, treeElements[*pElementNo].content);
			lastPos++;
			(*pElementNo)++;
		}
	}
	game_tree_add(gameTreeJsonValue, "\",\"Vs\":[");

	bool first = true;
	while (*pElementNo < treeElementsCount && treeElements[*pElementNo].pos > pos)
	{
		if (first)
		{
			first = false;
		}
		else
		{
			game_tree_add_char(gameTreeJsonValue, ',');
		}
		construct_game_tree(treeElements, treeElementsCount, gameTreeJsonValue, pElementNo);
	}

	game_tree_add(gameTreeJsonValue, "]}");
}


void create_game_tree_json_value(struct TREE_ELEMENT* treeElements, int treeElementsCount, char* gameTreeJsonValue)
{
	gameTreeJsonValue[0] = '\0';
	int elementNo = 0;
	construct_game_tree(treeElements, treeElementsCount, gameTreeJsonValue, &elementNo);
}

