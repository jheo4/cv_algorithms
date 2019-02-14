//
//    Author : Jin Heo
//    Date : 2017 09 09
//

#include "headers.h"
#include "wind.h"
#include "pgmreader.h"
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
      thresh = 128;
      break;

    case SDLK_KP_MINUS:
    case SDLK_MINUS:
      thresh = std::max(thresh - 10, 16u);
      break;

    case SDLK_KP_PLUS:
    case SDLK_PLUS:
      thresh = std::min(thresh + 10, 255u);
      break;
  }
}


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

