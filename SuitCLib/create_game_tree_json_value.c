#include "header.h"
#include <ctype.h>


void game_tree_add(char* gameTreeJsonValue, const char* s)
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
	//printf("pElementNo %d\n", *pElementNo);

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
	gameTreeJsonValue[0] = '\0';  // Start with an empty string
	strcat(gameTreeJsonValue, "[\n");  // Begin JSON array
	int elementNo = 0;
	for (int elementNo = 0; elementNo < treeElementsCount; elementNo++) {
		char tempBuffer[JSON_BUFFER_SIZE];  // Temporary buffer for each element
		tempBuffer[0] = '\0';

		construct_game_tree(treeElements, treeElementsCount, gameTreeJsonValue, &elementNo);
		elementNo--;

		strcat(gameTreeJsonValue, tempBuffer);  // Append element JSON

		if (elementNo < treeElementsCount - 1) {
			strcat(gameTreeJsonValue, ",\n");  // Add comma **only if not the last element**
		}
	}

	strcat(gameTreeJsonValue, "]\n");  // Close JSON array
}