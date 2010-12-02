#include "test_acapela.h"

void Init_acapela_test() {
  VALUE mTest = rb_define_module("Test");
  VALUE mTTS = rb_define_module_under(mTest, "TTS");
  VALUE mAcapela = rb_define_module_under(mTTS, "Acapela");

  rb_define_singleton_method(mAcapela, "run", acapela_test_run, 0);
}

VALUE acapela_test_run(VALUE self) {
  CU_initialize_registery();


  CU_cleanup_registry();
}
