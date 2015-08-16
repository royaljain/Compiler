#! /bin/bash
cd Lab5
make
./a.out <test-code1.c >../code.asm 
cd ..
make
./a.out
