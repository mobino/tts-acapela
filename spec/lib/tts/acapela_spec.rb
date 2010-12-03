require File.expand_path(File.join(File.dirname(__FILE__), "..", "..", "spec_helper"))

describe TTS::Acapela do

  before :each do
    @acapela = described_class.new :voice => "julia22k", :sample_frequency => 22050
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

  describe "settings_stamp" do

    it "should return a string with a snapshort of the synthesize-settings" do
      @acapela.settings_stamp.should == "julia22k22050"
    end

  end

end
