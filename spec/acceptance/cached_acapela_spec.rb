require File.expand_path(File.join(File.dirname(__FILE__), "..", "spec_helper"))

describe TTS::Cache do

  before :each do
    @acapela = TTS::Acapela.new :host => "46.51.190.56", :voice => "julia22k", :sample_frequency => 22050
    @acapela.connect

    @cache = described_class.new @acapela, "/tmp/cache"
  end

  after :each do
    @cache.clear
    @acapela.disconnect
  end

  describe "synthesize" do

    before :each do
      @cache_filename = "/tmp/cache/58bb7452ad8aaa2a25509ec076cc979d.pcm"
    end

    context "for a new request" do

      before :each do
        @cache.clear
      end

      it "should create a new cache file" do
        File.exists?(@cache_filename).should be_false
        @cache.synthesize "Hallo"
        File.exists?(@cache_filename).should be_true
      end

      it "should return the right file" do
        file = @cache.synthesize "Hallo"
        file.path.should == @cache_filename
      end

    end

    context "for a cached request" do

      before :each do
        @cache.synthesize "Hallo"
      end

      it "should not create a new cache file" do
        File.exists?(@cache_filename).should be_true
        @cache.synthesize "Hallo"
        File.exists?(@cache_filename).should be_true
      end

      it "should return the right file" do
        file = @cache.synthesize "Hallo"
        file.path.should == @cache_filename
      end

    end

  end

end
