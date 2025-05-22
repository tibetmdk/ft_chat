#include "../includes/ft_chat.h"

int main()
{
	int		running;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("FT_CHAT",\
	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,\
	SDL_WINDOW_SHOWN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,\
	SDL_RENDERER_ACCELERATED);
	running = 1;
	SDL_Event event;
	while (running)
	{
		while (SDL_PollEvent(&event))
			if (event.type == SDL_QUIT)
				running = 0;
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}

	SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
	SDL_Quit();
	return (0);
}