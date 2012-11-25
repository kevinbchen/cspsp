
#include "ListBox.h"
#include "Globals.h"

JGE* ListBox::mEngine = NULL;
JRenderer* ListBox::mRenderer = NULL;

ListBox::ListBox(int x, int y, int width, int height, int itemHeight, int itemsPerPage, int type)
{
	mEngine = JGE::GetInstance();
	mRenderer = JRenderer::GetInstance();
	mItemIndex = 0;
	mScrollIndex = 0;

	mX = x;
	mY = y;
	mWidth = width;
	mHeight = height;
	mItemHeight = itemHeight;
	mItemsPerPage = itemsPerPage;
	mType = type;
}

ListBox::~ListBox()
{
	Clear();
}

void ListBox::Clear() {
	for (int i=0; i<(int)mItems.size(); i++) {
		delete mItems[i];
	}
	mItems.clear();

	mItemIndex = 0;
	mScrollIndex = 0;
}

void ListBox::AddItem(ListItem *item) {
	mItems.push_back(item);
}
void ListBox::RemoveItem(ListItem *item) {
	for (int i=0; i<(int)mItems.size(); i++) {
		if (mItems[i] == item) {
			mItems.erase(mItems.begin()+i);
			delete item;
			if (mItemIndex == mItems.size()-1 && mItemIndex > 0) {
				mItemIndex--;
				mScrollIndex--;
				if (mScrollIndex < 0) {
					mScrollIndex = 0;
				}
			}
			return;
		}
	}
}

ListItem *ListBox::GetItem() {
	if (mItemIndex >= 0 && mItemIndex < (int)mItems.size()) {
		return mItems[mItemIndex];
	}
	else {
		return NULL;
	}
}

void ListBox::AddText(char* text) {
	std::vector<char*> mLines;
	FormatText(mLines, text, mWidth*7/8.0f, 0.75f);

	for (int i=0; i<mLines.size(); i++) {
		AddItem(new TextItem(mLines[i]));
	}
}

bool ListBox::IsEmpty() {
	return mItems.size() == 0;
}

void ListBox::Sort(bool (*compare)(ListItem*, ListItem*)) {
	std::sort(mItems.begin(), mItems.end(), compare);
}

void ListBox::SetIndices(int itemIndex, int scrollIndex) {
	mItemIndex = itemIndex;
	mScrollIndex = scrollIndex;
	if (mScrollIndex > mItems.size()-mItemsPerPage) {
		mScrollIndex = mItems.size()-mItemsPerPage;
	}
}

void ListBox::Update(float dt) {
	int numItems = (int)mItems.size();

	if (numItems == 0) return;
	if (mType == TYPE_TEXT && numItems <= mItemsPerPage) return;

	int dir = 0;
	if (mEngine->GetButtonState(PSP_CTRL_UP) || mEngine->GetAnalogY()<64) {
		if (KeyRepeated(PSP_CTRL_UP,dt)) {
			dir = -1;
		}
	}
	else if (mEngine->GetButtonState(PSP_CTRL_DOWN) || mEngine->GetAnalogY()>192) {
		if (KeyRepeated(PSP_CTRL_DOWN,dt)) {
			dir = 1;
		}
	}
	else {
		gLastKey = 0;
	}
	
	if (mType == TYPE_LIST) {
		if (mEngine->GetButtonClick(PSP_CTRL_RIGHT)) {
			dir = 0;
			mItemIndex += mItemsPerPage;
			if (mItemIndex > numItems-1) {
				mItemIndex = numItems-1;
			}
			mScrollIndex += mItemsPerPage;
			if (mScrollIndex > numItems-mItemsPerPage) {
				mScrollIndex = numItems-mItemsPerPage;
			}
		}
		else if (mEngine->GetButtonClick(PSP_CTRL_LEFT)) {
			dir = 0;
			mItemIndex -= mItemsPerPage;
			if (mItemIndex < 0) {
				mItemIndex = 0;
			}
			mScrollIndex -= mItemsPerPage;
			if (mScrollIndex < 0) {
				mScrollIndex = 0;
			}
		}
	}
	else if (mType == TYPE_TEXT) {
		if (mEngine->GetButtonState(PSP_CTRL_RIGHT) || mEngine->GetAnalogX()>192) {
			dir = 1;
		}
		else if (mEngine->GetButtonState(PSP_CTRL_LEFT) || mEngine->GetAnalogX()<64) {
			dir = -1;
		}
	}

	if (mType == TYPE_LIST) {
		mItemIndex += dir;
		if (mItemIndex < 0) {
			mItemIndex = numItems-1;
			mScrollIndex = mItemIndex-mItemsPerPage+1;
			if (mScrollIndex < 0) {
				mScrollIndex = 0;
			}
		}
		else if (mItemIndex > numItems-1) {
			mItemIndex = 0;
			mScrollIndex = 0;
		}

		if (mItemIndex < mScrollIndex) {
			mScrollIndex--;
		}
		else if (mItemIndex-mScrollIndex >= mItemsPerPage) {
			mScrollIndex++;
		}
	}
	else if (mType == TYPE_TEXT) {
		mItemIndex += dir;
		if (mItemIndex < 0) {
			mItemIndex = 0;
		}
		else if (mItemIndex > numItems-mItemsPerPage) {
			mItemIndex = numItems-mItemsPerPage;
		}
		mScrollIndex = mItemIndex;
	}
}

void ListBox::Render() {
	if (mItems.size() > 0 && mType == TYPE_LIST) {
		mRenderer->FillRect(mX,mY+(mItemIndex-mScrollIndex)*mItemHeight,mWidth,mItemHeight,ARGB(255,0,0,0));
	}

	int index = mScrollIndex;
	for (int i=0; i<mItemsPerPage; i++) {
		if (index >= mItems.size()) break;

		if (index == mItemIndex && mType == TYPE_LIST) {
			gFont->SetColor(ARGB(255,0,128,255));
		}
		else {
			gFont->SetColor(ARGB(255,255,255,255));
		}
		mItems[index]->Render(mX+mWidth/16.0f, mY+i*mItemHeight, index == mItemIndex);

		index++;
	}

	if (mItems.size() > mItemsPerPage) {
		mRenderer->FillRect(mX+mWidth*31/32.0f-8,mY,8,mHeight,ARGB(255,100,100,100));
		mRenderer->FillRect(mX+mWidth*31/32.0f-8,mY+((float)mScrollIndex/mItems.size())*mHeight,8,mHeight*((float)mItemsPerPage/mItems.size()),ARGB(255,255,255,255));
	}

	gFont->SetColor(ARGB(255,255,255,255));
}