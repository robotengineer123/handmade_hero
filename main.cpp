#include "SDL3/SDL_events.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>

#define internal static
#define global_var static
#define local_persist static

constexpr char kWindowTitle[] = "Handmade Hero";
constexpr int kWinWidth = 1280;
constexpr int kWinHeight = 720;

// The texture is SDL_PIXELFORMAT_ARGB8888
typedef struct {
  SDL_Texture *texture;
  void *texturePixels;
  int width;
  int height;
  int pitch;
} sdl_offscreen_buffer;

typedef struct {
  int width;
  int height;
} sdl_window_dimensions;

sdl_window_dimensions SDLGetWindowDimensions(SDL_Window *window) {
  sdl_window_dimensions dims;
  SDL_GetWindowSizeInPixels(window, &dims.width, &dims.height);
  return dims;
}

void SDLUpdateWindow(SDL_Window *window, sdl_offscreen_buffer BackBuffer) {
  SDL_Renderer *renderer = SDL_GetRenderer(window);
  if (SDL_UpdateTexture(BackBuffer.texture, 0, BackBuffer.texturePixels,
                        BackBuffer.pitch)) {
    // TODO: Handle error
  };
  SDL_RenderTexture(renderer, BackBuffer.texture, 0, 0);
  SDL_RenderPresent(renderer);
}

void renderWierdGradientPattern(sdl_offscreen_buffer BackBuffer,
                                int xOffset = 0, int yOffset = 0) {
  uint8_t *row = (uint8_t *)BackBuffer.texturePixels;
  for (int i = 0; i < BackBuffer.height; i++) {
    uint32_t *pixel = (uint32_t *)row;
    for (int j = 0; j < BackBuffer.width; j++) {
      uint8_t blue = (j + xOffset);
      uint8_t green = (i + yOffset);
      *pixel++ = 0xFF000000 | ((green << 8) | blue);
    }
    row += BackBuffer.pitch;
  }
}
void resizeTexture(SDL_Renderer *renderer, sdl_offscreen_buffer *BackBuffer,
                   int width, int height) {
  if (BackBuffer->texture) {
    SDL_DestroyTexture(BackBuffer->texture);
  }
  if (BackBuffer->texturePixels) {
    free(BackBuffer->texturePixels);
  }

  BackBuffer->texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, width, height);
  BackBuffer->width = width;
  BackBuffer->height = height;
  BackBuffer->pitch = width * 4;
  BackBuffer->texturePixels = malloc(BackBuffer->height * BackBuffer->pitch);
}

void handleEvent(bool &running, const SDL_Event &event) {
  SDL_Window *window = SDL_GetWindowFromEvent(&event);
  SDL_Renderer *renderer = SDL_GetRenderer(window);
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
  } break;
  case SDL_EVENT_WINDOW_EXPOSED: {
  } break;
  default: {
  } break;
  }
}
int main(int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
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

  SDL_Gamepad *controllerHandles[4] = {0};
  SDL_JoystickID *joystick_ids = SDL_GetJoysticks();
  for (SDL_JoystickInstanceID current = joystick_ids[0]; current != 0;) {
  }

  SDL_Free(joystick_ids);
  sdl_offscreen_buffer BackBuffer = {0};
  bool running = true;
  int winWidth;
  int winHeight;

  SDL_GetWindowSizeInPixels(window, &winWidth, &winHeight);
  resizeTexture(renderer, &BackBuffer, winWidth, winHeight);
  SDLUpdateWindow(window, BackBuffer);
  int xOffset = 0;
  int yOffset = 0;
  while (running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      handleEvent(running, event);
    }
    int width, height;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    renderWierdGradientPattern(BackBuffer, xOffset, yOffset);
    SDLUpdateWindow(window, BackBuffer);
    ++xOffset;
    ++yOffset;
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
