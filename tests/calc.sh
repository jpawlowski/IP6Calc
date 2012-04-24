#!/bin/bash

cd `dirname $0`
. ./_lib.sh

echo "Merge tests"
check "merge default" 2001:db8::1 2001:db8::ff/64 ff::1 -i
check "merge host" 2001:db8::1 2001:db8::ff/64 -e ff::1 -i
check "merge net" 2001:db8::1 ff::1/64 -E 2001:db8::ff -i
check "merge none" 2001:db8::1 2001:db8::1/128 ff::ff -i
check "merge all" 2001:db8::1 ff::ff/0 2001:db8::1 -i
check "merge all net" 2001:db8::1 ff::ff/128 -E 2001:db8::1 -i
check "merge none net" 2001:db8::1 2001:db8::1/0 -E ff::ff -i
check "merge complex" 2001:db8::1/64 ff:db8:ff::ff/32 ::1/16 -E 2001:ee:ee::ee/64 -6
check "merge prefix" 2001:db8::1/64 2001:db8::ff/32 ::1/16 /64 -6

echo
echo "Shift tests"
check "shr 2 blocks" ::111:222:333:444:555:666 111:222:333:444:555:666:777:888 --shr=32 -i
check "shr 2.5 blocks" ::1:1102:2203:3304:4405:5506 111:222:333:444:555:666:777:888 --shr=40 -i
check "shl 2 blocks" 333:444:555:666:777:888:: 111:222:333:444:555:666:777:888 --shl=32 -i
check "shl 2.5 blocks" 3304:4405:5506:6607:7708:8800:: 111:222:333:444:555:666:777:888 --shl=40 -i

echo
echo "Rotate tests"
check "ror 2 blocks" 777:888:111:222:333:444:555:666 111:222:333:444:555:666:777:888 --ror=32 -i
check "ror 2.5 blocks" 6607:7708:8801:1102:2203:3304:4405:5506 111:222:333:444:555:666:777:888 --ror=40 -i
check "rol 2 blocks" 333:444:555:666:777:888:111:222 111:222:333:444:555:666:777:888 --rol=32 -i
check "rol 2.5 blocks" 3304:4405:5506:6607:7708:8801:1102:2203 111:222:333:444:555:666:777:888 --rol=40 -i


print_result

exit $FAIL