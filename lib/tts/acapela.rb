
module TTS

  class Acapela

    class Error < StandardError; end
    class NotConnectedError < Error; end
    class ResponseError < Error; end
    class ValueError < Error; end
    class VoiceNotSupportedError < Error; end

    attr_accessor :host
    attr_accessor :command_port
    attr_accessor :data_port
    attr_accessor :voice
    attr_reader   :sample_frequency

    def initialize(options = { })
      @host, @command_port, @data_port, @voice, @sample_frequency = options.values_at :host, :command_port, :data_port, :voice, :sample_frequency
      @host ||= "127.0.0.1"
      @command_port ||= 6666
      @data_port ||= 6665
      @sample_frequency ||= 22050
    end

    def sample_frequency=(value)
      raise ValueError, "sample frequency #{value} is not supported. try 22050, 16000, 11025 or 8000." unless [ 22050, 16000, 11025, 8000 ].include?(value)
      @sample_frequency = value
    end

  end

end

require File.expand_path(File.join(File.dirname(__FILE__), "..", "..", "ext", "acapela"))
