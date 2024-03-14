#ifndef __IME_DIALOG_H__
#define __IME_DIALOG_H__

#define IME_DIALOG_RESULT_NONE 0
#define IME_DIALOG_RESULT_RUNNING 1
#define IME_DIALOG_RESULT_FINISHED 2
#define IME_DIALOG_RESULT_CANCELED 3
#define IME_DIALOG_ALREADY_RUNNING -1
#define IME_DIALOG_UNKNOW_STATUS -2

typedef void (*ime_callback_t)(int ime_result);

namespace Dialog
{
  int initImeDialog(const char *Title, const char *initialTextBuffer, int max_text_length, uint32_t type, float posx, float posy);
  uint8_t *getImeDialogInputText();
  uint16_t *getImeDialogInputText16();
  const char *getImeDialogInitialText();
  int isImeDialogRunning();
  int updateImeDialog();
}

#endif
