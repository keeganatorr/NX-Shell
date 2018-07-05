#include <switch.h>

#include "common.h"
#include "config.h"
#include "fs.h"
#include "menu_txt.h"
#include "SDL_helper.h"
#include "status_bar.h"
#include "textures.h"
#include "touch_helper.h"
#include "utils.h"

static SDL_RWops* textFile = NULL;
static char* text = NULL;
static char* section = NULL;
static int screenlines = 20;
static int width = 0, height = 0;

char* Text_LoadFile(char* path) {
        textFile = SDL_RWFromFile(path, "rb");
        if (textFile == NULL) return NULL;
        Sint64 res_size = SDL_RWsize(textFile);
        char* res = (char*)malloc(res_size + 1);
        Sint64 nb_read_total = 0, nb_read = 1;
        char* buf = res;
        while (nb_read_total < res_size && nb_read != 0) {
                nb_read = SDL_RWread(textFile, buf, 1, (res_size - nb_read_total));
                nb_read_total += nb_read;
                buf += nb_read;
        }
        SDL_RWclose(textFile);
        if (nb_read_total != res_size) {
                free(res);
                return NULL;
        }

        res[nb_read_total] = '\0';
        return res;
}

void Menu_ShowText(char *path)
{

	int textStartX = 20;
	int textStartY = 144;

	int textposX = textStartX;
	int textposY = textStartY;	

	char* text = Text_LoadFile(path);

	int title_height = 0;
	TTF_SizeText(Roboto_large, path, NULL, &title_height);
	SDL_Rect textSize = SDL_GetTextSize(RENDERER, RobotoMono_large, textposX, textposY, WHITE, text); // fix for bigger files //

	int textX = textSize.x;
	int textY = textSize.y;
	int textWidth = textSize.w;
	int textHeight = textSize.h;

	printf("TotalWidth: %d, TotalHeight: %d\n",textWidth,textHeight);	
	
	TouchInfo touchInfo;
	Touch_Init(&touchInfo);


	int lineWidth,lineHeight;
	TTF_SizeText(RobotoMono_large,"a",&lineWidth,&lineHeight);

	int endY = ((-textHeight)+textStartY)+570;

	printf("Start: %d, End: %d\n",(textposY-textStartY),(endY-textStartY));

	int speed = lineHeight;

	float percent = 0;

	int textViewTop = 140;
	int textViewHeight = 580;

	int scrollbarWidth = 50;

	while(appletMainLoop())
	{
		//SDL_ClearScreen(RENDERER, SDL_MakeColour(33, 39, 43, 255));
		SDL_ClearScreen(RENDERER, config_dark_theme? BLACK_BG : WHITE);
		SDL_RenderClear(RENDERER);

		SDL_DrawText(RENDERER, RobotoMono_large, textposX, textposY, config_dark_theme? WHITE : BLACK, text);

		SDL_DrawRect(RENDERER, 0, 0, 1280, 40, config_dark_theme? STATUS_BAR_DARK : STATUS_BAR_LIGHT);
		StatusBar_DisplayTime();
		SDL_DrawRect(RENDERER, 0, 40, 1280, 100, config_dark_theme? MENU_BAR_DARK : MENU_BAR_LIGHT);	// Menu bar

		SDL_DrawImage(RENDERER, icon_back, 40, 66);
		SDL_DrawText(RENDERER, Roboto_large, 128, 40 + ((100 - title_height)/2), WHITE, path); // Audio filename

		hidScanInput();
		Touch_Process(&touchInfo);
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);

		percent = ((float)(-(textposY-textStartY))/(float)(-(endY-textStartY)));
		

		

		if(textHeight>570)
		{
			SDL_DrawRect(RENDERER, 1230, textViewTop, scrollbarWidth, textViewHeight, config_dark_theme? STATUS_BAR_DARK_TSP : STATUS_BAR_LIGHT_TSP);
			SDL_DrawRect(RENDERER, 1230, textViewTop+(percent*(textViewHeight-scrollbarWidth)), scrollbarWidth, scrollbarWidth, config_dark_theme? MENU_BAR_DARK_TSP : MENU_BAR_LIGHT_TSP);

			if ((kHeld & KEY_Y))
			{
				speed = lineHeight*2;
			}
			else
			{
				speed = lineHeight;
			}

			if ((kHeld & KEY_UP))
			{
				wait(1);
				textposY += speed;
			}
			else if ((kHeld & KEY_DOWN))
			{
				wait(1);
				textposY -= speed;
			}

			if ((kDown & KEY_LEFT) || (kDown & KEY_L))
			{
				wait(1);
				textposY += speed*16;
			}
			else if ((kDown & KEY_RIGHT) || (kDown & KEY_R))
			{
				wait(1);
				textposY -= speed*16;
			}

			if ((kDown & KEY_ZL))
			{
				wait(1);
				textposY = textStartY;
			}
			else if ((kDown & KEY_ZR))
			{
				wait(1);
				textposY = endY;
			}

			if(textposY > textStartY)
			{
				textposY = textStartY;
			}
			if(textposY < endY)
			{
				textposY = endY;
			}
		}

		if (touchInfo.state == TouchEnded && touchInfo.tapType != TapNone)
		{
			if (tapped_inside(touchInfo, 40, 66, 108, 114))
			{
				wait(1);
				break;
			}
		}

		SDL_RenderPresent(RENDERER);
		
		if (kDown & KEY_B)
			break;
	}
	MENU_DEFAULT_STATE = MENU_STATE_HOME;
}