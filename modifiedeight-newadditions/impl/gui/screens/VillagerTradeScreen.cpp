#include <gui/screens/VillagerTradeScreen.hpp>
#include <Minecraft.hpp>
#include <entity/Player.hpp>
#include <entity/LocalPlayer.hpp>
#include <entity/Villager.hpp>
#include <inventory/Inventory.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <level/Level.hpp>
#include <network/RakNetInstance.hpp>
#include <network/packet/ContainerSetSlotPacket.hpp>
#include <rendering/Font.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <rendering/entity/ItemRenderer.hpp>
#include <gui/buttons/Button.hpp>
#include <gui/NinePatchFactory.hpp>
#include <gui/NinePatchLayer.hpp>
#include <util/IntRectangle.hpp>
#include <I18n.hpp>
#include <cpputils.hpp>
#include <unigl.h>
#include <algorithm>
#include <cstdio>

static void renderMcpeBtn(NinePatchLayer* normal, NinePatchLayer* pressed, Font* font, int32_t x, int32_t y, int32_t w, int32_t h, const std::string& text, bool hov, bool enabled = true, uint32_t customColor = 0) {
	NinePatchLayer* layer = (enabled && hov) ? pressed : normal;
	if (layer) {
		layer->setSize((float)w, (float)h);
		if (!enabled) {
			glColor4f(0.75f, 0.75f, 0.75f, 1.0f);
		} else {
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
		layer->draw(Tesselator::instance, (float)x, (float)y);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
	int32_t tw = font->width(text);
	int32_t tx = x + (w - tw) / 2;
	int32_t ty = y + (h - 8) / 2;
	uint32_t col = !enabled ? 0xFFA0A0A0 : (customColor ? customColor : (hov ? 0xFFFFFFA0 : 0xFFE0E0E0));
	font->drawShadow(text, tx, ty, col);
}

static void renderMcpeCloseBtn(Minecraft* mc, int32_t x, int32_t y, int32_t size, bool hov) {
	mc->texturesPtr->loadAndBindTexture("gui/spritesheet.png");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (hov) {
		glColor4f(1.0f, 1.0f, 0.85f, 1.0f);
	} else {
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
	float u0 = 60.0f / 256.0f;
	float v0 = 0.0f / 256.0f;
	float u1 = 78.0f / 256.0f;
	float v1 = 18.0f / 256.0f;
	Tesselator& t = Tesselator::instance;
	t.begin(4);
	t.vertexUV((float)x, (float)(y + size), 0.0f, u0, v1);
	t.vertexUV((float)(x + size), (float)(y + size), 0.0f, u1, v1);
	t.vertexUV((float)(x + size), (float)y, 0.0f, u1, v0);
	t.vertexUV((float)x, (float)y, 0.0f, u0, v0);
	t.draw(1);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

static std::string getProfessionTitle(int32_t prof) {
	switch (prof) {
		case 0: return "Villager - Farmer";
		case 1: return "Villager - Librarian";
		case 2: return "Villager - Priest";
		case 3: return "Villager - Blacksmith";
		case 4: return "Villager - Butcher";
		default: return "Villager";
	}
}

static std::string getItemDisplayName(const ItemInstance& inst) {
	if (inst.isNull()) return "";
	ItemInstance tmp(inst);
	std::string raw = tmp.getName();
	if (raw.empty()) return "Item";
	std::string loc = I18n::get(raw);
	if (!loc.empty() && loc != raw) raw = loc;
	else {
		std::string key = raw + ".name";
		loc = I18n::get(key);
		if (!loc.empty() && loc != key) raw = loc;
	}
	if (raw.size() > 5 && raw.substr(raw.size() - 5) == ".name") {
		raw = raw.substr(0, raw.size() - 5);
	}
	if (raw.size() > 5 && (raw.substr(0, 5) == "item." || raw.substr(0, 5) == "tile.")) {
		raw = raw.substr(5);
	}
	if (!raw.empty() && raw[0] >= 'a' && raw[0] <= 'z') {
		raw[0] = (char)(raw[0] - 'a' + 'A');
	}
	return raw;
}

static void syncInventoryToServer(Minecraft* mc, Player* player, const std::vector<ItemInstance>& before) {
	if (!mc || !mc->level || !mc->level->isClientMaybe || !mc->rakNetInstance || !player || !player->inventory) return;
	bool isCreative = player->inventory->field_20 != 0;
	if (isCreative) {
		for (int32_t i = 0; i < 9 && i < (int32_t)before.size(); ++i) {
			ItemInstance* cur = player->inventory->getItem(i);
			ItemInstance curVal = (cur && !cur->isNull() && cur->count > 0) ? *cur : ItemInstance();
			if (!ItemInstance::matches(&before[i], &curVal)) {
				ContainerSetSlotPacket pk(0, 0, i, curVal);
				mc->rakNetInstance->send(pk);
			}
		}
		return;
	}
	int32_t sz = player->inventory->getContainerSize();
	for (int32_t i = 9; i < sz && i < (int32_t)before.size(); ++i) {
		ItemInstance* cur = player->inventory->getItem(i);
		ItemInstance curVal = (cur && !cur->isNull() && cur->count > 0) ? *cur : ItemInstance();
		if (!ItemInstance::matches(&before[i], &curVal)) {
			ContainerSetSlotPacket pk(0, 0, i - 9, curVal);
			mc->rakNetInstance->send(pk);
		}
	}
}

static std::vector<ItemInstance> snapshotInventory(Player* player) {
	std::vector<ItemInstance> snap;
	if (!player || !player->inventory) return snap;
	int32_t sz = player->inventory->getContainerSize();
	snap.resize(sz);
	for (int32_t i = 0; i < sz; ++i) {
		ItemInstance* s = player->inventory->getItem(i);
		snap[i] = (s && !s->isNull() && s->count > 0) ? *s : ItemInstance();
	}
	return snap;
}

static int32_t countMatchingInInventory(Player* player, int32_t itemId, int32_t itemMeta, const ItemInstance* extraSlot) {
	if (!player || !player->inventory) return 0;
	if (player->inventory->field_20 != 0) return 999;
	int32_t total = 0;
	if (extraSlot && !extraSlot->isNull() && extraSlot->count > 0 &&
	    extraSlot->getId() == itemId && (itemMeta == 0 || extraSlot->getAuxValue() == itemMeta)) {
		total += extraSlot->count;
	}
	int32_t sz = player->inventory->getContainerSize();
	for (int32_t i = 9; i < sz; ++i) {
		ItemInstance* s = player->inventory->getItem(i);
		if (s && !s->isNull() && s->count > 0 && s->getId() == itemId &&
		    (itemMeta == 0 || s->getAuxValue() == itemMeta)) {
			total += s->count;
		}
	}
	return total;
}

static void returnPlacedItem(Minecraft* mc, Player* player, ItemInstance& placedInputItem, int32_t& lastSourceSlot) {
	if (!player || !player->inventory || placedInputItem.isNull() || placedInputItem.count <= 0) {
		placedInputItem = ItemInstance();
		return;
	}
	std::vector<ItemInstance> before = snapshotInventory(player);
	bool isCreative = player->inventory->field_20 != 0;
	if (!isCreative) {
		if (lastSourceSlot >= 9 && lastSourceSlot < player->inventory->getContainerSize() &&
		    (!player->inventory->getItem(lastSourceSlot) || player->inventory->getItem(lastSourceSlot)->isNull())) {
			player->inventory->setItem(lastSourceSlot, &placedInputItem);
		} else if (!player->inventory->add(&placedInputItem)) {
			player->drop(&placedInputItem, 0);
		}
		player->inventory->setContainerChanged();
		syncInventoryToServer(mc, player, before);
	}
	placedInputItem = ItemInstance();
	lastSourceSlot = -1;
}

static bool doTrade(Minecraft* mc, Player* player, Villager* villager, int32_t idx, ItemInstance* placedSlot, int32_t& sourceSlot) {
	if (!player || !player->inventory || !villager) return false;
	if (idx < 0 || idx >= villager->tradeCount) return false;
	VillagerTrade& tr = villager->trades[idx];

	std::vector<ItemInstance> before = snapshotInventory(player);
	bool isCreative = player->inventory->field_20 != 0;
	if (isCreative) {
		ItemInstance out(tr.outputId, tr.outputCount, tr.outputMeta);
		int32_t targetSlot = (sourceSlot >= 0 && sourceSlot < 9) ? sourceSlot : player->inventory->selectedSlot;
		if (targetSlot >= 0 && targetSlot < 9) {
			player->inventory->setItem(targetSlot, &out);
		} else {
			player->inventory->add(&out);
		}
		player->inventory->setContainerChanged();
		if (placedSlot) *placedSlot = ItemInstance();
		syncInventoryToServer(mc, player, before);
		return true;
	}

	int32_t fromPlaced = 0;
	if (placedSlot && !placedSlot->isNull() && placedSlot->count > 0) {
		if (placedSlot->getId() == tr.inputId && (tr.inputMeta == 0 || placedSlot->getAuxValue() == tr.inputMeta)) {
			fromPlaced = placedSlot->count;
		} else {
			returnPlacedItem(mc, player, *placedSlot, sourceSlot);
			before = snapshotInventory(player);
		}
	}

	int32_t haveInv = 0;
	int32_t sz = player->inventory->getContainerSize();
	for (int32_t i = 9; i < sz; ++i) {
		ItemInstance* s = player->inventory->getItem(i);
		if (s && !s->isNull() && s->count > 0 && s->getId() == tr.inputId &&
		    (tr.inputMeta == 0 || s->getAuxValue() == tr.inputMeta)) {
			haveInv += s->count;
		}
	}

	if (fromPlaced + haveInv < tr.inputCount) return false;

	int32_t rem = tr.inputCount;
	if (fromPlaced > 0 && placedSlot) {
		int32_t take = fromPlaced < rem ? fromPlaced : rem;
		placedSlot->count -= take;
		rem -= take;
		if (placedSlot->count <= 0) {
			*placedSlot = ItemInstance();
		}
	}

	for (int32_t i = 9; i < sz && rem > 0; ++i) {
		ItemInstance* s = player->inventory->getItem(i);
		if (s && !s->isNull() && s->count > 0 && s->getId() == tr.inputId &&
		    (tr.inputMeta == 0 || s->getAuxValue() == tr.inputMeta)) {
			int32_t take = s->count < rem ? s->count : rem;
			s->count -= take;
			rem -= take;
			if (s->count <= 0) {
				player->inventory->clearSlot(i);
			}
		}
	}

	ItemInstance out(tr.outputId, tr.outputCount, tr.outputMeta);
	if (!player->inventory->add(&out)) {
		player->drop(&out, 0);
	}
	player->inventory->setContainerChanged();
	syncInventoryToServer(mc, player, before);
	return true;
}

VillagerTradeScreen::VillagerTradeScreen(Player* p, Villager* v)
	: player(p), villager(v), selectedTrade(0), scrollOffset(0),
	  guiX(0), guiY(0), dialogW(256), dialogH(212),
	  lastSourceSlot(-1), statusTimer(0), statusMessage(""), statusColor(0xFFC8C8C8),
	  bgPanel(0), insetPanel(0), btnNormal(0), btnPressed(0), slotNormal(0), slotSelected(0) {
	if (this->villager) {
		this->villager->tradingPlayer = p;
		if (this->villager->tradeCount <= 0) {
			this->villager->initTrades();
		}
	}
}

VillagerTradeScreen::~VillagerTradeScreen() {
	if (this->villager) this->villager->tradingPlayer = nullptr;
	safeRemove(this->bgPanel);
	safeRemove(this->insetPanel);
	safeRemove(this->btnNormal);
	safeRemove(this->btnPressed);
	safeRemove(this->slotNormal);
	safeRemove(this->slotSelected);
}

void VillagerTradeScreen::init() {
	this->buttons.clear();
	this->dialogW = std::min(256, this->width - 12);
	this->dialogH = std::min(212, this->height - 12);
	this->guiX = (this->width - this->dialogW) / 2;
	this->guiY = (this->height - this->dialogH) / 2;

	safeRemove(this->bgPanel);
	safeRemove(this->insetPanel);
	safeRemove(this->btnNormal);
	safeRemove(this->btnPressed);
	safeRemove(this->slotNormal);
	safeRemove(this->slotSelected);

	NinePatchFactory npf(this->minecraft->texturesPtr, "gui/spritesheet.png");
	this->bgPanel = npf.createSymmetrical({0, 0, 16, 16}, 4, 4, (float)this->dialogW, (float)this->dialogH);
	this->insetPanel = npf.createSymmetrical({0, 32, 8, 8}, 3, 3, 32.0f, 32.0f);
	this->btnNormal = npf.createSymmetrical({8, 32, 8, 8}, 2, 2, 32.0f, 18.0f);
	this->btnPressed = npf.createSymmetrical({0, 32, 8, 8}, 2, 2, 32.0f, 18.0f);
	this->slotNormal = npf.createSymmetrical({0, 32, 8, 8}, 3, 3, 20.0f, 20.0f);
	this->slotSelected = npf.createSymmetrical({10, 42, 16, 16}, 5, 5, 22.0f, 22.0f);
}

void VillagerTradeScreen::tick() {
	if (this->statusTimer > 0) {
		this->statusTimer--;
	}
	if (!this->villager || this->villager->isDead) {
		this->minecraft->setScreen(0);
		return;
	}
	if (this->player && this->player->distanceTo(this->villager) > 6.5f) {
		this->minecraft->setScreen(0);
	}
}

void VillagerTradeScreen::removed() {
	returnPlacedItem(this->minecraft, this->player, this->placedInputItem, this->lastSourceSlot);
}

bool_t VillagerTradeScreen::renderGameBehind() { return 1; }
bool_t VillagerTradeScreen::isInGameScreen() { return 0; }

void VillagerTradeScreen::buttonClicked(Button* btn) {
}

static void renderSlotItem(Font* font, Textures* textures, const ItemInstance* inst, float x, float y) {
	if (!inst || inst->isNull() || inst->count <= 0) return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	ItemRenderer::renderGuiItemNew(textures, (ItemInstance*)inst, 0, x, y, 1.0f, 1.0f, 1.0f);
	if (inst->count > 1) {
		char buf[16];
		snprintf(buf, sizeof(buf), "%d", inst->count);
		int32_t w = font->width(buf);
		font->drawShadow(buf, (int32_t)(x + 17.0f - (float)w), (int32_t)(y + 9.0f), 0xFFFFFFFF);
	}
}

void VillagerTradeScreen::mouseClicked(int32_t mx, int32_t my, int32_t btn) {
	int32_t x0 = this->guiX;
	int32_t y0 = this->guiY;
	int32_t x1 = x0 + this->dialogW;
	int32_t y1 = y0 + this->dialogH;

	int32_t closeX = x1 - 21;
	int32_t closeY = y0 + 3;
	if (mx >= closeX && mx <= closeX + 18 && my >= closeY && my <= closeY + 18) {
		this->minecraft->setScreen(0);
		return;
	}

	if (!this->villager) return;
	if (this->villager->tradeCount <= 0) this->villager->initTrades();
	int32_t cnt = this->villager->tradeCount;
	if (this->selectedTrade >= cnt) this->selectedTrade = cnt - 1;
	if (this->selectedTrade < 0) this->selectedTrade = 0;

	int32_t leftX = x0 + 8;
	int32_t leftY = y0 + 40;
	int32_t leftW = 92;
	int32_t leftH = this->dialogH - 40 - 27;
	int32_t listY = leftY + 4;
	int32_t listH = leftH - 8;
	int32_t itemH = 24;
	int32_t visibleCount = std::max(1, listH / itemH);

	if (btn == 4) {
		if (this->scrollOffset > 0) this->scrollOffset--;
		return;
	}
	if (btn == 5) {
		if (this->scrollOffset + visibleCount < cnt) this->scrollOffset++;
		return;
	}

	int32_t rightX = leftX + leftW + 4;
	int32_t rightY = leftY;
	int32_t rightW = this->dialogW - 16 - leftW - 4;
	int32_t rightH = leftH;

	for (int32_t i = 0; i < visibleCount; ++i) {
		int32_t tIdx = this->scrollOffset + i;
		if (tIdx >= cnt) break;
		int32_t ry = listY + i * itemH;
		int32_t rx = leftX + 3;
		int32_t rw = leftW - 6;
		if (mx >= rx && mx <= rx + rw && my >= ry && my <= ry + itemH - 2) {
			if (this->selectedTrade != tIdx) {
				returnPlacedItem(this->minecraft, this->player, this->placedInputItem, this->lastSourceSlot);
				this->selectedTrade = tIdx;
				if (this->minecraft->level) {
					this->minecraft->level->playSound(this->villager, "mob.villager.haggle", 1.0f, 1.0f);
				}
			}
			return;
		}
	}

	int32_t prevX = rightX + 4;
	int32_t navY = rightY + 4;
	int32_t navW = 20;
	int32_t navH = 16;
	if (mx >= prevX && mx <= prevX + navW && my >= navY && my <= navY + navH) {
		if (this->selectedTrade > 0) {
			returnPlacedItem(this->minecraft, this->player, this->placedInputItem, this->lastSourceSlot);
			this->selectedTrade--;
			if (this->selectedTrade < this->scrollOffset) this->scrollOffset = this->selectedTrade;
			if (this->minecraft->level) {
				this->minecraft->level->playSound(this->villager, "mob.villager.haggle", 1.0f, 1.0f);
			}
		}
		return;
	}

	int32_t nextX = rightX + rightW - 24;
	if (mx >= nextX && mx <= nextX + navW && my >= navY && my <= navY + navH) {
		if (this->selectedTrade < cnt - 1) {
			returnPlacedItem(this->minecraft, this->player, this->placedInputItem, this->lastSourceSlot);
			this->selectedTrade++;
			if (this->selectedTrade >= this->scrollOffset + visibleCount) {
				this->scrollOffset = this->selectedTrade - visibleCount + 1;
			}
			if (this->minecraft->level) {
				this->minecraft->level->playSound(this->villager, "mob.villager.haggle", 1.0f, 1.0f);
			}
		}
		return;
	}

	VillagerTrade& tr = this->villager->trades[this->selectedTrade];

	int32_t tradeBoxY = rightY + 34;
	int32_t inSlotX = rightX + 18;
	int32_t inSlotY = tradeBoxY + 6;
	if (mx >= inSlotX && mx <= inSlotX + 24 && my >= inSlotY && my <= inSlotY + 24) {
		if (!this->placedInputItem.isNull() && this->placedInputItem.count > 0) {
			returnPlacedItem(this->minecraft, this->player, this->placedInputItem, this->lastSourceSlot);
		}
		return;
	}

	int32_t outSlotX = rightX + rightW - 42;
	int32_t outSlotY = tradeBoxY + 6;
	int32_t tradeBtnX = rightX + 14;
	int32_t tradeBtnY = rightY + rightH - 30;
	int32_t tradeBtnW = rightW - 28;
	int32_t tradeBtnH = 20;
	if ((mx >= tradeBtnX && mx <= tradeBtnX + tradeBtnW && my >= tradeBtnY && my <= tradeBtnY + tradeBtnH) ||
	    (mx >= outSlotX && mx <= outSlotX + 24 && my >= outSlotY && my <= outSlotY + 24)) {
		bool ok = doTrade(this->minecraft, this->player, this->villager, this->selectedTrade, &this->placedInputItem, this->lastSourceSlot);
		if (ok) {
			ItemInstance outInst(tr.outputId, tr.outputCount, tr.outputMeta);
			char msgBuf[96];
			snprintf(msgBuf, sizeof(msgBuf), "Traded for %dx %s!", tr.outputCount, getItemDisplayName(outInst).c_str());
			this->statusMessage = msgBuf;
			this->statusColor = 0xFF77FF77;
			this->statusTimer = 60;
		} else {
			ItemInstance inInst(tr.inputId, tr.inputCount, tr.inputMeta);
			char msgBuf[96];
			snprintf(msgBuf, sizeof(msgBuf), "Need %dx %s to trade", tr.inputCount, getItemDisplayName(inInst).c_str());
			this->statusMessage = msgBuf;
			this->statusColor = 0xFFFF7777;
			this->statusTimer = 60;
		}
		if (this->minecraft->level) {
			this->minecraft->level->playSound(this->villager, ok ? "mob.villager.yes" : "mob.villager.no", 1.0f, 1.0f);
		}
		return;
	}

	int32_t doneW = 80;
	int32_t doneH = 18;
	int32_t doneY = y1 - 22;
	int32_t doneX = x0 + (this->dialogW - doneW) / 2;
	if (mx >= doneX && mx <= doneX + doneW && my >= doneY && my <= doneY + doneH) {
		this->minecraft->setScreen(0);
		return;
	}

	Screen::mouseClicked(mx, my, btn);
}

void VillagerTradeScreen::keyPressed(int32_t key) {
	if (key == 27 || key == 6) this->minecraft->setScreen(0);
}

bool_t VillagerTradeScreen::backPressed(Minecraft* mc, bool_t b) {
	if (mc) mc->setScreen(0);
	return 1;
}

void VillagerTradeScreen::render(int32_t mx, int32_t my, float f) {
	this->renderBackground(0);

	int32_t x0 = this->guiX;
	int32_t y0 = this->guiY;
	int32_t x1 = x0 + this->dialogW;
	int32_t y1 = y0 + this->dialogH;

	if (this->bgPanel) {
		this->bgPanel->setSize((float)this->dialogW, (float)this->dialogH);
		this->bgPanel->draw(Tesselator::instance, (float)x0, (float)y0);
	}

	int32_t prof = this->villager ? this->villager->getProfession() : 0;
	std::string title = getProfessionTitle(prof);
	int32_t tw = this->font->width(title);
	this->font->draw(title, (float)(x0 + (this->dialogW - tw) / 2), (float)(y0 + 7), 0xFF404040);

	int32_t closeX = x1 - 21;
	int32_t closeY = y0 + 3;
	bool closeHov = (mx >= closeX && mx <= closeX + 18 && my >= closeY && my <= closeY + 18);
	renderMcpeCloseBtn(this->minecraft, closeX, closeY, 18, closeHov);

	int32_t statusX = x0 + 8;
	int32_t statusY = y0 + 21;
	int32_t statusW = this->dialogW - 16;
	int32_t statusH = 16;
	if (this->insetPanel) {
		this->insetPanel->setSize((float)statusW, (float)statusH);
		this->insetPanel->draw(Tesselator::instance, (float)statusX, (float)statusY);
	}

	if (!this->villager) return;
	if (this->villager->tradeCount <= 0) this->villager->initTrades();
	int32_t cnt = this->villager->tradeCount;
	if (this->selectedTrade >= cnt) this->selectedTrade = cnt - 1;
	if (this->selectedTrade < 0) this->selectedTrade = 0;

	VillagerTrade& tr = this->villager->trades[this->selectedTrade];
	ItemInstance inInst(tr.inputId, tr.inputCount, tr.inputMeta);
	ItemInstance outInst(tr.outputId, tr.outputCount, tr.outputMeta);
	int32_t totalHave = countMatchingInInventory(this->player, tr.inputId, tr.inputMeta, &this->placedInputItem);
	bool canAfford = (totalHave >= tr.inputCount);

	std::string barText;
	uint32_t barCol = 0xFFE0E0E0;
	if (this->statusTimer > 0 && !this->statusMessage.empty()) {
		barText = this->statusMessage;
		barCol = this->statusColor;
	} else {
		char buf[128];
		snprintf(buf, sizeof(buf), "%dx %s -> %dx %s (%d/%d)",
		         tr.inputCount, getItemDisplayName(inInst).c_str(),
		         tr.outputCount, getItemDisplayName(outInst).c_str(),
		         std::min(totalHave, 999), tr.inputCount);
		barText = buf;
		barCol = canAfford ? 0xFF88FF88 : 0xFFFFCC66;
	}
	this->font->drawShadow(barText, statusX + 5, statusY + 4, barCol);

	int32_t leftX = x0 + 8;
	int32_t leftY = y0 + 40;
	int32_t leftW = 92;
	int32_t leftH = this->dialogH - 40 - 27;
	if (this->insetPanel) {
		this->insetPanel->setSize((float)leftW, (float)leftH);
		this->insetPanel->draw(Tesselator::instance, (float)leftX, (float)leftY);
	}

	int32_t listY = leftY + 4;
	int32_t listH = leftH - 8;
	int32_t itemH = 24;
	int32_t visibleCount = std::max(1, listH / itemH);

	for (int32_t i = 0; i < visibleCount; ++i) {
		int32_t tIdx = this->scrollOffset + i;
		if (tIdx >= cnt) break;
		int32_t ry = listY + i * itemH;
		int32_t rx = leftX + 3;
		int32_t rw = leftW - 6;

		bool hov = (mx >= rx && mx <= rx + rw && my >= ry && my <= ry + itemH - 2);
		bool active = (tIdx == this->selectedTrade);

		NinePatchLayer* rowBg = hov ? this->btnPressed : this->btnNormal;
		if (rowBg) {
			rowBg->setSize((float)rw, (float)(itemH - 2));
			rowBg->draw(Tesselator::instance, (float)rx, (float)ry);
		}
		if (active && this->slotSelected) {
			this->slotSelected->setSize((float)(rw + 2), (float)itemH);
			this->slotSelected->draw(Tesselator::instance, (float)(rx - 1), (float)(ry - 1));
		}

		VillagerTrade& rowTr = this->villager->trades[tIdx];
		ItemInstance rowIn(rowTr.inputId, rowTr.inputCount, rowTr.inputMeta);
		ItemInstance rowOut(rowTr.outputId, rowTr.outputCount, rowTr.outputMeta);
		int32_t rowHave = countMatchingInInventory(this->player, rowTr.inputId, rowTr.inputMeta,
		                                           (tIdx == this->selectedTrade) ? &this->placedInputItem : nullptr);
		bool rowCan = (rowHave >= rowTr.inputCount);

		renderSlotItem(this->font, this->minecraft->texturesPtr, &rowIn, (float)(rx + 6), (float)(ry + 3));
		this->font->drawShadow(rowCan ? "->" : "x", rx + 36, ry + 7, rowCan ? 0xFF88FF88 : 0xFFFF6666);
		renderSlotItem(this->font, this->minecraft->texturesPtr, &rowOut, (float)(rx + 58), (float)(ry + 3));
	}

	int32_t rightX = leftX + leftW + 4;
	int32_t rightY = leftY;
	int32_t rightW = this->dialogW - 16 - leftW - 4;
	int32_t rightH = leftH;
	if (this->insetPanel) {
		this->insetPanel->setSize((float)rightW, (float)rightH);
		this->insetPanel->draw(Tesselator::instance, (float)rightX, (float)rightY);
	}

	int32_t prevX = rightX + 4;
	int32_t navY = rightY + 4;
	int32_t navW = 20;
	int32_t navH = 16;
	bool canPrev = (this->selectedTrade > 0);
	bool prevHov = (canPrev && mx >= prevX && mx <= prevX + navW && my >= navY && my <= navY + navH);
	renderMcpeBtn(this->btnNormal, this->btnPressed, this->font, prevX, navY, navW, navH, "<<", prevHov, canPrev);

	char offerBuf[32];
	snprintf(offerBuf, sizeof(offerBuf), "Offer %d / %d", this->selectedTrade + 1, cnt);
	int32_t ow = this->font->width(offerBuf);
	this->font->drawShadow(offerBuf, rightX + (rightW - ow) / 2, navY + 4, 0xFFE0E0E0);

	int32_t nextX = rightX + rightW - 24;
	bool canNext = (this->selectedTrade < cnt - 1);
	bool nextHov = (canNext && mx >= nextX && mx <= nextX + navW && my >= navY && my <= navY + navH);
	renderMcpeBtn(this->btnNormal, this->btnPressed, this->font, nextX, navY, navW, navH, ">>", nextHov, canNext);

	int32_t tradeBoxY = rightY + 34;
	int32_t inSlotX = rightX + 18;
	int32_t inSlotY = tradeBoxY + 6;
	if (this->slotNormal) {
		this->slotNormal->setSize(24.0f, 24.0f);
		this->slotNormal->draw(Tesselator::instance, (float)inSlotX, (float)inSlotY);
	}
	renderSlotItem(this->font, this->minecraft->texturesPtr, &inInst, (float)(inSlotX + 4), (float)(inSlotY + 4));

	std::string arrowStr = "==>";
	int32_t aw = this->font->width(arrowStr);
	this->font->drawShadow(arrowStr, rightX + (rightW - aw) / 2, tradeBoxY + 14, canAfford ? 0xFF88FF88 : 0xFFFF6666);

	int32_t outSlotX = rightX + rightW - 42;
	int32_t outSlotY = tradeBoxY + 6;
	if (this->slotNormal) {
		this->slotNormal->setSize(24.0f, 24.0f);
		this->slotNormal->draw(Tesselator::instance, (float)outSlotX, (float)outSlotY);
	}
	renderSlotItem(this->font, this->minecraft->texturesPtr, &outInst, (float)(outSlotX + 4), (float)(outSlotY + 4));

	std::string inLabel = getItemDisplayName(inInst);
	int32_t inLw = this->font->width(inLabel);
	this->font->drawShadow(inLabel, rightX + (rightW - inLw) / 2, tradeBoxY + 38, 0xFFC8C8C8);

	std::string outLabel = getItemDisplayName(outInst);
	int32_t outLw = this->font->width(outLabel);
	this->font->drawShadow(outLabel, rightX + (rightW - outLw) / 2, tradeBoxY + 50, 0xFF88FF88);

	char haveBuf[64];
	if (this->player && this->player->inventory && this->player->inventory->field_20 != 0) {
		snprintf(haveBuf, sizeof(haveBuf), "Creative Mode");
	} else {
		snprintf(haveBuf, sizeof(haveBuf), "You have: %d / %d", totalHave, tr.inputCount);
	}
	int32_t hw = this->font->width(haveBuf);
	this->font->drawShadow(haveBuf, rightX + (rightW - hw) / 2, tradeBoxY + 64, canAfford ? 0xFF88FF88 : 0xFFFF7777);

	int32_t tradeBtnX = rightX + 14;
	int32_t tradeBtnY = rightY + rightH - 30;
	int32_t tradeBtnW = rightW - 28;
	int32_t tradeBtnH = 20;
	bool tradeHov = (mx >= tradeBtnX && mx <= tradeBtnX + tradeBtnW && my >= tradeBtnY && my <= tradeBtnY + tradeBtnH);
	uint32_t tradeCol = canAfford ? (tradeHov ? 0xFFFFFFA0 : 0xFF88FF88) : 0xFFA0A0A0;
	renderMcpeBtn(this->btnNormal, this->btnPressed, this->font, tradeBtnX, tradeBtnY, tradeBtnW, tradeBtnH, "Trade", tradeHov, canAfford, tradeCol);

	int32_t doneW = 80;
	int32_t doneH = 18;
	int32_t doneY = y1 - 22;
	int32_t doneX = x0 + (this->dialogW - doneW) / 2;
	bool doneHov = (mx >= doneX && mx <= doneX + doneW && my >= doneY && my <= doneY + doneH);
	renderMcpeBtn(this->btnNormal, this->btnPressed, this->font, doneX, doneY, doneW, doneH, "Done", doneHov);

	Screen::render(mx, my, f);
}
