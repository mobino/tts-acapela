#include "tts.h"

void Init_tts() {
    TTS = rb_define_module("TTS");
    rb_define_method(TTS, "test1", method_test1, 0);
}

VALUE method_test1(VALUE self) {
    int x = 10;
    return INT2NUM(x);
}
