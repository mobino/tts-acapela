#include "ruby.h"

VALUE TTS = Qnil;

void Init_acapela();

VALUE method_synthesize(VALUE self, VALUE text);
