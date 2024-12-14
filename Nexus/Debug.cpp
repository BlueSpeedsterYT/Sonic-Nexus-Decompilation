#include "RetroEngine.hpp"

#if RETRO_USE_MOD_LOADER
int modOffset = 0;
#endif

void InitSystemMenu() {
    XScrollOffset = 0;
    YScrollOffset = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    PaletteMode = 0;

    if (Engine.UseBinFile
        || ((Engine.startList_Game != 0xFF && Engine.startList_Game) || (Engine.startStage_Game != 0xFF && Engine.startStage_Game))) {
        ClearGraphicsData();
        for (int i = 0; i < PLAYER_COUNT; ++i) PlayerScriptList[i].scriptPath[0] = 0;
        LoadPalette("Data/Palettes/MasterPalette.act", 0, 256);
        LoadPlayerFromList(0, 0);
        Engine.GameMode = ENGINE_MAINGAME;
        StageMode       = STAGEMODE_LOAD;
        ActiveStageList   = Engine.startList_Game == 0xFF ? 0 : Engine.startList_Game;
        StageListPosition = Engine.startStage_Game == 0xFF ? 0 : Engine.startStage_Game;
    } else {
        Engine.GameMode = ENGINE_SYSMENU;
        ClearGraphicsData();
        LoadPalette("Data/Palettes/MasterPalette.act", 0, 256);
        TextMenuSurfaceNo = 0;
        LoadGIFFile("Data/Game/SystemText.gif", 0);
        StageMode = DEVMENU_MAIN;
        SetupTextMenu(&GameMenu[0], 0);
        AddTextMenuEntry(&GameMenu[0], "RETRO SONIC DEFAULT MENU");
        AddTextMenuEntry(&GameMenu[0], " ");
        char Version[0x80];
        StrCopy(Version, Engine.GameWindowText);
        StrAdd(Version, " VERSION");
        AddTextMenuEntry(&GameMenu[0], Version);
        AddTextMenuEntry(&GameMenu[0], Engine.GameVersion);
#ifdef RETRO_DEV_EXTRA
        AddTextMenuEntry(&GameMenu[0], RETRO_DEV_EXTRA);
#else
        AddTextMenuEntry(&GameMenu[0], " ");
#endif
        AddTextMenuEntry(&GameMenu[0], " ");
        AddTextMenuEntry(&GameMenu[0], " ");
        AddTextMenuEntry(&GameMenu[0], " ");
        AddTextMenuEntry(&GameMenu[0], " ");
        AddTextMenuEntry(&GameMenu[0], "START GAME");
        AddTextMenuEntry(&GameMenu[0], " ");
        AddTextMenuEntry(&GameMenu[0], "1 PLAYER");
        AddTextMenuEntry(&GameMenu[0], " ");
#if RETRO_USE_MOD_LOADER
        AddTextMenuEntry(&GameMenu[0], "MODS");
        AddTextMenuEntry(&GameMenu[0], " ");
#endif
        AddTextMenuEntry(&GameMenu[0], "QUIT");
        GameMenu[0].alignment      = MENU_ALIGN_CENTER;
        GameMenu[0].selectionCount = 2;
        GameMenu[0].selection1     = 0;
        GameMenu[0].selection2     = 9;
    }
}
void InitErrorMessage()
{
    XScrollOffset = 0;
    YScrollOffset = 0;
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    PaletteMode = 0;
    Engine.GameMode = ENGINE_SYSMENU;
    ClearGraphicsData();
    LoadPalette("Data/Palettes/MasterPalette.act", 0, 256);
    TextMenuSurfaceNo = 0;
    LoadGIFFile("Data/Game/SystemText.gif", 0);
    StageMode = DEVMENU_SCRIPTERROR;
    GameMenu[0].alignment        = MENU_ALIGN_CENTER;
    GameMenu[0].selectionCount   = 1;
    GameMenu[0].selection1       = 0;
}
void ProcessSystemMenu() {
    ClearScreen(0xF0);
    GKeyDown.start = false;
    GKeyDown.B     = false;
    GKeyDown.up    = false;
    GKeyDown.down  = false;
    CheckKeyDown(&GKeyDown, 0xFF);
    CheckKeyPress(&GKeyPress, 0xFF);

    switch (StageMode) {
        case DEVMENU_MAIN: // Main Menu
        {
            if (GKeyPress.down)
                GameMenu[0].selection2 += 2;

            if (GKeyPress.up)
                GameMenu[0].selection2 -= 2;

            if (GameMenu[0].selection2 > (RETRO_USE_MOD_LOADER ? 15 : 13))
                GameMenu[0].selection2 = 9;
            if (GameMenu[0].selection2 < 9)
                GameMenu[0].selection2 = (RETRO_USE_MOD_LOADER ? 15 : 13);

            DrawTextMenu(&GameMenu[0], SCREEN_CENTERX, 72);
            if (GKeyPress.start || GKeyPress.A) {
                if (GameMenu[0].selection2 == 9) {
                    ClearGraphicsData();
                    ClearAnimationData();
                    LoadPalette("Data/Palettes/MasterPalette.act", 0, 256);
                    ActiveStageList   = 0;
                    StageMode         = STAGEMODE_LOAD;
                    Engine.GameMode   = ENGINE_MAINGAME;
                    StageListPosition = 0;
                }
                else if (GameMenu[0].selection2 == 11) {
                    SetupTextMenu(&GameMenu[0], 0);
                    AddTextMenuEntry(&GameMenu[0], "CHOOSE A PLAYER");
                    SetupTextMenu(&GameMenu[1], 0);
                    LoadConfigListText(&GameMenu[1], 0);
                    GameMenu[1].alignment      = MENU_ALIGN_LEFT;
                    GameMenu[1].selectionCount = 1;
                    GameMenu[1].selection1     = 0;
                    StageMode                  = DEVMENU_PLAYERSEL;
                }
#if RETRO_USE_MOD_LOADER
                else if (GameMenu[0].selection2 == 13) {
                    SetupTextMenu(&GameMenu[0], 0);
                    AddTextMenuEntry(&GameMenu[0], "MOD LIST");
                    SetupTextMenu(&GameMenu[1], 0);

                    char buffer[0x100];
                    int visible = modList.size() > 18 ? 18 : modList.size();
                    for (int m = 0; m < visible; ++m) {
                        StrCopy(buffer, modList[m].name.c_str());
                        StrAdd(buffer, " ");
                        StrAdd(buffer, modList[m].active ? "  Active" : "Inactive");
                        for (int c = 0; c < StrLength(buffer); ++c) buffer[c] = toupper(buffer[c]);
                        AddTextMenuEntry(&GameMenu[1], buffer);
                    }

                    modOffset                  = 0;
                    GameMenu[1].alignment      = MENU_ALIGN_RIGHT;
                    GameMenu[1].selectionCount = 3;
                    GameMenu[1].selection1     = 0;

                    GameMenu[0].alignment      = MENU_ALIGN_CENTER;
                    GameMenu[0].selectionCount = 1;
                    StageMode                  = DEVMENU_MODMENU;
                }
#endif
                else {
                    Engine.GameMode = ENGINE_EXITGAME;
                }
            }
            else if (GKeyPress.B && Engine.UseBinFile) {
                ClearGraphicsData();
                ClearAnimationData();
                LoadPalette("Data/Palettes/MasterPalette.act", 0, 256);
                ActiveStageList   = 0;
                StageMode         = STAGEMODE_LOAD;
                Engine.GameMode   = ENGINE_MAINGAME;
                StageListPosition = 0;
            }
            break;
        }
        case DEVMENU_PLAYERSEL: // Selecting Player
        {
            if (GKeyPress.down)
                ++GameMenu[1].selection1;
            if (GKeyPress.up)
                --GameMenu[1].selection1;
            if (GameMenu[1].selection1 == GameMenu[1].rowCount)
                GameMenu[1].selection1 = 0;

            if (GameMenu[1].selection1 < 0)
                GameMenu[1].selection1 = GameMenu[1].rowCount - 1;

            DrawTextMenu(&GameMenu[0], SCREEN_CENTERX - 4, 72);
            DrawTextMenu(&GameMenu[1], SCREEN_CENTERX - 40, 96);
            if (GKeyPress.start || GKeyPress.A) {
                LoadPlayerFromList(GameMenu[1].selection1, 0);
                SetupTextMenu(&GameMenu[0], 0);
                AddTextMenuEntry(&GameMenu[0], "SELECT A STAGE LIST");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "   PRESENTATION");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "   REGULAR");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "   SPECIAL");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "   BONUS");
                GameMenu[0].alignment  = MENU_ALIGN_LEFT;
                GameMenu[0].selection2 = 3;
                StageMode              = DEVMENU_STAGELISTSEL;
            }
            else if (GKeyPress.B) {
                StageMode = DEVMENU_MAIN;
                SetupTextMenu(&GameMenu[0], 0);
                AddTextMenuEntry(&GameMenu[0], "RETRO SONIC DEFAULT MENU");
                AddTextMenuEntry(&GameMenu[0], " ");
                char Version[0x80];
                StrCopy(Version, Engine.GameWindowText);
                StrAdd(Version, " Version");
                AddTextMenuEntry(&GameMenu[0], Version);
                AddTextMenuEntry(&GameMenu[0], Engine.GameVersion);
#ifdef RETRO_DEV_EXTRA
                AddTextMenuEntry(&GameMenu[0], RETRO_DEV_EXTRA);
#else
                AddTextMenuEntry(&GameMenu[0], " ");
#endif
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "START GAME");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "1 PLAYER");
                AddTextMenuEntry(&GameMenu[0], " ");
