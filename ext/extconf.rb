require 'mkmf'

extension_name = "acapela"

dir_config extension_name, "/opt/Acapela/TelecomTTS/include"

found_nscube = find_library "nscube", "nscCreateServerContext", "/opt/Acapela/TelecomTTS/lib"

if found_nscube
  create_makefile extension_name
else
  puts "library 'nscube' is required, but wasn't found.\n"
  exit 1
end
