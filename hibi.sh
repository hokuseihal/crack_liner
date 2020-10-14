find images/ | tail -n +2 | grep -v "txt" > img.txt;
./build/main img.txt;
