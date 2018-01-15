cd ../build.linux
echo "Rebuild Nachos"
make clean
make 

cd ../test
make clean
make
../build.linux/nachos -f
../build.linux/nachos -cp num_100.txt /100
../build.linux/nachos -cp num_1000.txt /1000	
../build.linux/nachos -p /1000
echo "========================================="
../build.linux/nachos -p /100
echo "========================================="
../build.linux/nachos -mkdir /t0
../build.linux/nachos -mkdir /t1
../build.linux/nachos -mkdir /t2
../build.linux/nachos -cp num_100.txt /t0/f1
../build.linux/nachos -mkdir /t0/aa
../build.linux/nachos -mkdir /t0/aa/bb
../build.linux/nachos -p /t0/f1
../build.linux/nachos -mkdir /t0/cc
../build.linux/nachos -cp num_100.txt /t0/aa/bb/f1
../build.linux/nachos -r /t0/f1
../build.linux/nachos -cp num_100.txt /t0/aa/bb/f4
../build.linux/nachos -l /
../build.linux/nachos -r /t0/aa/bb
../build.linux/nachos -cp num_1000.txt /t0/cc/f2
../build.linux/nachos -lr /
../build.linux/nachos -cp num_100.txt /t0/aa/f3
../build.linux/nachos -p /t0/cc/f2
../build.linux/nachos -mkdir /a
../build.linux/nachos -f
../build.linux/nachos -cp num_100.txt /a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/f1
../build.linux/nachos -p /a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/f1