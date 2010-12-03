
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
    attr_accessor :cache_directory

    def initialize(options = { })
      self.host             = options[:host]             || "127.0.0.1"
      self.command_port     = options[:command_port]     || 6666
      self.data_port        = options[:data_port]        || 6665
      self.voice            = options[:voice]
      self.sample_frequency = options[:sample_frequency] || 22050
      self.cache_directory  = options[:cache_directory]  || "/tmp"
    end

    def sample_frequency=(value)
      raise ValueError, "sample frequency #{value} is not supported. try 22050, 16000, 11025 or 8000." unless [ 22050, 16000, 11025, 8000 ].include?(value)
      @sample_frequency = value
    end

    def settings_stamp
      "#{@voice}#{@sample_frequency}"
    end

  end

end

require File.expand_path(File.join(File.dirname(__FILE__), "..", "..", "ext", "acapela"))
