require "pp"

data = Hash.new{ |h,k| h[k]=Hash.new{ |h,k| h[k]=0 } }
test_names = {}

Dir.glob( "../eval/*.log" ).each do |fn|
  s = File.open(fn, &:read)
  ix = s.index("========= Summary results of MidCrush =========")
  sum = s[ix...]
  /Generator\:\s*(?<name>.*)/=~sum
  name.strip!
  /\-{46}(?<list_text>[\s\S]+)-{46}/=~sum
  list_text.split(/[\r\n]+/).each do |line|
    next unless /\d+\s+(?<test_name>.*)\s{4,}/=~line
    test_name.strip!
    data[name][test_name]+=1
    test_names[test_name]=true
  end
end

rngs = data.keys

puts( "|テスト名|#{rngs.join("|")}|")
puts( "|:--|#{rngs.map{ ":--:" }.join("|")}|")
test_names.keys.sort.each do |tn|
  r = rngs.map{ |rng| data[rng][tn] }
  puts( "|#{tn}|#{r.join("|")}|" )
end

test_name_list = test_names.keys.sort


puts( "|RNG|#{test_name_list.join("|")}|")
puts( "|:--|#{test_name_list.map{ ":--:" }.join("|")}|")
rngs.each do |rng|
  r = test_name_list.map{ |tn| data[rng][tn] }
  puts( "|#{rng}|#{r.join("|")}|" )
end
