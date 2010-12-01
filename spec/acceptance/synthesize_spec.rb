require File.expand_path(File.join(File.dirname(__FILE__), "..", "spec_helper"))

describe TTS::Acapela do

  before :each do
    @acapela = described_class.new "46.51.190.56", 6666
  end

  describe "synthesize" do

    it "should return a file with the synthesized text" do
      file = @acapela.synthesize "Hallo"
      file.should be_instance_of(File)
    end

  end

end