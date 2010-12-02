#include "acapela.h"

// private functions

VALUE acapelaErrorClass;

void checkConnection(const char* methodName, VALUE self) {
  if (acapela_connected(self) == Qfalse) {
    rb_raise(acapelaErrorClass, "method '%s' can just be executed with a open connection", methodName);
  }
}

void checkResult(const char* action, nscRESULT result) {
  if (result != NSC_OK) {
    rb_raise(acapelaErrorClass, "error %d while performing action '%s'\n", result, action);
  }
}

int callback_speech_data(
  const unsigned char *data,
  unsigned int dataSize,
  PNSC_SOUND_DATA soundData,
  void *instanceData
) {
    fwrite(data, dataSize, 1, (FILE*)instanceData);
    return 0;
}

void initializeExecutionData(NSC_EXEC_DATA* executionData, FILE *file) {
  executionData->pfnSpeechData = callback_speech_data;
  executionData->pfnSpeechEvent = NULL;
  executionData->ulEventFilter = NSC_EVTBIT_TEXT;
  executionData->bEventSynchroReq = 1;
  executionData->vsSoundData.uiSize = 0;
  executionData->vsSoundData.pSoundBuffer = NULL;
  executionData->pAppInstanceData = (void*)file;
}

// public functions

void Init_acapela() {
  VALUE ttsModule = rb_define_module("TTS");
  VALUE acapelaClass = rb_define_class_under(ttsModule, "Acapela", rb_cObject);
  VALUE standardErrorClass = rb_const_get(rb_cObject, rb_intern("StandardError"));
  acapelaErrorClass = rb_define_class_under(acapelaClass, "Error", standardErrorClass);

  rb_define_method(acapelaClass, "initialize", acapela_initialize, 3);
  rb_define_method(acapelaClass, "connect", acapela_connect, 0);
  rb_define_method(acapelaClass, "connected?", acapela_connected, 0);
  rb_define_method(acapelaClass, "disconnect", acapela_disconnect, 0);
  rb_define_method(acapelaClass, "voices", acapela_voices, 0);
  rb_define_method(acapelaClass, "synthesize", acapela_synthesize, 2);
}

VALUE acapela_initialize(VALUE self, VALUE host, VALUE commandPort, VALUE dataPort) {
  rb_ivar_set(self, rb_intern("@host"), host);
  rb_ivar_set(self, rb_intern("@command_port"), commandPort);
  rb_ivar_set(self, rb_intern("@data_port"), dataPort);
}

VALUE acapela_connect(VALUE self) {
  nscRESULT response;
  nscHSRV *serverHandle;
  nscHANDLE *dispatcherHandle;

  if (acapela_connected(self) == Qtrue)
    return;

  VALUE host = rb_ivar_get(self, rb_intern("@host"));
  VALUE commandPort = rb_ivar_get(self, rb_intern("@command_port"));
  VALUE dataPort = rb_ivar_get(self, rb_intern("@data_port"));

  serverHandle = (nscHSRV*)malloc(sizeof(nscHSRV));
  response = nscCreateServerContextEx(NSC_AF_INET, NUM2INT(commandPort), NUM2INT(dataPort), StringValuePtr(host), serverHandle);
  rb_ivar_set(self, rb_intern("@connection"), response == NSC_OK ? (VALUE)serverHandle : Qnil);

  if (response != NSC_OK)
    return;

  dispatcherHandle = (nscHANDLE*)malloc(sizeof(nscHANDLE));
  response = nscCreateDispatcher(dispatcherHandle);
  rb_ivar_set(self, rb_intern("@dispatcher"), response == NSC_OK ? (VALUE)dispatcherHandle : Qnil);
}

VALUE acapela_connected(VALUE self) {
  return rb_ivar_get(self, rb_intern("@connection")) == Qnil ? Qfalse : Qtrue;
}

VALUE acapela_disconnect(VALUE self) {
  nscRESULT response;
  nscHSRV *serverHandle;
  nscHANDLE *dispatcherHandle;

  if (acapela_connected(self) == Qfalse)
    return;

  dispatcherHandle = (nscHANDLE*)rb_ivar_get(self, rb_intern("@dispatcher"));
  response = nscDeleteDispatcher(*dispatcherHandle);
  if (response == NSC_OK) {
    rb_ivar_set(self, rb_intern("@dispatcher"), Qnil);
    free(dispatcherHandle);
  }

  serverHandle = (nscHSRV*)rb_ivar_get(self, rb_intern("@connection"));
  response = nscReleaseServerContext(*serverHandle);
  if (response == NSC_OK) {
    rb_ivar_set(self, rb_intern("@connection"), Qnil);
    free(serverHandle);
  }
}

VALUE acapela_voices(VALUE self) {
  VALUE result;

  nscRESULT response;
  nscHSRV *serverHandle;

  nscHANDLE voiceHandle;
  NSC_FINDVOICE_DATA voice;

  checkConnection("voices", self);

  serverHandle = (nscHSRV*)rb_ivar_get(self, rb_intern("@connection"));

  result = rb_ary_new();
  response = nscFindFirstVoice(*serverHandle, NULL, 0, 0, 0, &voice, &voiceHandle);
  while (response == NSC_OK) {
    rb_ary_push(result, rb_str_new2(voice.cVoiceName));
    response = nscFindNextVoice(voiceHandle, &voice);
  }
  nscCloseFindVoice(voiceHandle);

  return result;
}

VALUE acapela_synthesize(VALUE self, VALUE voice, VALUE text) {
  VALUE result;

  nscRESULT response;
  nscHSRV *serverHandle;
  nscHANDLE *dispatcherHandle;
  nscHANDLE ttsHandle;
  NSC_EXEC_DATA executionData;
  char* filename;
  FILE *file;

  char *voiceName;
  nscCHANID channelId;

  checkConnection("synthesize", self);

  serverHandle = (nscHSRV*)rb_ivar_get(self, rb_intern("@connection"));
  dispatcherHandle = (nscHANDLE*)rb_ivar_get(self, rb_intern("@dispatcher"));

  voiceName = StringValuePtr(voice);

  checkResult("opening channel", nscInitChannel(*serverHandle, voiceName, 0, 0, *dispatcherHandle, &channelId));
  checkResult("lock channel", nscLockChannel(*serverHandle, channelId, *dispatcherHandle, &ttsHandle));
  checkResult("add text", nscAddText(ttsHandle, StringValuePtr(text), NULL));

  filename = tmpnam(NULL);
  file = fopen(filename, "w");
  initializeExecutionData(&executionData, file);
  checkResult("execute channel", nscExecChannel(ttsHandle, &executionData));
  fclose(file);

  checkResult("unlock channel", nscUnlockChannel(ttsHandle));
  checkResult("closing channel", nscCloseChannel(*serverHandle, channelId));

  VALUE fileClass = rb_const_get(rb_cObject, rb_intern("File"));
  result = rb_funcall(fileClass, rb_intern("new"), 1, rb_str_new2(filename));

  return result;
}
