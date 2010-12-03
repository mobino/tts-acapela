#include "acapela.h"

// private functions

VALUE notConnectedErrorClass;
VALUE responseErrorClass;
VALUE valueErrorClass;

void checkConnection(const char* methodName, VALUE self) {
  if (acapela_connected(self) == Qfalse)
    rb_raise(notConnectedErrorClass, "method '%s' can just be executed with a open connection", methodName);
}

void checkResponse(const char* action, nscRESULT response) {
  if (response != NSC_OK)
    rb_raise(responseErrorClass, "error %d while performing action '%s'\n", response, action);
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
  notConnectedErrorClass = rb_const_get(acapelaClass, rb_intern("NotConnectedError"));
  responseErrorClass = rb_const_get(acapelaClass, rb_intern("ResponseError"));
  valueErrorClass = rb_const_get(acapelaClass, rb_intern("ValueError"));

  rb_define_method(acapelaClass, "connect", acapela_connect, 0);
  rb_define_method(acapelaClass, "connected?", acapela_connected, 0);
  rb_define_method(acapelaClass, "disconnect", acapela_disconnect, 0);
  rb_define_method(acapelaClass, "voices", acapela_voices, 0);
  rb_define_method(acapelaClass, "synthesize", acapela_synthesize, 1);
}

VALUE acapela_connect(VALUE self) {
  nscHSRV *serverHandle;
  nscHANDLE *dispatcherHandle;

  if (acapela_connected(self) == Qtrue)
    return Qfalse;

  VALUE host = rb_ivar_get(self, rb_intern("@host"));
  VALUE commandPort = rb_ivar_get(self, rb_intern("@command_port"));
  VALUE dataPort = rb_ivar_get(self, rb_intern("@data_port"));

  serverHandle = (nscHSRV*)malloc(sizeof(nscHSRV));
  checkResponse("connecting", nscCreateServerContextEx(NSC_AF_INET, NUM2INT(commandPort), NUM2INT(dataPort), StringValuePtr(host), serverHandle));
  rb_ivar_set(self, rb_intern("@connection"), (VALUE)serverHandle);

  dispatcherHandle = (nscHANDLE*)malloc(sizeof(nscHANDLE));
  checkResponse("creating dispatcher", nscCreateDispatcher(dispatcherHandle));
  rb_ivar_set(self, rb_intern("@dispatcher"), (VALUE)dispatcherHandle);

  return Qtrue;
}

VALUE acapela_connected(VALUE self) {
  return rb_ivar_get(self, rb_intern("@connection")) == Qnil ? Qfalse : Qtrue;
}

VALUE acapela_disconnect(VALUE self) {
  nscHSRV *serverHandle;
  nscHANDLE *dispatcherHandle;

  if (acapela_connected(self) == Qfalse)
    return Qfalse;

  dispatcherHandle = (nscHANDLE*)rb_ivar_get(self, rb_intern("@dispatcher"));
  checkResponse("deleting dispatcher", nscDeleteDispatcher(*dispatcherHandle));
  rb_ivar_set(self, rb_intern("@dispatcher"), Qnil);
  free(dispatcherHandle);

  serverHandle = (nscHSRV*)rb_ivar_get(self, rb_intern("@connection"));
  checkResponse("disconnecting", nscReleaseServerContext(*serverHandle));
  rb_ivar_set(self, rb_intern("@connection"), Qnil);
  free(serverHandle);

  return Qtrue;
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

VALUE acapela_synthesize(VALUE self, VALUE text) {
  VALUE result;

  nscRESULT response;
  nscHSRV* serverHandle;
  nscHANDLE* dispatcherHandle;
  VALUE voice;
  int sampleFrequency;
  nscHANDLE ttsHandle;
  NSC_EXEC_DATA executionData;
  char* filename;
  FILE* file;
  nscCHANID channelId;

  checkConnection("synthesize", self);

  serverHandle = (nscHSRV*)rb_ivar_get(self, rb_intern("@connection"));
  dispatcherHandle = (nscHANDLE*)rb_ivar_get(self, rb_intern("@dispatcher"));
  voice = rb_ivar_get(self, rb_intern("@voice"));
  sampleFrequency = rb_ivar_get(self, rb_intern("@sample_frequency"));

  checkResponse("opening channel", nscInitChannel(*serverHandle, StringValuePtr(voice), NUM2INT(sampleFrequency), 0, *dispatcherHandle, &channelId));
  checkResponse("lock channel", nscLockChannel(*serverHandle, channelId, *dispatcherHandle, &ttsHandle));
  checkResponse("add text", nscAddTextEx(ttsHandle, "UTF-8", StringValuePtr(text), strlen(StringValuePtr(text)), NULL));

  filename = tmpnam(NULL);
  file = fopen(filename, "w");
  initializeExecutionData(&executionData, file);
  checkResponse("execute channel", nscExecChannel(ttsHandle, &executionData));
  fclose(file);

  checkResponse("unlock channel", nscUnlockChannel(ttsHandle));
  checkResponse("closing channel", nscCloseChannel(*serverHandle, channelId));

  VALUE fileClass = rb_const_get(rb_cObject, rb_intern("File"));
  result = rb_funcall(fileClass, rb_intern("new"), 1, rb_str_new2(filename));

  return result;
}
