
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
