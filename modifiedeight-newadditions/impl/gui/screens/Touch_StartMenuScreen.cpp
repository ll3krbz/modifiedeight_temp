#include <Minecraft.hpp>
#include <NinecraftApp.hpp>
#include <gui/NinePatchFactory.hpp>
#include <gui/NinePatchLayer.hpp>
#include <gui/buttons/ImageWithBackground.hpp>
#include <gui/screens/InvalidLicenseScreen.hpp>
#include <gui/screens/OptionsScreen.hpp>
#include <gui/screens/PatchErrorScreen.hpp>
#include <gui/screens/PlayScreen.hpp>
#include <gui/screens/Touch_StartMenuScreen.hpp>
#include <network/mco/MojangConnector.hpp>
#include <patch/PatchManager.hpp>
#include <rendering/Font.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <util/Common.hpp>
#include <cmath>
#include <algorithm>
#include <string>
#include <util/IntRectangle.hpp>

char_t *gSplashes[] = {"Scientific!",
                       "Cooler than Spock!",
                       "Collaborate and listen!",
                       "Never dig down!",
                       "Take frequent breaks!",
                       "Not linear!",
                       "Han shot first!",
                       "Nice to meet you!",
                       "Buckets of lava!",
                       "Ride the pig!",
                       "Larger than Earth!",
                       "sqrt(-1) love you!",
                       "Phobos anomaly!",
                       "Punching wood!",
                       "Falling off cliffs!",
                       "0% sugar!",
                       "150% hyperbole!",
                       "Synecdoche!",
                       "Let's danec!",
                       "Seecret Friday update!",
                       "Ported implementation!",
                       "Lewd with two dudes with food!",
                       "Kiss the sky!",
                       "20 GOTO 10!",
                       "Verlet intregration!",
                       "Peter Griffin!",
                       "Do not distribute!",
                       "Cogito ergo sum!",
                       "4815162342 lines of code!",
                       "A skeleton popped out!",
                       "The Work of Notch!",
                       "The sum of its parts!",
                       "BTAF used to be good!",
                       "I miss ADOM!",
                       "umop-apisdn!",
                       "OICU812!",
                       "Bring me Ray Cokes!",
                       "Finger-licking!",
                       "Thematic!",
                       "Pneumatic!",
                       "Sublime!",
                       "Octagonal!",
                       "Une baguette!",
                       "Gargamel plays it!",
                       "Rita is the new top dog!",
                       "SWM forever!",
                       "Representing Edsbyn!",
                       "Matt Damon!",
                       "Supercalifragilisticexpialidocious!",
                       "Consummate V's!",
                       "Cow Tools!",
                       "Double buffered!",
                       "V-synched!",
                       "Fan fiction!",
                       "Flaxkikare!",
                       "Jason! Jason! Jason!",
                       "Hotter than the sun!",
                       "Internet enabled!",
                       "Autonomous!",
                       "Engage!",
                       "Fantasy!",
                       "DRR! DRR! DRR!",
                       "Kick it root down!",
                       "Regional resources!",
                       "Woo, facepunch!",
                       "Woo, somethingawful!",
                       "Woo, /v/!",
                       "Woo, tigsource!",
                       "Woo, minecraftforum!",
                       "Woo, worldofminecraft!",
                       "Woo, reddit!",
                       "Woo, 2pp!",
                       "Google anlyticsed!",
                       "Give us Gordon!",
                       "Tip your waiter!",
                       "Very fun!",
                       "12345 is a bad password!",
                       "Vote for net neutrality!",
                       "Lives in a pineapple under the sea!",
                       "Omnipotent!",
                       "Gasp!",
                       "...!",
                       "Bees, bees, bees, bees!",
                       "Haha, LOL!",
                       "Hampsterdance!",
                       "Switches and ores!",
                       "Menger sponge!",
                       "idspispopd!",
                       "Eple (original edit)!",
                       "So fresh, so clean!",
                       "Don't look directly at the bugs!",
                       "Oh, ok, Pigmen!",
                       "Scary!",
                       "Play Minecraft, Watch Topgear, Get Pig!",
                       "Twittered about!",
                       "Jump up, jump up, and get down!",
                       "Joel is neat!",
                       "A riddle, wrapped in a mystery!",
                       "Huge tracts of land!",
                       "Welcome to your Doom!",
                       "Stay a while, stay forever!",
                       "Stay a while and listen!",
                       "Treatment for your rash!",
                       "\"Autological\" is!",
                       "Information wants to be free!",
                       "\"Almost never\" is an interesting concept!",
                       "Lots of truthiness!",
                       "The creeper is a spy!",
                       "It's groundbreaking!",
                       "Let our battle's begin!",
                       "The sky is the limit!",
                       "Jeb has amazing hair!",
                       "Casual gaming!",
                       "Undefeated!",
                       "Kinda like Lemmings!",
                       "Follow the train, CJ!",
                       "Leveraging synergy!",
                       "DungeonQuest is unfair!",
                       "110813!",
                       "90210!",
                       "Tyrion would love it!",
                       "Also try VVVVVV!",
                       "Also try Super Meat Boy!",
                       "Also try Terraria!",
                       "Also try Mount And Blade!",
                       "Also try Project Zomboid!",
                       "Also try World of Goo!",
                       "Also try Limbo!",
                       "Also try Pixeljunk Shooter!",
                       "Also try Braid!",
                       "That's super!",
                       "Bread is pain!",
                       "Read more books!",
                       "Khaaaaaaaaan!",
                       "Less addictive than TV Tropes!",
                       "More addictive than lemonade!",
                       "Bigger than a bread box!",
                       "Millions of peaches!",
                       "Fnord!",
                       "This is my true form!",
                       "Totally forgot about Dre!",
                       "Don't bother with the clones!",
                       "Pumpkinhead!",
                       "Hobo humping slobo babe!",
                       "Endless!",
                       "Feature packed!",
                       "Boots with the fur!",
                       "Stop, hammertime!",
                       "Conventional!",
                       "Homeomorphic to a 3-sphere!",
                       "Doesn't avoid double negatives!",
                       "Place ALL the blocks!",
                       "Does barrel rolls!",
                       "Meeting expectations!",
                       "PC gaming since 1873!",
                       "Ghoughpteighbteau tchoghs!",
                       "Got your nose!",
                       "Haley loves Elan!",
                       "Afraid of the big, black bat!",
                       "Doesn't use the U-word!",
                       "Child's play!",
                       "See you next Friday or so!",
                       "150 bpm for 400000 minutes!",
                       "Technologic!",
                       "Funk soul brother!",
                       "Pumpa kungen!",
                       "Helo Cymru!",
                       "My life for Aiur!",
                       "Lennart lennart = new Lennart();",
                       "I see your vocabulary has improved!",
                       "Who put it there?",
                       "You can't explain that!",
                       "if not ok then return end",
                       "SOPA means LOSER in Swedish!",
                       "Big Pointy Teeth!",
                       "Bekarton guards the gate!",
                       "Mmmph, mmph!",
                       "Don't feed avocados to parrots!",
                       "Swords for everyone!",
                       "Plz reply to my tweet!",
                       ".party()!",
                       "Take her pillow!",
                       "Put that cookie down!",
                       "Pretty scary!",
                       "I have a suggestion.",
                       "Now with extra hugs!",
                       "Almost C++11!",
                       "Woah.",
                       "HURNERJSGER?",
                       "What's up, Doc?",
                       "1 star! Deal with it notch!",
                       "100% more yellow text!",
                       "Glowing creepy eyes!",
                       "Toilet friendly!",
                       "Annoying touch buttons!",
                       "Astronomically accurate!",
                       "0xffff-1 chunks!",
                       "Cubism!",
                       "Pocket!",
                       "Mostly harmless!",
                       "!!!1!",
                       "Dramatic lighting!",
                       "As seen on TV!",
                       "Awesome!",
                       "100% pure!",
                       "May contain nuts!",
                       "Better than Prey!",
                       "Less polygons!",
                       "Sexy!",
                       "Limited edition!",
                       "Flashing letters!",
                       "Made by Mojang!",
                       "It's here!",
                       "Best in class!",
                       "It's alpha!",
                       "100% dragon free!",
                       "Excitement!",
                       "More than 500 sold!",
                       "One of a kind!",
                       "Heaps of hits on YouTube!",
                       "Indev!",
                       "Spiders everywhere!",
                       "Check it out!",
                       "Holy cow, man!",
                       "It's a game!",
                       "Made in Sweden!",
                       "Uses C++!",
                       "Reticulating splines!",
                       "Minecraft!",
                       "Yaaay!",
                       "Multiplayer!",
                       "Touch compatible!",
                       "Undocumented!",
                       "Ingots!",
                       "Exploding creepers!",
                       "That's no moon!",
                       "l33t!",
                       "No soul.",
                       "Create!",
                       "Survive!",
                       "Dungeon!",
                       "Exclusive!",
                       "The bee's knees!",
                       "Down with O.P.P.!",
                       "Open source!",
                       "Classy!",
                       "Wow!",
                       "Not on steam!",
                       "Oh man!",
                       "Awesome community!",
                       "Pixels!",
                       "Teetsuuuuoooo!",
                       "Kaaneeeedaaaa!",
                       "Enhanced!",
                       "90% bug free!",
                       "Pretty!",
                       "12 herbs and spices!",
                       "Fat free!",
                       "Absolutely no memes!",
                       "Free dental!",
                       "Ask your doctor!",
                       "Minors welcome!",
                       "Cloud computing!",
                       "Legal in Finland!",
                       "Hard to label!",
                       "Technically good!",
                       "Bringing home the bacon!",
                       "Quite Indie!",
                       "GOTY!",
                       "Euclidian!",
                       "Now in 3D!",
                       "Inspirational!",
                       "Herregud!",
                       "Complex cellular automata!",
                       "Yes, sir!",
                       "Played by cowboys!",
                       "OpenGL ES 1.1!",
                       "Thousands of colors!",
                       "Try it!",
                       "Age of Wonders is better!",
                       "Try the mushroom stew!",
                       "Sensational!",
                       "Hot tamale, hot hot tamale!",
                       "Play him off, keyboard cat!",
                       "Guaranteed!",
                       "Macroscopic!",
                       "Bring it on!",
                       "Random splash!",
                       "Call your mother!",
                       "Monster infighting!",
                       "Loved by millions!",
                       "Ultimate edition!",
                       "Freaky!",
                       "You've got a brand new key!",
                       "Water proof!",
                       "Uninflammable!",
                       "Whoa, dude!",
                       "All inclusive!",
                       "Tell your friends!",
                       "NP is not in P!",
                       "Notch <3 ez!",
                       "Livestreamed!",
                       "Haunted!",
                       "Polynomial!",
                       "Terrestrial!",
                       "All is full of love!",
                       "Full of stars!",
                       0};
