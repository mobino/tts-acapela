#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ruby.h"
#include "nscube.h"

void Init_acapela();

VALUE acapela_connect(VALUE self);
VALUE acapela_connected(VALUE self);
VALUE acapela_disconnect(VALUE self);
VALUE acapela_voices(VALUE self);
VALUE acapela_synthesize(VALUE self, VALUE text);
