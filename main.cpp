#include "SDL3/SDL_events.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstddef>
#include <cstdint>
#include <iostream>

#define internal static
#define global_var static
#define local_persist static

constexpr char kWindowTitle[] = "Handmade Hero";
constexpr int kWinWidth = 1280;
constexpr int kWinHeight = 720;

global_var SDL_Texture *texture;
global_var void *texturePixels;
global_var int bitmapWidth;
global_var int bitmapHeight;

void SDLUpdateWindow(SDL_Renderer *renderer) {
  // SDL_RenderClear(renderer);
  SDL_RenderTexture(renderer, texture, 0, 0);
  SDL_RenderPresent(renderer);
}

void resizeTexture(SDL_Renderer *renderer, int width, int height) {
  if (texture) {
    SDL_DestroyTexture(texture);
  }
  if (texturePixels) {
    free(texturePixels);
  }

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, width, height);
  int bytesPerPixel = 4;
  texturePixels = malloc(width * height * bytesPerPixel);

  int pitch = width * bytesPerPixel;
  uint8_t *row = (uint8_t *)texturePixels;
  for (int i = 0; i < height; i++) {
    uint8_t *pixel = (uint8_t *)row;
    for (int j = 0; j < width; j++) {
      // Pixel in memory  BB GG RR AA;
      *pixel = (uint8_t)i; // Blue
      ++pixel;
      *pixel = (uint8_t)j; // Green
      ++pixel;
      *pixel = 0; // Red
      ++pixel;
      *pixel = 255; // Alpha
      ++pixel;
    }
    row += pitch;
  }

  if (SDL_UpdateTexture(texture, 0, texturePixels, width * bytesPerPixel)) {
    // TODO: Handle error
  };
}

void handleEvent(bool &running, const SDL_Event &event) {
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
    int winWidth = event.window.data1;
    int winHeight = event.window.data2;
  } break;
  case SDL_EVENT_WINDOW_EXPOSED: {
    SDL_Window *window = SDL_GetWindowFromEvent(&event);
    int width, height;
    SDL_Renderer *renderer = SDL_GetRenderer(window);
    SDL_GetWindowSize(window, &width, &height);
    resizeTexture(renderer, width, height);
    SDLUpdateWindow(renderer);
  } break;
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

  resizeTexture(renderer, winWidth, winHeight);
  SDLUpdateWindow(renderer);
  while (running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      handleEvent(running, event);
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