int32_t Touch::StartMenuScreen::currentSplash = -1;

Touch::StartMenuScreen::StartMenuScreen()
    : playButton(2, "Play", 0), playOnRealmsButton(3, "Play on Realms", 0),
      buyButton(5) {
  this->settingsButtonMaybe = 0;
  this->field_140 = 0;
  this->field_148 = 0;
  this->field_14C = 1;
  this->field_144 = 0;
  this->field_150 = 0;
}

void Touch::StartMenuScreen::_updateLicense() {
  bool_t v4;
  int32_t licenseId = this->minecraft->getLicenseId();
  if (licenseId < 0)
    v4 = 0;
  else {
    if ((uint32_t)licenseId > 1) {
      this->minecraft->setScreen(new InvalidLicenseScreen(
          licenseId,
          this->minecraft->platform()->hasBuyButtonWhenInvalidLicense()));
      return;
    }
    v4 = 1;
  }
  this->playOnRealmsButton.active = v4;
  this->playButton.active = v4;
}

void Touch::StartMenuScreen::chooseRandomSplash() {
  Random rng;
  int32_t splashcnt = 0;
  while (gSplashes[splashcnt])
    ++splashcnt;
  Touch::StartMenuScreen::currentSplash = rng.genrand_int32() % splashcnt;
}
void Touch::StartMenuScreen::setupPlayButtons(bool_t a2) {
  int32_t width, height;
  int32_t v4, v5, v6;
  width = this->width;
  this->playButton.width = 100;
  this->playButton.height = 30;
  height = this->height;
  v4 = width / 2 - 50;
  this->playButton.posX = v4;
  v5 = height / 2;

  this->playOnRealmsButton.width = 100;
  this->playOnRealmsButton.height = 30;
  this->playOnRealmsButton.posX = v4;

  if (a2)
    v6 = v5 - 15;
  else
    v6 = v5 + 10;

  this->playButton.posY = v6;
  this->playOnRealmsButton.posY = v6 + 40;
}

