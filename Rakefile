require 'rubygems'
gem 'rspec', '>= 2'
require 'rspec'
require 'rspec/core/rake_task'

namespace :ext do

  task :create_makefile do
    chdir("ext") { ruby "extconf.rb" }
  end

  task :compile => :create_makefile do
    chdir("ext") { sh "make" }
  end

  task :move_objects do
    chdir("ext") do
      mkdir_p "objs"
      Dir["*.o"].each do |filename|
        mv filename, "objs"
      end
    end
  end

  task :build => [ :compile, :move_objects ] do

  end

  namespace :tests do

    task :create_makefile do
      chdir("ext/tests") { ruby "extconf.rb" }
    end

    task :compile => :create_makefile do
      chdir("ext/tests") { sh "make" }
    end

    task :move_objects do
      chdir("ext/tests") do
        mkdir_p "objs"
        Dir["*.o"].each do |filename|
          mv filename, "objs"
        end
      end
    end

    task :run => [ :compile, :move_objects ] do
      require File.join(File.dirname(__FILE__), "ext", "tests", "acapela_test")
      Test::TTS::Acapela.run();
    end

  end

end

namespace :spec do

  desc "Run all integration specs in spec/acceptance directory"
  RSpec::Core::RakeTask.new(:acceptance => "ext:build") do |task|
    task.pattern = "spec/acceptance/**/*_spec.rb"
  end

end
