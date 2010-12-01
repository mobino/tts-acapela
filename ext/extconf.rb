require 'mkmf'

extension_name = "acapela"

dir_config extension_name

have_library "nscube", "nscCreateServerContext"

create_makefile extension_name
