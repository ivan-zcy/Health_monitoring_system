#!/bin/bash

for i in {1..9}
do
    scp -r ~/piHealth/ gin.tama@pi$i:~/
done