#if RETRO_USE_MOD_LOADER
                AddTextMenuEntry(&GameMenu[0], "MODS");
                AddTextMenuEntry(&GameMenu[0], " ");
#endif
                AddTextMenuEntry(&GameMenu[0], "QUIT");
                StageMode                  = DEVMENU_MAIN;
                GameMenu[0].alignment      = MENU_ALIGN_CENTER;
                GameMenu[0].selectionCount = 2;
                GameMenu[0].selection1     = 0;
                GameMenu[0].selection2     = 9;
            }
            break;
        }
        case DEVMENU_STAGELISTSEL: // Selecting Category
        {
            if (GKeyPress.down)
                GameMenu[0].selection2 += 2;
            if (GKeyPress.up)
                GameMenu[0].selection2 -= 2;

            if (GameMenu[0].selection2 > 9)
                GameMenu[0].selection2 = 3;

            if (GameMenu[0].selection2 < 3)
                GameMenu[0].selection2 = 9;

            DrawTextMenu(&GameMenu[0], SCREEN_CENTERX - 80, 72);
            bool nextMenu = false;
            switch (GameMenu[0].selection2) {
                case 3: // Presentation
                    if (stageListCount[0] > 0)
                        nextMenu = true;
                    ActiveStageList = 0;
                    break;
                case 5: // Regular
                    if (stageListCount[1] > 0)
                        nextMenu = true;
                    ActiveStageList = 1;
                    break;
                case 7: // Special
                    if (stageListCount[3] > 0)
                        nextMenu = true;
                    ActiveStageList = 3;
                    break;
                case 9: // Bonus
                    if (stageListCount[2] > 0)
                        nextMenu = true;
                    ActiveStageList = 2;
                    break;
                default: break;
            }

            if ((GKeyPress.start || GKeyPress.A) && nextMenu) {
                SetupTextMenu(&GameMenu[0], 0);
                AddTextMenuEntry(&GameMenu[0], "SELECT A STAGE");
                SetupTextMenu(&GameMenu[1], 0);
                LoadConfigListText(&GameMenu[1], ((GameMenu[0].selection2 - 3) >> 1) + 1);
                GameMenu[1].alignment      = MENU_ALIGN_RIGHT;
                GameMenu[1].selectionCount = 3;
                GameMenu[1].selection1     = 0;

                GameMenu[0].alignment      = MENU_ALIGN_CENTER;
                GameMenu[0].selectionCount = 1;
                StageMode                  = DEVMENU_STAGESEL;
            }
            else if (GKeyPress.B) {
                SetupTextMenu(&GameMenu[0], 0);
                AddTextMenuEntry(&GameMenu[0], "CHOOSE A PLAYER");
                SetupTextMenu(&GameMenu[1], 0);
                LoadConfigListText(&GameMenu[1], 0);
                GameMenu[0].alignment      = MENU_ALIGN_CENTER;
                GameMenu[1].alignment      = MENU_ALIGN_LEFT;
                GameMenu[1].selectionCount = 1;
                GameMenu[1].selection1     = 0;
                StageMode                  = DEVMENU_PLAYERSEL;
            }
            break;
        }
        case DEVMENU_STAGESEL: // Selecting Stage
        {
            if (GKeyPress.down)
                ++GameMenu[1].selection1;
            if (GKeyPress.up)
                --GameMenu[1].selection1;
            if (GameMenu[1].selection1 == GameMenu[1].rowCount)
                GameMenu[1].selection1 = 0;
            if (GameMenu[1].selection1 < 0)
                GameMenu[1].selection1 = GameMenu[1].rowCount - 1;

            DrawTextMenu(&GameMenu[0], SCREEN_CENTERX - 4, 40);
            DrawTextMenu(&GameMenu[1], SCREEN_CENTERX + 100, 64);
            if (GKeyPress.start || GKeyPress.A) {
                debugMode         = GKeyDown.A;
                StageMode         = STAGEMODE_LOAD;
                Engine.GameMode   = ENGINE_MAINGAME;
                StageListPosition = GameMenu[1].selection1;
            }
            else if (GKeyPress.B) {
                SetupTextMenu(&GameMenu[0], 0);
                AddTextMenuEntry(&GameMenu[0], "SELECT A STAGE LIST");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "   PRESENTATION");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "   REGULAR");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "   SPECIAL");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "   BONUS");
                GameMenu[0].alignment      = MENU_ALIGN_LEFT;
                GameMenu[0].selection2     = (ActiveStageList << 1) + 3;
                GameMenu[0].selection2     = GameMenu[0].selection2 == 7 ? 9 : GameMenu[0].selection2 == 9 ? 7 : GameMenu[0].selection2;
                GameMenu[0].selectionCount = 2;
                StageMode                  = DEVMENU_STAGELISTSEL;
            }
            break;
        }
        case DEVMENU_SCRIPTERROR: // Script Error
        {
            DrawTextMenu(&GameMenu[0], SCREEN_CENTERX, 72);
            if (GKeyPress.start || GKeyPress.A) {
                StageMode = DEVMENU_MAIN;
                SetupTextMenu(&GameMenu[0], 0);
                AddTextMenuEntry(&GameMenu[0], "RETRO SONIC DEFAULT MENU");
                AddTextMenuEntry(&GameMenu[0], " ");
                char Version[0x80];
                StrCopy(Version, Engine.GameWindowText);
                StrAdd(Version, " Version");
                AddTextMenuEntry(&GameMenu[0], Version);
                AddTextMenuEntry(&GameMenu[0], Engine.GameVersion);
#ifdef RETRO_DEV_EXTRA
                AddTextMenuEntry(&GameMenu[0], RETRO_DEV_EXTRA);
#else
                AddTextMenuEntry(&GameMenu[0], " ");
#endif
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "START GAME");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "1 PLAYER");
                AddTextMenuEntry(&GameMenu[0], " ");
