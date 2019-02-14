//
//
//  Author : Jin Heo
//  Date : Oct, 3, 2017
//

#include "headers.h"
#include "wind.h"
#include "fileReader.h"
#include "demo.h"

void eventLoop()
{
  bool running = true;
  while(running) {
    SDL_Event event;
    if(SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          running = false;
          break;

        case SDL_KEYDOWN:
          keyboardPressEvent(&event);
          break;

        case SDL_WINDOWEVENT:
          if(event.window.event == SDL_WINDOWEVENT_EXPOSED) {
            repaint();
          }
          break;
      }
    }
    else {
      repaint();
    }
  }
}


// For dealing with keyborad press
void keyboardPressEvent(SDL_Event *event)
{
  SDL_KeyboardEvent *keyEvent = (SDL_KeyboardEvent *)event;
  switch(keyEvent->keysym.sym) {
    case SDLK_ESCAPE:
    case SDLK_RETURN:
    case SDLK_q: {
      SDL_Event quitEvent;
      quitEvent.type = SDL_QUIT;
      SDL_PushEvent(&quitEvent);
      break;
    }
    case SDLK_SPACE:
      if(graphOn==0) graphOn=1;
      else graphOn=0;
      break;

    // Threshold adjustments.
    case SDLK_EQUALS:
      break;

    case SDLK_KP_MINUS:
    case SDLK_MINUS:
      break;

    case SDLK_KP_PLUS:
    case SDLK_PLUS:
      break;
  }
}

// Repaint for the result
void repaint()
{
  if(graphOn) {
    graphExe();
  }

  if(useDisplay) {
    if(!graphOn) {
      // Reset window title.
      SDL_SetWindowTitle(sdlWindow, "No Execution");
    }

    // Blit the image surface to the window surface.
    SDL_BlitScaled(graphOn ? sdlOutputSurface : sdlInputSurface, NULL,
        sdlWindowSurface, NULL);

    // Update the window surface.
    SDL_UpdateWindowSurface(sdlWindow);
  }
}

