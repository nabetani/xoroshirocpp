require "csv"
require "pp"

rels = {
"clang-mac"=>["../clang-mac.txt", 0],
"g++-mac"=>["../g++-mac.txt", 0],
"clang-rp32"=>["../rposrp.txt", 1],
"g++-rp32"=>["../rposrp.txt", 0],
"clang-rp64"=>["../ubuntu64rp.txt", 1],
"g++-rp64"=>["../ubuntu64rp.txt", 0],
"cl32-win"=>["../win.txt", 0],
"cl64-win"=>["../win.txt", 1]
}

def nth_index(s, sub, c, start=0)
  return nil unless s
  ix=s.index(sub, start)
  return ix if c<=0
  nth_index(s, sub, c-1, ix+1)
end

RNG_NAMES={
  "std::mt19937"=>"mt19937",
  "std::mt19937_64"=>"mt19937_64",
  "xoroshiro::rng128pp"=>"xoroshiro128++",
  "xoroshiro::rng128ss"=>"xoroshiro128**",
  "xoshiro::rng128pp"=>"xoshiro128++",
  "xoshiro::rng128ss"=>"xoshiro128**",
  "xorshift128::rng"=>"XorShift128",
}

data=Hash.new{ |h,k| h[k]={} }
rels.each do |name,(fn, ix)|
  s = File.open( fn, &:read )
  start = nth_index(s,"std::mt19937:", ix*2+1)
  s[start...].split(/[\r\n]+/)[0,7].each do |line|
    /(?<rng>[\w\:]+)\:\s*(?<ns>[\d\.]+)ns/ =~ line
    data[name][RNG_NAMES[rng]]=ns.to_f
  end
end

head = rels.keys.join("|")

puts <<"HEAD"
|乱数|#{head}|
|:--|--:|--:|--:|--:|--:|--:|--:|--:|
HEAD
RNG_NAMES.values.each do |rng|
  v=rels.keys.map{ |pn| "%.1fns" % data[pn][rng] }.join("|")
  puts "|#{rng}|#{v}|"
end

CSV.open( "data.csv", "w" ) do |csv|
  csv << ["rng"]+rels.keys
  RNG_NAMES.values.each do |rng|
    v=rels.keys.map{ |pn| data[pn][rng] }
    csv << [rng] + v
  end
end  
