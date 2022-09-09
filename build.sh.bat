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
OUTPROGNAME="EmulRISC.out"
BUILDSUCCESS=0

g++ $PROGNAME.cpp -o $OUTPROGNAME -Wall -Wextra && echo -e "\n\e[1;92mBuilded\e[1;m.\n" && BUILDSUCCESS=1 #&& sleep 0.2 && ./$PROGNAME.elf

if [[ "$1" == "r" || "$1" == "run" ]]; then
	if [[ $BUILDSUCCESS == 1 ]]; then
	    ./*.elf
	fi
fi

unset BUILDSUCCESS
unset OUTPROGNAME
unset PROGNAME

# ====== Bash Script End ======
case $- in *"i"*) cat /dev/stdin >/dev/null ;; esac
exit
#>
