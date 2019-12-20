// Copyright 2019 The OpenSDS Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#include "pluginInt.h"
#include "securec.h"

/* List of all plugins in this library */
static VixDiskLibNasPlugin *gPlugins[] = { &ExampleNasPlugin, NULL };

/* Usage counter for init/done functions */
static uint32 gUseCount = 0;

/* External symbol to be exported by shared lib. Contains plugin entrypoints. */
VixDiskLibPlugin **VixDiskLibPlugin_EntryPoint = (VixDiskLibPlugin **)gPlugins;

static VixDiskLibGenericLogFunc *pluginLog;
static VixDiskLibGenericLogFunc *pluginWarning;
static VixDiskLibGenericLogFunc *pluginPanic;

VixError
ExampleNas_PluginInit(VixDiskLibGenericLogFunc *log,
           VixDiskLibGenericLogFunc *warn,
           VixDiskLibGenericLogFunc *panic)
{
    if (gUseCount > 0)
    {
        /* Already initialized... */
        gUseCount += 1;
        return VIX_OK;
    }

    if (log == NULL || warn == NULL || panic == NULL)
    {
        return VIX_E_INVALID_ARG;
    }

   gUseCount += 1;

   pluginLog = log;
   pluginWarning = warn;
   pluginPanic = panic;

   memset_s(gZeroBuf, sizeof(gZeroBuf), 0, sizeof(gZeroBuf));

   return VIX_OK;
}

void
ExampleNas_PluginDone(void)
{
   gUseCount -= 1;

   if (gUseCount == 0)
   {
      // verify that all sessions are closed
      (void)pthread_mutex_lock(&gSessionListLock);

      if (gSessionHead) {
         exNasPluginSession *sess;
         for(sess = gSessionHead; sess; sess = sess->next) {
            Warning(LGPFX" Session %p is active. \n", sess);
         }

         Panic(LGPFX" Some sessions are still active.");
      }

      (void)pthread_mutex_unlock(&gSessionListLock);
   }
}

void
Log(const char *fmt, ...)
{
   va_list args;

   va_start(args, fmt);

   pluginLog(fmt, args);

   va_end(args);
}

void
Warning(const char *fmt, ...)
{
   va_list args;

   va_start(args, fmt);

   pluginWarning(fmt, args);

   va_end(args);
}

void
Panic(const char *fmt, ...)
{
   va_list args;

   va_start(args, fmt);

   pluginPanic(fmt, args);

   va_end(args);
}

