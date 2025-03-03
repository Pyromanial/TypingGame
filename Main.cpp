//Hand image adapted from freepik
//Hourglass image adapted from vecteezy

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "stdio.h"
#include "string"

#include "UniTexture.h"

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Renderer* gRenderer = NULL;

const std::string textString = "Haiii!!!1!11! x3";
SDL_Color textColor = { 121,235,7 };
TTF_Font* gFont = NULL;

//!Make sure to include all necessary flags! here by or'ing them together (e.g 'imgInitFlags = IMG_INIT_JPG | IMG_INIT_PNG' )
const int sdlInitFlags = SDL_INIT_VIDEO;
const int imgInitFlags = IMG_INIT_JPG;
const int windowFlags = 0;
const int rendererFlags = SDL_RENDERER_PRESENTVSYNC;

//window creation values -- !See flags section for windowFlags!
const int window_w = 1000;
const int window_h = 500;
const int window_x = SDL_WINDOWPOS_UNDEFINED;
const int window_y = SDL_WINDOWPOS_UNDEFINED;
const char windowTitle[] = "Test";

bool init()
{
	bool success = true;
	if (SDL_Init(sdlInitFlags) < 0)
	{
		printf("Failed to initialize SDL subsystems! ERR:%s\n", SDL_GetError());
		success = false;
	}
	else if (!(IMG_Init(imgInitFlags) & imgInitFlags))
	{
		printf("Failed to initialize SDL_image subsystems! ERR:%s\n", IMG_GetError());
		success = false;
	}
	else if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		gWindow = SDL_CreateWindow(windowTitle, window_x, window_y, window_w, window_h, windowFlags);
		if (gWindow == NULL)
		{
			printf("Failed to create window! Err:%s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, rendererFlags);
			if (gRenderer == NULL)
			{
				printf("Failed to generate renderer from window! Err:%s\n", SDL_GetError());
				success = false;
			}
		}
	}
	return success;
}

void close()
{
	SDL_FreeSurface(gScreenSurface);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	//Destroy/free all other globals

	SDL_Quit();
}

int main(int argc, char* args[])
{
	if (!init())
	{
		printf("Could not initialize!");
	}
	else
	{
		gFont = TTF_OpenFont("lazy.ttf",18);

		UniTexture inputTextTexture;
		inputTextTexture.setRenderTarget(gRenderer);
		bool renderText = false;

		std::string inputText = "Enter Text Here";
		inputTextTexture.loadFromString(gFont, inputText.c_str(), textColor);

		SDL_StartTextInput();

		SDL_Event e;
		bool quit = false;
		while (!quit)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT)
					quit = true;
				//Special key input
				else if (e.type == SDL_KEYDOWN)
				{
					//Handle backspace
					if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
					{
						//lop off character
						inputText.pop_back();
						renderText = true;
					}
					//Handle copy
					else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
					{
						SDL_SetClipboardText(inputText.c_str());
					}
					//Handle paste
					else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
					{
						//Copy text from temporary buffer
						char* tempText = SDL_GetClipboardText();
						inputText = tempText;
						SDL_free(tempText);

						renderText = true;
					}
				}
				else if (e.type == SDL_TEXTINPUT)
				{
					//Not copy or pasting
					if (!(SDL_GetModState() & KMOD_CTRL && (e.text.text[0] == 'c' || e.text.text[0] == 'C' || e.text.text[0] == 'v' || e.text.text[0] == 'V')))
					{
						//Append character
						inputText += e.text.text;
						renderText = true;
					}
				}
			}
			//Rerender text if needed
			if (renderText)
			{
				//Text is not empty
				if (inputText != "")
				{
					//Render new text
					inputTextTexture.free();
					inputTextTexture.setRenderTarget(gRenderer);
					inputTextTexture.loadFromString(gFont, inputText.c_str(), textColor);
				}
				//Text is empty
				else
				{
					//Render space texture
					inputTextTexture.free();
					inputTextTexture.setRenderTarget(gRenderer);
					inputTextTexture.loadFromString(gFont, " ", textColor);
				}
			}
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);

			inputTextTexture.render((window_w - inputTextTexture.getWidth()) / 2, (window_h - inputTextTexture.getHeight()) / 2);

			SDL_RenderPresent(gRenderer);
		}

		inputTextTexture.free();
	}
	SDL_StopTextInput();

	TTF_CloseFont(gFont);

	close();

	return 0;
}