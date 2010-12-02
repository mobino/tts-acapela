require 'mkmf'

extension_name = "acapela_test"

dir_config extension_name

have_library "cunit", "CU_initialize_registery"
have_library "nscube", "nscCreateServerContext"

create_makefile extension_name
