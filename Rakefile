require 'rubygems'
gem 'rspec', '>= 2'
require 'rspec'
require 'rspec/core/rake_task'

namespace :gem do

  desc "Builds the gem"
  task :build do
    system "gem build *.gemspec && mkdir -p pkg/ && mv *.gem pkg/"
  end

  desc "Builds and installs the gem"
  task :install => :build do
    system "gem install pkg/"
  end

end

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

end

desc "Run all specs in spec/lib directory"
RSpec::Core::RakeTask.new(:spec) do |task|
  task.pattern = "spec/lib/**/*_spec.rb"
end

namespace :spec do

  desc "Run all specs in spec/acceptance directory"
  RSpec::Core::RakeTask.new(:acceptance => "ext:build") do |task|
    task.pattern = "spec/acceptance/**/*_spec.rb"
  end

end