#if RETRO_USE_MOD_LOADER
                AddTextMenuEntry(&GameMenu[0], "MODS");
                AddTextMenuEntry(&GameMenu[0], " ");
#endif
                AddTextMenuEntry(&GameMenu[0], "QUIT");
                GameMenu[0].alignment        = MENU_ALIGN_CENTER;
                GameMenu[0].selectionCount   = 2;
                GameMenu[0].selection1       = 0;
                GameMenu[0].selection2       = 9;
            }
            else if (GKeyPress.B) {
                ClearGraphicsData();
                ClearAnimationData();
                LoadPalette("Data/Palettes/MasterPalette.act", 0, 256);
                ActiveStageList   = 0;
                StageMode         = STAGEMODE_LOAD;
                Engine.GameMode   = ENGINE_MAINGAME;
                StageListPosition = 0;
            }
            else if (GKeyPress.C) {
                ClearGraphicsData();
                ClearAnimationData();
                StageMode         = STAGEMODE_LOAD;
                Engine.GameMode   = ENGINE_MAINGAME;
            }
            break;
        }
#if RETRO_USE_MOD_LOADER
        case DEVMENU_MODMENU: // Mod Menu
        {
            int preOption = GameMenu[1].selection1;
            if (GKeyPress.down)
                ++GameMenu[1].selection1;
            if (GKeyPress.up)
                --GameMenu[1].selection1;

            if (GameMenu[1].selection1 >= GameMenu[1].rowCount)
                GameMenu[1].selection1 = 0;
            if (GameMenu[1].selection1 < 0)
                GameMenu[1].selection1 = GameMenu[1].rowCount - 1;

            char buffer[0x100];
            if (GameMenu[1].selection1 < modList.size() && (GKeyPress.A || GKeyPress.start || GKeyPress.left || GKeyPress.right)) {
                modList[modOffset + GameMenu[1].selection1].active ^= 1;
                StrCopy(buffer, modList[modOffset + GameMenu[1].selection1].name.c_str());
                StrAdd(buffer, " ");
                StrAdd(buffer, (modList[modOffset + GameMenu[1].selection1].active ? "  Active" : "Inactive"));
                for (int c = 0; c < StrLength(buffer); ++c) buffer[c] = toupper(buffer[c]);
                EditTextMenuEntry(&GameMenu[1], buffer, GameMenu[1].selection1);
            }

            if (GKeyDown.C && GameMenu[1].selection1 != preOption) {
                int option         = GameMenu[1].selection1;
                ModInfo swap       = modList[preOption];
                modList[preOption] = modList[option];
                modList[option]    = swap;

                SetupTextMenu(&GameMenu[0], 0);
                AddTextMenuEntry(&GameMenu[0], "MOD LIST");
                SetupTextMenu(&GameMenu[1], 0);

                char buffer[0x100];
                for (int m = 0; m < modList.size(); ++m) {
                    StrCopy(buffer, modList[m].name.c_str());
                    StrAdd(buffer, " ");
                    StrAdd(buffer, modList[m].active ? "  Active" : "Inactive");
                    for (int c = 0; c < StrLength(buffer); ++c) buffer[c] = toupper(buffer[c]);
                    AddTextMenuEntry(&GameMenu[1], buffer);
                }
            }

            if (GKeyPress.B) {
                StageMode = DEVMENU_MAIN;
                SetupTextMenu(&GameMenu[0], 0);
                AddTextMenuEntry(&GameMenu[0], "RETRO SONIC DEFAULT MENU");
                AddTextMenuEntry(&GameMenu[0], " ");
                char Version[0x80];
                StrCopy(Version, Engine.GameWindowText);
                StrAdd(Version, " Version");
                AddTextMenuEntry(&GameMenu[0], Version);
                AddTextMenuEntry(&GameMenu[0], Engine.GameVersion);
#ifdef RETRO_DEV_EXTRA
                AddTextMenuEntry(&GameMenu[0], RETRO_DEV_EXTRA);
#else
                AddTextMenuEntry(&GameMenu[0], " ");
#endif
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "START GAME");
                AddTextMenuEntry(&GameMenu[0], " ");
                AddTextMenuEntry(&GameMenu[0], "1 PLAYER");
                AddTextMenuEntry(&GameMenu[0], " ");
#if RETRO_USE_MOD_LOADER
                AddTextMenuEntry(&GameMenu[0], "MODS");
                AddTextMenuEntry(&GameMenu[0], " ");
#endif
                AddTextMenuEntry(&GameMenu[0], "QUIT");
                GameMenu[0].alignment      = MENU_ALIGN_CENTER;
                GameMenu[0].selectionCount = 2;
                GameMenu[0].selection1     = 0;
                GameMenu[0].selection2     = 9;

                RefreshEngine();
            }

            DrawTextMenu(&GameMenu[0], SCREEN_CENTERX - 4, 40);
            DrawTextMenu(&GameMenu[1], SCREEN_CENTERX + 100, 64);
            break;
        }
#endif
        default: break;
    }
}