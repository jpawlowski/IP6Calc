#!/bin/bash

cd `dirname $0`
. ./_lib.sh

echo "Parser tests"
check "simple" 2001:db8:0:0:0:0:0:1 -Bd 2001:db8:0:0:0:0:0:1 -i
check "short 1" 2001:db8:0:0:0:0:0:1 -Bd 2001:db8::1 -i
check "short 2" 2001:db8:0:0:0:0:0:0 -Bd 2001:db8:: -i
check "short 3" 0:0:0:0:0:0:0:1 -Bd ::1 -i
check "short 4" 0:0:0:0:0:0:0:0 -Bd :: -i
check "with ip6:" 2001:db8:0:0:0:0:0:1 -Bd ip6:2001:db8::1 -i
check "ipv4" 0:0:0:0:0:0:a02:304 -Bd 10.2.3.4 -i
check "ipv4 with ip4:" 0:0:0:0:0:0:a02:304 -Bd ip4:10.2.3.4 -i
check "embedded ipv4" 0:0:0:0:0:0:a02:304 -Bd ::10.2.3.4 -i
check "embedded ipv4 b" 1:0:0:0:0:0:a02:304 -Bd 1::10.2.3.4 -i
check "embedded ipv4 c" 0:0:0:0:0:1:a02:304 -Bd ::1:10.2.3.4 -i
check "embedded ipv4 d" 1:0:0:0:0:2:a02:304 -Bd 1::2:10.2.3.4 -i
check "mac with colons" fe80:0:0:0:211:22ff:fe33:4455 -Bd 00:11:22:33:44:55 -i
check "mac with dashes" fe80:0:0:0:211:22ff:fe33:4455 -Bd 00-11-22-33-44-55 -i
check "mac with mac:" fe80:0:0:0:211:22ff:fe33:4455 -Bd mac:00:11:22:33:44:55 -i
check "with prefix" 2001:db8:0:0:0:0:0:1/31 -Bd 2001:db8::1/31 -6
check "null with prefix" 0:0:0:0:0:0:0:0/31 -Bd ::/31 -6
check "prefix only" 0:0:0:0:0:0:0:0/31 -Bd /31 -6

checkfail "invalid address" akjh:djks::ah
checkfail "invalid ipv4" 1.2.3.4000
checkfail "invalid mac" aa-bb-cc-dd-ee-zz
checkfail "ipv6 in ip4:" ip4:2001:db8::1
checkfail "ipv4 in ip6:" ip6:1.2.3.4
checkfail "mac in ip6:" ip6:00:11:22:33:44:55
checkfail "ipv4 in mac:" mac:1.2.3.4

print_result

exit $FAIL