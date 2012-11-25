#ifndef _LISTBOX_H_
#define _LISTBOX_H_

#include <vector>
#include <algorithm>
#include "JGE.h"
#include "JRenderer.h"
#include "Globals.h"
//------------------------------------------------------------------------------------------------

#define TYPE_LIST 0
#define TYPE_TEXT 1 

class ListItem
{
public:
	ListItem() {}
	virtual ~ListItem() {}

	virtual void Render(float x, float y, bool selected = false) {}
};

class TextItem : public ListItem
{
public:
	char *text;
	TextItem(char *text) {
		this->text = text;
	}
	~TextItem() {
		delete text;
	}

	void Render(float x, float y, bool selected = false) {
		gFont->DrawShadowedString(text,x,y+5);
	}
};

class ListBox
{
private:
	static JGE* mEngine;
	static JRenderer* mRenderer;

public:
	int mType;

	std::vector<ListItem*> mItems;
	int mItemIndex;
	int mScrollIndex;

	float mX;
	float mY;
	int mWidth;
	int mHeight;

	int mItemHeight;
	int mItemsPerPage;

	ListBox(int x, int y, int width, int height, int itemHeight, int itemsPerPage, int type = TYPE_LIST);
	~ListBox();

	void Update(float dt);
	void Render();

	void AddItem(ListItem *item);
	void RemoveItem(ListItem *item);
	void Sort(bool (*compare)(ListItem*, ListItem*));
	void SetIndices(int itemIndex, int scrollIndex);

	void Clear();
	bool IsEmpty();

	ListItem *GetItem();

	void AddText(char *text);

};
#endif
