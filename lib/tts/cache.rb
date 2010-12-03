require 'digest/md5'
require 'fileutils'

module TTS

  class Cache

    attr_accessor :tts
    attr_accessor :cache_directory

    def initialize(tts, cache_directory)
      @tts, @cache_directory = tts, cache_directory
      FileUtils.mkdir_p @cache_directory
    end

    def clear
      FileUtils.rm Dir[File.join(@cache_directory, "*.pcm")]
    end

    def synthesize(text)
      cache_filename = cache_path(text)
      unless File.exists?(cache_filename)
        file = @tts.synthesize text
        FileUtils.mv file.path, cache_filename
      end
      File.new cache_filename
    end

    private

    def cache_path(text)
      File.join @cache_directory, cache_filename(text)
    end

    def cache_filename(text)
      Digest::MD5.hexdigest("#{text}#{@tts.settings_stamp}") + ".pcm"
    end

  end

end
