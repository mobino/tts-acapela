#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ruby.h"
#include "nscube.h"

void Init_acapela();

VALUE acapela_initialize(VALUE self, VALUE host, VALUE port);

VALUE acapela_synthesize(VALUE self, VALUE text);
