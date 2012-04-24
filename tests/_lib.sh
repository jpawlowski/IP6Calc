#Test Library for ip6calc
# (c) Konrad Rosenbaum, 2012
# protected under the GNU GPLv3 or at your option any newer

#amount of tests executed
TCNT=0
#amount of PASSES
PASS=0
#amount of FAILS
FAIL=0

#checks the result of a call to ip6calc
# Arguments:
#  $1: simple description of the test, this is output right away
#  $2: expected output of ip6calc
#  $3...$n: arguments for the call to ip6calc
# Pass: if the result matches $2 and the return code is 0
check(){
	echo -n "Testing $1 ..."
	TCNT=`expr $TCNT + 1`
	shift
	exp="$1" ; shift
	res=`../ip6calc "$@"`; ret=$?
	test x"$exp" == x"$res" -a $ret -eq 0 && {
		PASS=`expr $PASS + 1`
		echo passed
	} || {
		FAIL=`expr $FAIL + 1`
		echo "FAILED"
		echo " args: $@"
		echo " return code: $ret"
		echo " expecting: '$exp'"
		echo " have got : '$res'"
	}
}

#checks that a call to ip6calc fails
# Arguments:
#  $1: simple description of the test, this is output right away
#  $2...$n: arguments for the call to ip6calc
# Pass: if the return code is non-zero, no check of output
checkfail(){
	echo -n "Testing $1 ..."
	shift
	res=`../ip6calc "$@" 2>&1`; ret=$?
	test $ret -ne 0 && {
		PASS=`expr $PASS + 1`
		echo "passed ($ret)"
	} || {
		FAIL=`expr $FAIL + 1`
		echo "FAILED (while expecting non-zero return code)"
		echo " args: $@"
		echo " output: $res"
	}
}

#prints the overall statistics
print_result(){
	echo
	echo '==================================='
	echo "Executed $TCNT tests"
	echo "Passed $PASS tests"
	echo "Failed $FAIL tests"
}