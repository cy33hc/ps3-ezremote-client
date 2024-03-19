#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/memory.h>
#include <sysutil/osk.h>
#include <sysutil/sysutil.h>
#include <tiny3d.h>

#include "ime_dialog.h"

static int ime_dialog_running = 0;
static uint16_t inputTextBuffer[1024 + 1];
static uint8_t storebuffer[1024];
static char initial_ime_text[1024];

static oskInputFieldInfo inputFieldInfo;
static oskParam parameters;
static oskCallbackReturnParam outputParam;
static sys_mem_container_t containerid;

static void utf16_to_utf8(const uint16_t *src, uint8_t *dst)
{
  int i;
  for (i = 0; src[i]; i++)
  {
    if ((src[i] & 0xFF80) == 0)
    {
      *(dst++) = src[i] & 0xFF;
    }
    else if ((src[i] & 0xF800) == 0)
    {
      *(dst++) = ((src[i] >> 6) & 0xFF) | 0xC0;
      *(dst++) = (src[i] & 0x3F) | 0x80;
    }
    else if ((src[i] & 0xFC00) == 0xD800 && (src[i + 1] & 0xFC00) == 0xDC00)
    {
      *(dst++) = (((src[i] + 64) >> 8) & 0x3) | 0xF0;
      *(dst++) = (((src[i] >> 2) + 16) & 0x3F) | 0x80;
      *(dst++) = ((src[i] >> 4) & 0x30) | 0x80 | ((src[i + 1] << 2) & 0xF);
      *(dst++) = (src[i + 1] & 0x3F) | 0x80;
      i += 1;
    }
    else
    {
      *(dst++) = ((src[i] >> 12) & 0xF) | 0xE0;
      *(dst++) = ((src[i] >> 6) & 0x3F) | 0x80;
      *(dst++) = (src[i] & 0x3F) | 0x80;
    }
  }

  *dst = '\0';
}

static void utf8_to_utf16(const uint8_t *src, uint16_t *dst)
{
  int i;
  for (i = 0; src[i];)
  {
    if ((src[i] & 0xE0) == 0xE0)
    {
      *(dst++) = ((src[i] & 0x0F) << 12) | ((src[i + 1] & 0x3F) << 6) | (src[i + 2] & 0x3F);
      i += 3;
    }
    else if ((src[i] & 0xC0) == 0xC0)
    {
      *(dst++) = ((src[i] & 0x1F) << 6) | (src[i + 1] & 0x3F);
      i += 2;
    }
    else
    {
      *(dst++) = src[i];
      i += 1;
    }
  }

  *dst = '\0';
}

namespace Dialog
{
  static void sysutil_exit_callback(u64 status, u64 param, void *usrdata)
  {
    switch (status)
    {
    case SYSUTIL_EXIT_GAME:
      break;
    case SYSUTIL_DRAW_BEGIN:
    case SYSUTIL_DRAW_END:
      break;
    case SYSUTIL_OSK_LOADED:
      break;
    case SYSUTIL_OSK_INPUT_CANCELED:
      oskAbort();
    case SYSUTIL_OSK_DONE:
      if (status == SYSUTIL_OSK_DONE)
        oskUnloadAsync(&outputParam);
      break;
    case SYSUTIL_OSK_UNLOADED:
      ime_dialog_running = 0;
      break;
    default:
      break;
    }
  }

  int initImeDialog(const char *Title, const char *initialTextBuffer, int max_text_length, u32 type, float posx, float posy)
  {
    if (ime_dialog_running)
      return IME_DIALOG_ALREADY_RUNNING;

    uint16_t title[100];

    if ((initialTextBuffer && strlen(initialTextBuffer) > 1023) || (Title && strlen(Title) > 99))
    {
      ime_dialog_running = 0;
      return -1;
    }

    memset(&inputTextBuffer[0], 0, sizeof(inputTextBuffer));
    memset(&storebuffer[0], 0, sizeof(storebuffer));
    memset(&initial_ime_text[0], 0, sizeof(initial_ime_text));

    if (initialTextBuffer)
    {
      snprintf(initial_ime_text, 1023, "%s", initialTextBuffer);
    }

    // converts the multibyte string src to a wide-character string starting at dest.
    utf8_to_utf16((uint8_t *)initialTextBuffer, inputTextBuffer);
    utf8_to_utf16((uint8_t *)Title, title);

    memset(&inputFieldInfo, 0, sizeof(oskInputFieldInfo));

    inputFieldInfo.maxLength = max_text_length;
    inputFieldInfo.startText = inputTextBuffer;
    inputFieldInfo.message = title;

    memset(&parameters, 0, sizeof(oskParam));
    parameters.allowedPanels = type;
    parameters.firstViewPanel = type;
    parameters.controlPoint = (oskPoint){posx, posy};
    parameters.prohibitFlags = OSK_PROHIBIT_RETURN;

    memset(&outputParam, 0, sizeof(oskCallbackReturnParam));
    outputParam.res = OSK_OK;
    outputParam.len = max_text_length;
    outputParam.str = inputTextBuffer;

    sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT0, sysutil_exit_callback, NULL);

    s32 res = 0;
    res = sysMemContainerCreate(&containerid, 4 * 1024 * 1024);
    if (res != 0)
    {
      return -1;
    }

    oskSetInitialInputDevice(OSK_DEVICE_PAD);
    oskSetKeyLayoutOption(OSK_FULLKEY_PANEL);
    oskSetLayoutMode(OSK_LAYOUTMODE_HORIZONTAL_ALIGN_CENTER | OSK_LAYOUTMODE_VERTICAL_ALIGN_CENTER);

    res = oskLoadAsync(containerid, &parameters, &inputFieldInfo);
    if (res == 0)
    {
      return ime_dialog_running = 1;
    }

    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0);
    sysMemContainerDestroy(containerid);

    return res;
  }

  int isImeDialogRunning()
  {
    return ime_dialog_running;
  }

  uint8_t *getImeDialogInputText()
  {
    return storebuffer;
  }

  uint16_t *getImeDialogInputText16()
  {
    return inputTextBuffer;
  }

  const char *getImeDialogInitialText()
  {
    return initial_ime_text;
  }

  int updateImeDialog()
  {
    if (!ime_dialog_running)
      return IME_DIALOG_RESULT_NONE;

    while (ime_dialog_running)
    {
      tiny3d_Clear(0xff000000, TINY3D_CLEAR_ALL);
      tiny3d_Flip();
    }

    int status;
    switch (outputParam.res)
    {
    case OSK_OK:
      utf16_to_utf8(inputTextBuffer, storebuffer);
      status = IME_DIALOG_RESULT_FINISHED;
      break;
    case OSK_CANCELED:
      status = IME_DIALOG_RESULT_CANCELED;
      break;
    case OSK_ABORT:
      status = IME_DIALOG_RESULT_CANCELED;
      break;
    case OSK_NO_TEXT:
      status = IME_DIALOG_RESULT_NONE;
      break;
    default:
      status = IME_DIALOG_RESULT_CANCELED;
    }

    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0);
    sysMemContainerDestroy(containerid);
    return status;
  }
} // namespace Dialog