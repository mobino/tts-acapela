#include "acapela.h"

void Init_acapela() {
    VALUE mTTS = rb_define_module("TTS");
    VALUE cAcapela = rb_define_class_under(mTTS, "Acapela", rb_cObject);

    rb_define_method(cAcapela, "initialize", acapela_initialize, 2);
    rb_define_method(cAcapela, "synthesize", acapela_synthesize, 1);
}

VALUE acapela_initialize(VALUE self, VALUE host, VALUE port) {
    rb_ivar_set(self, rb_intern("@host"), host);
    rb_ivar_set(self, rb_intern("@port"), port);
}

VALUE acapela_synthesize(VALUE self, VALUE text) {
    VALUE vHost = rb_ivar_get(self, rb_intern("@host"));
    VALUE vPort = rb_ivar_get(self, rb_intern("@port"));

    const char* host = StringValuePtr(vHost);
    int port = NUM2INT(vPort);

    nscHSRV hServer;
    nscRESULT Result;

    printf("Connecting to %s %d ... ", host, port);

    Result = nscCreateServerContext(NSC_AF_INET, port, host, &hServer);
    printf(Result == NSC_OK ? "OK\n" : "Error\n");

    return Qnil;
}
