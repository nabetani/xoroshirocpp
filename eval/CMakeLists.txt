cmake_minimum_required(VERSION 3.15)

project(eval)

set(CMAKE_C_COMPILER "/usr/local/bin/gcc-10" )
set(CMAKE_CXX_COMPILER "/usr/local/bin/g++-10" )

set(TEST_U01_DIR ../TestU01-1.2.3)

set(TEST_U01
  mid_crush.c
  ${TEST_U01_DIR}/mylib/addstr.c
  ${TEST_U01_DIR}/mylib/bitset.c
  ${TEST_U01_DIR}/mylib/chrono.c
  ${TEST_U01_DIR}/mylib/gdef.c
  ${TEST_U01_DIR}/mylib/mystr.c
  ${TEST_U01_DIR}/mylib/num.c
  ${TEST_U01_DIR}/mylib/num2.c
  ${TEST_U01_DIR}/mylib/tables.c
  # ${TEST_U01_DIR}/mylib/tcode.c
  ${TEST_U01_DIR}/mylib/util.c
  ${TEST_U01_DIR}/probdist/fbar.c
  ${TEST_U01_DIR}/probdist/fdist.c
  ${TEST_U01_DIR}/probdist/finv.c
  ${TEST_U01_DIR}/probdist/fmass.c
  ${TEST_U01_DIR}/probdist/gofs.c
  ${TEST_U01_DIR}/probdist/gofw.c
  ${TEST_U01_DIR}/probdist/statcoll.c
  ${TEST_U01_DIR}/probdist/wdist.c
  ${TEST_U01_DIR}/testu01/bbattery.c
  ${TEST_U01_DIR}/testu01/fcho.c
  ${TEST_U01_DIR}/testu01/fcong.c
  ${TEST_U01_DIR}/testu01/ffam.c
  ${TEST_U01_DIR}/testu01/ffsr.c
  # ${TEST_U01_DIR}/testu01/fftc.c
  ${TEST_U01_DIR}/testu01/fknuth.c
  ${TEST_U01_DIR}/testu01/fmarsa.c
  ${TEST_U01_DIR}/testu01/fmultin.c
  ${TEST_U01_DIR}/testu01/fnpair.c
  ${TEST_U01_DIR}/testu01/fres.c
  ${TEST_U01_DIR}/testu01/fspectral.c
  ${TEST_U01_DIR}/testu01/fstring.c
  ${TEST_U01_DIR}/testu01/ftab.c
  ${TEST_U01_DIR}/testu01/fvaria.c
  ${TEST_U01_DIR}/testu01/fwalk.c
  ${TEST_U01_DIR}/testu01/rijndael-alg-fst.c
  ${TEST_U01_DIR}/testu01/scatter.c
  ${TEST_U01_DIR}/testu01/scomp.c
  ${TEST_U01_DIR}/testu01/sentrop.c
  ${TEST_U01_DIR}/testu01/sknuth.c
  ${TEST_U01_DIR}/testu01/smarsa.c
  ${TEST_U01_DIR}/testu01/smultin.c
  ${TEST_U01_DIR}/testu01/snpair.c
  ${TEST_U01_DIR}/testu01/sres.c
  ${TEST_U01_DIR}/testu01/sspacings.c
  ${TEST_U01_DIR}/testu01/sspectral.c
  ${TEST_U01_DIR}/testu01/sstring.c
  ${TEST_U01_DIR}/testu01/svaria.c
  ${TEST_U01_DIR}/testu01/swalk.c
  ${TEST_U01_DIR}/testu01/swrite.c
  ${TEST_U01_DIR}/testu01/tu01_sha1.c
  ${TEST_U01_DIR}/testu01/uautomata.c
  ${TEST_U01_DIR}/testu01/ubrent.c
  ${TEST_U01_DIR}/testu01/ucarry.c
  ${TEST_U01_DIR}/testu01/ucrypto.c
  # ${TEST_U01_DIR}/testu01/ucryptoIS.c
  ${TEST_U01_DIR}/testu01/ucubic.c
  ${TEST_U01_DIR}/testu01/udeng.c
  ${TEST_U01_DIR}/testu01/ufile.c
  ${TEST_U01_DIR}/testu01/ugfsr.c
  ${TEST_U01_DIR}/testu01/ugranger.c
  ${TEST_U01_DIR}/testu01/uinv.c
  ${TEST_U01_DIR}/testu01/uknuth.c
  ${TEST_U01_DIR}/testu01/ulcg.c
  ${TEST_U01_DIR}/testu01/ulec.c
  ${TEST_U01_DIR}/testu01/umarsa.c
  ${TEST_U01_DIR}/testu01/umrg.c
  ${TEST_U01_DIR}/testu01/unif01.c
  ${TEST_U01_DIR}/testu01/unumrec.c
  ${TEST_U01_DIR}/testu01/uquad.c
  ${TEST_U01_DIR}/testu01/usoft.c
  ${TEST_U01_DIR}/testu01/utaus.c
  ${TEST_U01_DIR}/testu01/utezuka.c
  ${TEST_U01_DIR}/testu01/utouzin.c
  ${TEST_U01_DIR}/testu01/uvaria.c
  ${TEST_U01_DIR}/testu01/uweyl.c
  ${TEST_U01_DIR}/testu01/uwu.c
  # ${TEST_U01_DIR}/testu01/uxorshift.c
  ${TEST_U01_DIR}/testu01/vectorsF2.c
)

#=========== eval_xs128
add_executable(eval_xs128
  ${TEST_U01}
  ../src/eval.c
  ../src/eval_xs128.cpp
)

target_include_directories(eval_xs128
  PUBLIC
    ../src
    ${TEST_U01_DIR}/testu01
    ${TEST_U01_DIR}/include
)
target_compile_features(eval_xs128
  PRIVATE cxx_std_11
  PRIVATE c_std_99
)


#=========== eval_xpp
add_executable(eval_xpp
  ${TEST_U01}
  ../src/eval.c
  ../src/eval_xpp.cpp
)

target_include_directories(eval_xpp
  PUBLIC
    ../src
    ${TEST_U01_DIR}/testu01
    ${TEST_U01_DIR}/include
)
target_compile_features(eval_xpp
  PRIVATE cxx_std_11
  PRIVATE c_std_99
)


#=========== eval_xss
add_executable(eval_xss
  ${TEST_U01}
  ../src/eval.c
  ../src/eval_xss.cpp
)

target_include_directories(eval_xss
  PUBLIC
    ../src
    ${TEST_U01_DIR}/testu01
    ${TEST_U01_DIR}/include
)
target_compile_features(eval_xss
  PRIVATE cxx_std_11
  PRIVATE c_std_99
)

