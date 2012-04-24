#!/bin/bash

cd `dirname $0`
. ./_lib.sh

echo "Output tests"
check "detail" 2001:db8:0:0:0:0:0:1 -Bd 2001:db8:0:0:0:0:0:1 -i
check "short" 2001:db8::1 -Bs 2001:db8::1 -i
check "full" 2001:0db8:0000:0000:0000:0000:0000:0001 -Bf 2001:db8::1 -i
check "detail/prefix" 2001:db8:0:0:0:0:0:1/31 -Bd 2001:db8:0:0:0:0:0:1 /31 -6
check "short/prefix" 2001:db8::1/31 -Bs 2001:db8::1 /31 -6
check "full/prefix" 2001:0db8:0000:0000:0000:0000:0000:0001/31 -Bf 2001:db8::1 /31 -6
check "short start" ::1 -Bs ::1 -i
check "short end" 1:: -Bs 1:: -i
check "NULL addr" :: -Bsi
check "short start/prefix" ::1/31 -Bs ::1 /31 -6
check "short end/prefix" 1::/31 -Bs 1:: /31 -6
check "NULL addr/prefix" ::/31 -Bs /31 -6
check "IPv4" 10.1.2.3 ::a01:203 -4
check "MAC" 00-11-22-33-44-aa fe80::211:22ff:fe33:44aa -M
check "Net ID" 2001:db8:: 2001:db8::1/40 -n
check "Net ID/prefix" 2001:db8::/40 2001:db8::1/40 -N
check "Host ID" ::1 2001:db8::1/40 -H
check "Host ID/prefix" ::1/40 2001:db8::1/40 -I
check "/prefix" /40 2001:db8::1/40 -m
check "embedded force n" 2001:db8::1.2.3.4 2001:db8::1.2.3.4 -bi
check "embedded force y" ::1.2.3.4 ::1.2.3.4 -bi
check "embedded none n" 2001:db8::102:304 2001:db8::1.2.3.4 -Bi
check "embedded none y" ::102:304 ::1.2.3.4 -Bi
check "embedded auto n" 2001:db8::102:304 2001:db8::1.2.3.4 -ai
check "embedded auto y" ::1.2.3.4 ::1.2.3.4 -ai
check "embedded auto x" ::1 ::1 -ai
check "embedded auto N" :: :: -ai

checkfail "no MAC" fe80::1122:3344:55:66 -M

print_result

exit $FAIL