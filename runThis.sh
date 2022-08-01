/snap/bin/cmake -DCMAKE_BUILD_TYPE=Release .
/snap/bin/cmake --build .  --target TVGGraphCalc -- -j 9
mv TVGGraphCalc  cmakeLoc/
cd cmakeLoc 
nohup ./TVGGraphCalc &
