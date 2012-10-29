# encoding: utf-8

Gem::Specification.new do |specification|
  specification.name                  = "tts-acapela"
  specification.version               = "0.1.1"
  specification.date                  = "2010-12-03"

  specification.authors               = [ "Philipp Brüll" ]
  specification.email                 = "b.phifty@gmail.com"
  specification.rubyforge_project     = "tts-acapela"

  specification.summary               = "A client for the Acapela TTS server."
  specification.description           = "It uses the native nscapi library to connect to the server."

  specification.has_rdoc              = true
  specification.files                 = [ "README.rdoc", "Rakefile" ] + Dir["ext/**/*.{c,h,rb}"] + Dir["lib/**/*"] + Dir["spec/**/*"]
  specification.extra_rdoc_files      = [ "README.rdoc" ]
  specification.require_path          = "lib"
  specification.required_ruby_version = ">= 1.8.7"

  specification.extensions << "ext/extconf.rb"

  specification.test_files            = Dir["spec/**/*_spec.rb"]

  specification.add_development_dependency "rspec", ">= 2"
end
