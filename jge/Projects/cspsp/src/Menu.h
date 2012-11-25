#ifndef _MENU_H_
#define _MENU_H_

struct Button {
	Button(int _id, char* _name) {
		id = _id;
		strcpy(name,_name);
	};
	int id;
	char name[256];
};
struct Category {
	int id;
	std::vector<Button> buttons;
};

#endif
