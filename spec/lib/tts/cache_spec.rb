require File.expand_path(File.join(File.dirname(__FILE__), "..", "..", "spec_helper"))

describe TTS::Cache do

  before :each do
    FileUtils.stub(:mkdir_p)

    @acapela = mock TTS::Acapela, :settings_stamp => "julia22k22050"

    @cache = described_class.new @acapela, "/tmp/cache"
  end

  describe "initialize" do

    it "should try to create the cache directory" do
      FileUtils.should_receive(:mkdir_p).with("/tmp/cache")
      described_class.new @acapela, "/tmp/cache"
    end

  end

  describe "clear" do

    before :each do
      FileUtils.stub(:rm)
    end

    it "should remove all *.pcm files from the cache directory" do
      FileUtils.should_receive(:rm).with(Dir["/tmp/cache/*.pcm"])
      @cache.clear
    end

  end

  describe "synthesize" do

    before :each do
      @cache_filename = "/tmp/cache/58bb7452ad8aaa2a25509ec076cc979d.pcm"

      @cached_file = mock File
      File.stub(:new).and_return(@cached_file)

      @file = mock File, :path => "/tmp/file"
      @acapela.stub(:synthesize).and_return(@file)
    end

    def do_synthesize(text = "Hallo")
      @cache.synthesize text
    end

    context "for a new request" do

      before :each do
        File.stub(:exists?).and_return(false)
        FileUtils.stub(:mv)
      end

      it "should check the existance of the right file" do
        File.should_receive(:exists?).with(@cache_filename).and_return(true)
        do_synthesize
      end

      it "should synthesize a new file" do
        @acapela.should_receive(:synthesize).with("Hallo").and_return(@file)
        file = do_synthesize
        file.should == @cached_file
      end

      it "should move the synthesized file to the cache" do
        FileUtils.should_receive(:mv).with(@file.path, @cache_filename)
        do_synthesize
      end

      it "should return the synthesized and cached file" do
        file = do_synthesize
        file.should == @cached_file
      end

    end

    context "for a cached request" do

      before :each do
        File.stub(:exists?).and_return(true)
      end

      it "should check the existance of the right file" do
        File.should_receive(:exists?).with(@cache_filename).and_return(true)
        do_synthesize
      end

      it "should not synthesize a new file" do
        @acapela.should_not_receive(:synthesize)
        do_synthesize
      end

      it "should return the cached file" do
        file = do_synthesize
        file.should == @cached_file
      end

    end

  end

end
