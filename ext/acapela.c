#include "acapela.h"

void Init_acapela() {
    TTS = rb_define_module("TTS");
    rb_define_method(TTS, "synthesize", method_synthesize, 1);
}

VALUE method_synthesize(VALUE self, VALUE text) {
    int x = 10;
    return INT2NUM(x);
}