Touch::StartMenuScreen::~StartMenuScreen() {
  if (this->settingsButtonMaybe) {
    delete this->settingsButtonMaybe;
    this->settingsButtonMaybe = 0;
  }
}
#include <item/ItemInstance.hpp>
#include <rendering/entity/ItemRenderer.hpp>

static int32_t rainbowLogoColor(float time, int32_t index) {
  (void)time;
  (void)index;
  return (int32_t)0xffcc5500u;
}

static void drawRainbowLogoText(Font* font, const char* text, float x, float y, float scale, float time) {
  float cursor = 0.0f;
  for (int32_t i = 0; text[i] != '\0'; ++i) {
    std::string glyph(1, text[i]);
    int32_t color = rainbowLogoColor(time, i);
    glPushMatrix();
    glTranslatef(x + cursor * scale, y, 0.0f);
    glScalef(scale, scale, scale);
    font->drawShadow(glyph, 0.0f, 0.0f, color);
    glPopMatrix();
    cursor += (float)font->width(glyph) + 0.5f;
  }
}

static float rainbowLogoTextWidth(Font* font, const char* text, float scale) {
  float cursor = 0.0f;
  for (int32_t i = 0; text[i] != '\0'; ++i) {
    std::string glyph(1, text[i]);
    cursor += (float)font->width(glyph) + 0.5f;
  }
  return cursor * scale;
}

