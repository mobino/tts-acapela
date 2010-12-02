require File.expand_path(File.join(File.dirname(__FILE__), "..", "spec_helper"))

describe TTS::Acapela do

  before :each do
    @acapela = described_class.new "46.51.190.56", 6666, 6665
  end

  shared_examples_for "a method that need an open connection" do

    it "should raise an #{TTS::Acapela::Error} if disconnected" do
      @acapela.disconnect
      lambda do
        do_call
      end.should raise_error(TTS::Acapela::Error);
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

  describe "voice=" do

    it "should set the voice" do
      @acapela.voice = "sarah22k"
      @acapela.voice.should == "sarah22k"
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

    def do_call
      @acapela.synthesize "Hallo Welt"
    end

    it_should_behave_like "a method that need an open connection"

    it "should return a file with the synthesized text" do
      file = do_call
      file.should be_instance_of(File)
    end

  end

end
