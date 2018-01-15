cd ../build.linux
echo "Rebuild Nachos"
make clean
make 

cd ../test
make clean
make
../build.linux/nachos -f
../build.linux/nachos -cp FS_test1 /FS_test1
echo "========================================="
../build.linux/nachos -e /FS_test1
echo "========================================="
../build.linux/nachos -p /file1
echo "========================================="
../build.linux/nachos -cp FS_test2 /FS_test2
echo "========================================="
../build.linux/nachos -e /FS_test2
