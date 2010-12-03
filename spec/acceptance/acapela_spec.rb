require File.expand_path(File.join(File.dirname(__FILE__), "..", "spec_helper"))
require 'digest/md5'

describe TTS::Acapela do

  before :each do
    @acapela = described_class.new :host => "46.51.190.56"
  end

  shared_examples_for "a method that need an open connection" do

    it "should raise an #{TTS::Acapela::NotConnectedError} if disconnected" do
      @acapela.disconnect
      lambda do
        do_call
      end.should raise_error(TTS::Acapela::NotConnectedError);
    end

  end

  describe "connect" do

    after :each do
      @acapela.disconnect
    end

    it "should change the state to connected" do
      lambda do
        @acapela.connect
      end.should change(@acapela, :connected?).from(false).to(true)
    end

    it "should not raise an error if it's already connected" do
      @acapela.connect
      lambda do
        @acapela.connect
      end.should_not raise_error
    end

  end

  describe "disconnect" do

    before :each do
      @acapela.connect
    end

    it "should change the state to disconnected" do
      lambda do
        @acapela.disconnect
      end.should change(@acapela, :connected?).from(true).to(false)
    end

    it "should not raise an error if it's already disconnected" do
      @acapela.disconnect
      lambda do
        @acapela.disconnect
      end.should_not raise_error
    end

  end

  describe "voices" do

    before :each do
      @acapela.connect
    end

    after :each do
      @acapela.disconnect
    end

    def do_call
      @acapela.voices
    end

    it_should_behave_like "a method that need an open connection"

    it "should return an array of voices" do
      voices = do_call
      voices.should be_instance_of(Array)
      voices.should == [
        "julia22k", "klaus22k", "lucy22k", "bruno22k", "claire22k", "julie22k",
        "peter22k", "sarah22k", "rachel22k", "justine22k", "graham22k", "alice22k"
      ]
    end

  end

  describe "sample_frequency=" do

    it "should set the sample frequency" do
      @acapela.sample_frequency = 22050
      @acapela.sample_frequency == 22050
    end

    it "should raise an #{TTS::Acapela::ValueError} on an invalid value" do
      lambda do
        @acapela.sample_frequency = 5
      end.should raise_error(TTS::Acapela::ValueError)

      lambda do
        @acapela.sample_frequency = "invalid"
      end.should raise_error(TTS::Acapela::ValueError)
    end

  end

  describe "synthesize" do

    before :each do
      @acapela.connect
      @acapela.voice = "sarah22k"
    end

    after :each do
      @acapela.disconnect
    end

    def do_call(text = "Hallo Welt")
      @acapela.synthesize text
    end

    it_should_behave_like "a method that need an open connection"

    it "should return a file with the synthesized text" do
      file = do_call
      file.should be_instance_of(File)
    end

    context "with voice 'julia22k' and sample frequency '22050'" do

      before :each do
        @acapela.voice = "julia22k"
        @acapela.sample_frequency = 22050
      end

      it "should return the correct file" do
        file = do_call
        md5 = Digest::MD5.hexdigest File.read(file.path)
        md5.should == "7e50b6c1666ec97be6c84b9dacf0ef89"
      end

    end

    context "with voice 'julia22k' and sample frequency '11025'" do

      before :each do
        @acapela.voice = "julia22k"
        @acapela.sample_frequency = 11025
      end

      it "should return the correct file" do
        file = do_call "test ok"
        md5 = Digest::MD5.hexdigest File.read(file.path)
        md5.should == "257df7f7972f794f4759ac97042faf97"
      end

    end

  end

end