#pragma once
#include <gui/Screen.hpp>
#include <entity/Villager.hpp>

struct Player;
struct NinePatchLayer;

struct VillagerTradeScreen : Screen {
	Player* player;
	Villager* villager;
	int32_t selectedTrade;
	int32_t scrollOffset;
	int32_t guiX, guiY;
	int32_t dialogW, dialogH;
	ItemInstance placedInputItem;
	int32_t lastSourceSlot;
	int32_t statusTimer;
	std::string statusMessage;
	uint32_t statusColor;
	NinePatchLayer* bgPanel;
	NinePatchLayer* insetPanel;
	NinePatchLayer* btnNormal;
	NinePatchLayer* btnPressed;
	NinePatchLayer* slotNormal;
	NinePatchLayer* slotSelected;

	VillagerTradeScreen(Player* player, Villager* villager);

	virtual ~VillagerTradeScreen();
	virtual void init();
	virtual void render(int32_t, int32_t, float);
	virtual void tick();
	virtual void removed();
	virtual bool_t renderGameBehind();
	virtual bool_t isInGameScreen();
	virtual void buttonClicked(Button*);
	virtual void mouseClicked(int32_t, int32_t, int32_t);
	virtual void keyPressed(int32_t);
	virtual bool_t backPressed(Minecraft*, bool_t);
};
