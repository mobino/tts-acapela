require File.join(File.dirname(__FILE__), "ext", "acapela")

include TTS

puts synthesize("test")