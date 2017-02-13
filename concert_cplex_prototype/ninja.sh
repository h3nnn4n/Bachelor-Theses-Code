#!/bin/bash

in=$1
out=$2
file=$3

param1=${in}'.*good'
param2=${in}'.*bad'
param3=${in}'.*uniq'

egrep -c -i $param1 $file >  $out
egrep -c -i $param2 $file >> $out
egrep -c -i $param3 $file >> $out