void Touch::StartMenuScreen::render(int32_t a2, int32_t a3, float a4) {
  bool_t mco = this->minecraft->mojangConnector->isServiceEnabled();
  this->playOnRealmsButton.setActiveAndVisibility(mco);
  this->setupPlayButtons(mco);
  this->renderMenuBackground(a4);
  glEnable(GL_TEXTURE_2D);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  TextureData* td = this->minecraft->texturesPtr->loadAndGetTextureData("gui/title.png");
  if (td) {
    this->minecraft->texturesPtr->loadAndBindTexture("gui/title.png");
    if (this->field_148 <= 0.0f || this->field_14C <= 0.0f) {
      float halfWidth = (float)td->width * 0.5f;
      if ((float)this->width * 0.5f <= halfWidth) halfWidth = (float)this->width * 0.5f;
      this->field_148 = halfWidth * 2.0f;
      this->field_140 = (float)this->width * 0.5f - halfWidth;
      this->field_144 = 12.0f;
      this->field_14C = (this->field_148 / (float)td->width) * (float)td->height;
    }
    Tesselator::instance.begin(4);
    Tesselator::instance.vertexUV(this->field_140, this->field_144 + this->field_14C, this->zLayer, 0, 1);
    Tesselator::instance.vertexUV(this->field_140 + this->field_148, this->field_144 + this->field_14C, this->zLayer, 1, 1);
    Tesselator::instance.vertexUV(this->field_140 + this->field_148, this->field_144, this->zLayer, 1, 0);
    Tesselator::instance.vertexUV(this->field_140, this->field_144, this->zLayer, 0, 0);
    Tesselator::instance.draw(1);
  }
  float logoRight = this->field_140 + this->field_148;
  float logoBottom = this->field_144 + this->field_14C;
  this->drawString(this->font, this->field_138, 1, this->height - 10, 0xffffff);
  this->drawString(this->font, this->field_13C, 1, this->height - 20, 0xffffff);
  if (Touch::StartMenuScreen::currentSplash == -1) {
    Touch::StartMenuScreen::chooseRandomSplash();
  }
  double v17 = this->field_150 + this->minecraft->field_D34;
  this->field_150 = v17;

  float scale = (float)(powf(sin(v17 * 3.14 * 2.3), 4.0) * 0.035) + 1.16f;
  this->font->drawTransformed(
      gSplashes[Touch::StartMenuScreen::currentSplash], logoRight * 0.66f, logoBottom - 15.0f,
      0xFFFFFF00, -20, scale,
      1, (float)this->width * 0.34f);
  Screen::render(a2, a3, a4);
  if (PatchManager::instance().isBuildRunning() && this->font) {
    this->fill(0, 0, this->width, this->height, 0x99000000);
    int dw = std::min(264, this->width - 24);
    int dh = 76;
    int dx = (this->width - dw) / 2;
    int dy = (this->height - dh) / 2;
    int barX0 = dx + 18;
    int barX1 = dx + dw - 18;
    int barW = barX1 - barX0;
    int py = dy + dh - 26;
    int barH = 12;
    static float s_dispProg = 0.0f;
    float targetProg = PatchManager::instance().getBuildProgress();
    if (targetProg < 0.04f) targetProg = 0.04f;
    if (targetProg > 1.0f) targetProg = 1.0f;
    if (targetProg < s_dispProg - 0.25f) s_dispProg = targetProg;
    if (s_dispProg < targetProg) {
      s_dispProg += (targetProg - s_dispProg) * 0.14f;
      if (s_dispProg > targetProg) s_dispProg = targetProg;
    } else if (s_dispProg < 0.96f) {
      s_dispProg += 0.0005f;
    }
    int fillW = (int)(barW * s_dispProg + 0.5f);
    if (fillW < 2) fillW = 2;
    if (fillW > barW) fillW = barW;

    if (!this->minecraft->options.classicGUI) {
      int32_t colorA, colorB;
      this->minecraft->options.getNeonColors(colorA, colorB);
      int32_t glowCol = (colorA & 0x00FFFFFF) | 0x44000000;
      this->fill(dx - 2, dy - 2, dx + dw + 2, dy + dh + 2, glowCol);
      this->fillGradient(dx + 1, dy + 1, dx + dw - 1, dy + dh - 1, 0xF00D0214, 0xF01E042D);
      this->fill(dx, dy, dx + dw, dy + 1, colorA);
      this->fill(dx, dy + dh - 1, dx + dw, dy + dh, colorA);
      this->fill(dx, dy, dx + 1, dy + dh, colorA);
      this->fill(dx + dw - 1, dy, dx + dw, dy + dh, colorA);
      this->fill(dx + 8, dy + 20, dx + dw - 8, dy + 21, (colorB & 0x00FFFFFF) | 0x88000000);
      this->fill(barX0 - 2, py - 2, barX1 + 2, py + barH + 2, 0xAA000000);
      this->fill(barX0 - 1, py - 1, barX1 + 1, py + barH + 1, (colorA & 0x00FFFFFF) | 0xAA000000);
      this->fill(barX0, py, barX1, py + barH, 0xFF140820);
      this->fillGradient(barX0, py, barX0 + fillW, py + barH, colorA, colorB);
      this->fill(barX0, py, barX0 + fillW, py + 2, 0x55FFFFFF);
    } else {
      NinePatchFactory npf(this->minecraft->texturesPtr, "gui/spritesheet.png");
      NinePatchLayer* dlgBg = npf.createSymmetrical({34, 43, 14, 14}, 3, 3, (float)dw, (float)dh);
      NinePatchLayer* insetBg = npf.createSymmetrical({0, 32, 8, 8}, 3, 3, (float)(dw - 14), (float)(dh - 26));
      if (dlgBg) {
        dlgBg->draw(Tesselator::instance, (float)dx, (float)dy);
        delete dlgBg;
      }
      if (insetBg) {
        insetBg->draw(Tesselator::instance, (float)(dx + 7), (float)(dy + 20));
        delete insetBg;
      }
      this->fill(barX0 - 2, py - 2, barX1 + 2, py + barH + 2, 0xFF141414);
      this->fill(barX0 - 1, py - 1, barX1 + 1, py + barH + 1, 0xFF3E3E3E);
      this->fill(barX0, py, barX1, py + barH, 0xFF202020);
      this->fill(barX0, py, barX0 + fillW, py + barH, 0xFF4CC44C);
      this->fill(barX0, py, barX0 + fillW, py + 2, 0xFF88FF88);
      this->fill(barX0, py + barH - 2, barX0 + fillW, py + barH, 0xFF2B8A2B);
      if (fillW > 2) {
        this->fill(barX0 + fillW - 1, py, barX0 + fillW, py + barH, 0xFF9EFF9E);
      }
    }
    this->drawCenteredString(this->font, "Applying Patches...", this->width / 2, dy + 7, 0xFFFFFFA0);
    std::string st = PatchManager::instance().getBuildStatus();
    while (st.size() > 3 && this->font->width(st) > dw - 28) {
      st.resize(st.size() - 4);
      st += "...";
    }
    this->drawCenteredString(this->font, st, this->width / 2, dy + 28, 0xFFE6E6E6);
    int pct = (int)(s_dispProg * 100.0f + 0.5f);
    if (pct > 100) pct = 100;
    char pctBuf[16];
    snprintf(pctBuf, sizeof(pctBuf), "%d%%", pct);
    this->drawCenteredString(this->font, pctBuf, this->width / 2, py + 2, 0xFFFFFFFF);
  }
}
void Touch::StartMenuScreen::init() {
  this->playButton.init(this->minecraft);
  this->playOnRealmsButton.init(this->minecraft);
  this->settingsButtonMaybe = new ImageWithBackground(4);
  IntRectangle v8, v12;
  ImageDef v13;
  // v13.field_0 = 120; wat *(_QWORD *)&v13.field_0 = 120i64;
  v12.width = 0x00000008; // XXX *(_QWORD *)&v12.width = 0x4300000008i64;
  v12.height = 0x43;
  v13.field_8 = 0x00000008; // XXX *(_QWORD *)&v13.field_8 = 0x4300000008i64;
  v13.width = 0x43;
  v12.minX = 112;
  v12.minY = 0;
  v8.minX = 0x78;
  v8.minY = 0;
  v8.width = 8;
  v8.height = 0x43;
  this->settingsButtonMaybe->init(this->minecraft->texturesPtr, 32, 32, v12, v8,
                                  2, 2, "gui/spritesheet.png");
  v13.field_1C = 1;
  v13.field_20 = 1;
  v13.field_0 =
      ""; //*(_QWORD *)&v13.field_0 = (unsigned int)&_used_as_empty_string__[8];
  v13.field_4 = 0;
  v13.field_8 = 0x00000000;
  v13.width = 16;
  v13.height = 16;
  v13.field_14 = 0;
  v13.field_18 = 0;
  v13.field_24 = 0;
  v13.field_0 = "gui/touchgui.png";
  v13.field_24 = 1;
  v13.width = 22;
  v13.field_18 = 0;
  v13.height = 21;
  v13.field_14 = 218;
  v13.field_1C = 22;
  v13.field_20 = 21;
  this->settingsButtonMaybe->setImageDef(v13, 0);
  this->settingsButtonMaybe->width = 32;
  this->settingsButtonMaybe->height = 32;
  this->buttons.emplace_back(&this->playButton);
  this->buttons.emplace_back(&this->playOnRealmsButton);
  this->buttons.emplace_back(this->settingsButtonMaybe);
  this->field_2C.emplace_back(&this->playButton);
  this->field_2C.emplace_back(&this->playOnRealmsButton);
  this->field_2C.emplace_back(this->settingsButtonMaybe);
  this->field_138 = "\x0fMojang AB, ModifiedEight New Additions 1.8.0.1_build8 ";
  this->field_13C = Common::getGameVersionString();
  this->playOnRealmsButton.active = 0;
  this->playButton.active = 0;
}

