echo \" <<'BATCH_SCRIPT' >/dev/null ">NUL "\" \`" <#"
@ECHO OFF
REM ===== Batch Script Begin =====
rem Original polyglot file: https://github.com/llamasoft/polyshell.

gcc main.c -Wall -Wextra -Wpedantic -o EmulRISC.exe
rem 2> stderr.txt

rem Don't know what to write here...

REM ====== Batch Script End ======
GOTO :eof
TYPE CON >NUL
BATCH_SCRIPT
#> | Out-Null


set +o histexpand 2>/dev/null
# ===== Bash Script Begin =====
# Original polyglot file: https://github.com/llamasoft/polyshell.

PROGNAME="main"
OUTPROGNAME="emulRISC.out"
BUILDSUCCESS=0

gcc $PROGNAME.c -o $OUTPROGNAME -lncurses -Wall -Wextra -Wpedantic && BUILDSUCCESS=1 #&& sleep 0.2 && ./$OUTPROGNAME

if [[ "$1" == "r" || "$1" == "run" ]]; then
	if [[ $BUILDSUCCESS == 1 ]]; then
	    ./$OUTPROGNAME
	fi
fi

unset BUILDSUCCESS
unset OUTPROGNAME
unset PROGNAME

# ====== Bash Script End ======
case $- in *"i"*) cat /dev/stdin >/dev/null ;; esac
exit
#>
