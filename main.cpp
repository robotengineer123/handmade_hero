#include "SDL3/SDL_events.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstddef>
#include <iostream>

#define internal static
#define global_var static
#define local_persist static

constexpr char kWindowTitle[] = "Handmade Hero";
constexpr int kWinWidth = 1280;
constexpr int kWinHeight = 720;

global_var SDL_Texture *texture;
global_var void *texturePixels;

void SDLUpdateWindow(SDL_Renderer *renderer) {
  static bool isWhite = true;
  if (isWhite) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    isWhite = false;
  } else {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    isWhite = true;
  }
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
}

void resizeTexture(SDL_Renderer *renderer) {
  if (texture) {
    SDL_DestroyTexture(texture);
  }
  if (texturePixels) {
    free(texturePixels);
  }

  int width, height;
  SDL_GetWindowSize(window, width, height);
  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, width, height);
  texturePixels = malloc(width * height * 4);

  if (SDL_UpdateTexture(texture, 0, texturePixels, width * 4)) {
    // TODO: Handle error
  };
}

void handleEvent(bool &running, int winWidth, int winHeight,
                 const SDL_Event &event) {
  switch (event.type) {
  case SDL_EVENT_QUIT: {
    running = false;
  } break;
  case SDL_EVENT_KEY_DOWN: {
    if (event.key.key == SDLK_ESCAPE) {
      running = false;
    }
  } break;
  case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
    winWidth = event.window.data1;
    winHeight = event.window.data2;
  } break;
  case SDL_EVENT_WINDOW_EXPOSED: {
    SDL_Window *window = SDL_GetWindowFromEvent(&event);
    SDL_Renderer *renderer = SDL_GetRenderer(window);
    resizeTexture(renderer, width, height);
    SDLUpdateWindow(renderer);
  }
  default: {
  } break;
  }
}
int main(int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
    return 1;
  }

  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

  if (!SDL_CreateWindowAndRenderer(kWindowTitle, kWinWidth, kWinHeight,
                                   SDL_WINDOW_RESIZABLE, &window, &renderer)) {
    std::cerr << "Window creation failed: " << SDL_GetError() << '\n';

    SDL_Quit();
    return 1;
  }

  bool running = true;
  int winWidth = kWinWidth;
  int winHeight = kWinHeight;

  while (running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      handleEvent(running, winWidth, winHeight, event);
    }

    SDLUpdateWindow(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
