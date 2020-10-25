COMMANDS = [
  "./eval_xs128", 
  "./eval_xpp", "./eval_xss",
  "./eval_pp", "./eval_ss",
  "./eval_mt"
 ]

res={}
COMMANDS.map{ |cmd|
  Thread.new{ res[cmd]=%x(#{cmd}) }
}.each(&:join)


def summary(v)
  ix=v.index("========= Summary results of MidCrush =========")
  v[ix...]
end

res.each do |k,v|
  puts k
  File.open( k+".log", "w" ) do |f|
    f.puts(v)
  end
  puts summary(v)
end