void Touch::StartMenuScreen::setupPositions() {
  TextureData *data = this->minecraft->texturesPtr->loadAndGetTextureData("gui/title.png");
  int32_t width = this->width;
  this->field_144 = 12;
  float halfWidth = data ? data->width * 0.5f : width * 0.5f;
  if (width * 0.5f <= halfWidth) halfWidth = width * 0.5f;
  this->field_148 = halfWidth * 2.0f;
  this->field_140 = width * 0.5f - halfWidth;
  this->field_14C = data ? (this->field_148 / data->width) * data->height : 36.0f;

  bool_t mco = 1;
  this->setupPlayButtons(mco);
  this->settingsButtonMaybe->height = 32;
  this->settingsButtonMaybe->width = 32;
  this->settingsButtonMaybe->posX =
      this->width - this->settingsButtonMaybe->width - 2;
  this->settingsButtonMaybe->posY =
      this->height - this->settingsButtonMaybe->height - 2;
  int32_t v16b = this->width;
  this->buyButton.posY = this->height - this->buyButton.height - 3;
  this->buyButton.posX = (v16b - this->buyButton.width) / 2;
}
bool_t Touch::StartMenuScreen::handleBackEvent(bool_t) {
  this->minecraft->quit();
  return 1;
}
void Touch::StartMenuScreen::tick() {
  this->_updateLicense();
  PatchManager& pm = PatchManager::instance();
  bool ok = false, willRelaunch = false;
  std::string err;
  if (pm.pollAsyncResult(ok, willRelaunch, err)) {
    if (!ok) {
      this->minecraft->setScreen(new PatchErrorScreen(err, PatchErrorScreen::RETURN_START_MENU, false));
      return;
    }
    if (willRelaunch) {
      if (!pm.relaunchPendingBinary()) {
        this->minecraft->setScreen(new PatchErrorScreen(pm.getLastError(), PatchErrorScreen::RETURN_START_MENU, false));
      }
      return;
    }
    if (pm.hasEnabledAssetOverrides()) {
      pm.applyRuntimeAssetReload((NinecraftApp*)this->minecraft);
    }
  } else if (!pm.isBuildRunning()) {
    static bool s_checkedStartupPatchErr = false;
    if (!s_checkedStartupPatchErr) {
      s_checkedStartupPatchErr = true;
      if (pm.hasError()) {
        this->minecraft->setScreen(new PatchErrorScreen(pm.getLastError(), PatchErrorScreen::RETURN_START_MENU, false));
        return;
      }
    }
  }
}
bool_t Touch::StartMenuScreen::isInGameScreen() { return 0; }
void Touch::StartMenuScreen::buttonClicked(struct Button *button) {
  if (PatchManager::instance().isBuildRunning()) return;
  if (button->buttonID == this->playButton.buttonID) {
    this->minecraft->setScreen(new PlayScreen(1));
    return;
  }
  if (button->buttonID == this->playOnRealmsButton.buttonID) {
    this->minecraft->setScreen(new PlayScreen(0));
    return;
  }
  if (button->buttonID == this->settingsButtonMaybe->buttonID) {
    this->minecraft->setScreen(new OptionsScreen(0));
    return;
  }
  if (button->buttonID == this->buyButton.buttonID) {
    this->minecraft->platform()->buyGame();
  }
}
