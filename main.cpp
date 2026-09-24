#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gamepad.h"
#include "SDL3/SDL_haptic.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_joystick.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>

#define internal static
#define global_var static
#define local_persist static

constexpr char kWindowTitle[] = "Handmade Hero";
constexpr int kWinWidth = 1280;
constexpr int kWinHeight = 720;
constexpr int kMaxGamePads = 4;

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
  case SDL_EVENT_KEY_UP: {
    if (event.key.key == SDLK_W) {
      running = false;
    } else if (event.key.key == SDLK_A) {
    } else if (event.key.key == SDLK_S) {
    } else if (event.key.key == SDLK_D) {
    } else if (event.key.key == SDLK_SPACE) {
    } else if (event.key.key == SDLK_UP) {
    } else if (event.key.key == SDLK_DOWN) {
    } else if (event.key.key == SDLK_LEFT) {
    } else if (event.key.key == SDLK_RIGHT) {
    } else if (event.key.key == SDLK_ESCAPE) {
    }
  }
  case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
  } break;
  case SDL_EVENT_WINDOW_EXPOSED: {
  } break;
  default: {
  } break;
  }
}

typedef struct {
  SDL_Gamepad *gamepad;
} Controller;

Controller *initGamepads() {
  SDL_JoystickID *joystick_ids = SDL_GetJoysticks(nullptr);
  Controller *controllerArray =
      (Controller *)malloc(sizeof(Controller) * (kMaxGamePads + 1));
  Controller *controllers = controllerArray;
  memset(controllers, 0, (kMaxGamePads + 1) * sizeof(Controller));
  int gamepadCount = 0;
  for (SDL_JoystickID *current = joystick_ids; *current != 0; current++) {
    if (SDL_IsGamepad(*current) && gamepadCount < kMaxGamePads) {
      controllers->gamepad = SDL_OpenGamepad(*current);
      controllers++;
      gamepadCount++;
    }
  }
  SDL_free(joystick_ids);
  std::cerr << gamepadCount << " number of gamepads initiated /n" << std::endl;
  return controllerArray;
}

void destroyGamepads(Controller *controllers) {
  for (Controller *controller = controllers; controller->gamepad != NULL;
       controller++) {
    SDL_CloseGamepad(controller->gamepad);
  }
  free(controllers);
}

void playHaptic(SDL_Gamepad *gamepad) {
  if (gamepad == NULL) {
    return;
  }

  // Play effect at 50% strength for 2 seconds
  if (!SDL_RumbleGamepad(gamepad, 0x7FFF, 0x7FFF, 2000)) {
  }
}
int main(int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC)) {
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

  Controller *controllers = initGamepads();

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

    for (Controller *controller = controllers; controller->gamepad != NULL;
         ++controller) {
      if (SDL_GamepadConnected(controller->gamepad)) {
        bool up = SDL_GetGamepadButton(controller->gamepad,
                                       SDL_GAMEPAD_BUTTON_DPAD_UP);
        bool down = SDL_GetGamepadButton(controller->gamepad,
                                         SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        bool right = SDL_GetGamepadButton(controller->gamepad,
                                          SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
        bool left = SDL_GetGamepadButton(controller->gamepad,
                                         SDL_GAMEPAD_BUTTON_DPAD_LEFT);
        bool start =
            SDL_GetGamepadButton(controller->gamepad, SDL_GAMEPAD_BUTTON_START);
        bool back =
            SDL_GetGamepadButton(controller->gamepad, SDL_GAMEPAD_BUTTON_BACK);
        bool north =
            SDL_GetGamepadButton(controller->gamepad, SDL_GAMEPAD_BUTTON_NORTH);
        bool west =
            SDL_GetGamepadButton(controller->gamepad, SDL_GAMEPAD_BUTTON_WEST);
        bool east =
            SDL_GetGamepadButton(controller->gamepad, SDL_GAMEPAD_BUTTON_EAST);
        bool south =
            SDL_GetGamepadButton(controller->gamepad, SDL_GAMEPAD_BUTTON_SOUTH);

        int16_t axisX =
            SDL_GetGamepadAxis(controller->gamepad, SDL_GAMEPAD_AXIS_LEFTX);

        int16_t axisY =
            SDL_GetGamepadAxis(controller->gamepad, SDL_GAMEPAD_AXIS_LEFTY);

        if (south) {
          xOffset += 2;
        }
        if (north) {

          playHaptic(controller->gamepad);
        }
      }
    }

    int width, height;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    renderWierdGradientPattern(BackBuffer, xOffset, yOffset);
    SDLUpdateWindow(window, BackBuffer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  destroyGamepads(controllers);
  SDL_Quit();

  return 0;
}
