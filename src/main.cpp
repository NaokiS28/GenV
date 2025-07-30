/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * main.cpp - Created on 21-04-2025
 *
 * GenV is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * GenV is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GenV. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>

#include "common/services/services.hpp"
#include "common/util/log.hpp"

int main(int argc, char *argv[])
{
   LOG_APP("Starting GenV...");

   if (!Services::startup())
   {
      LOG_APP("GenV failed to init.");
#ifndef GENV_WIN32
      while (1)
      {
      }
#endif
      return -1;
   }

   System::ISystem *system = Services::getSystem();
   Video::IVideo *video = Services::getVideo();            // It is assumed the System class will have init'd the I/O driver.
   Apps::AppManager *apps = Services::getAppMgrInstance(); // AppManager is a singleton and is not changed accross platforms.

   uint8_t sm_state = System::SM_NORMAL;
   while (sm_state != System::SM_QUIT)
   {
      sm_state = system->update();

      if (sm_state == System::SM_RESIZE)
      {
         // For windowed/computer platforms and the window has changed size.
         apps->reload();
      }
      apps->update();

      if (video->beginRender())
      {
         //apps->render();
         video->endRender();
      }
   }

   Services::shutdown(); // Shuts down the driver services and deletes them
   return 0;
}