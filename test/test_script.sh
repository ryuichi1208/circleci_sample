#!/bin/bash

TXT_A="../text/q_001.txt"
TXT_B="../text/q_002.txt"
PY_CODE="../src/s002.py"

# Files check
if [[ ! -f ${TXT_A} ]]; then
    echo "[ERROR] File does not exist."
    exit 1
fi

if [[ ! -f ${TXT_B} ]]; then
    echo "[ERROR] File does not exist."
    exit 1
fi

if [[ ! -f ${PY_CODE} ]]; then
    echo "[ERROR] File does not exist."
    exit 1
fi


# Function
function python_test(){
    PY_CMD="/usr/local/bin/python3.6 ${1} < ${2}"

    eval ${PY_CMD}
    RC=$?

    if [[ ${DEBUG:-0} == 1 ]]; then
        echo "[DEBUG] Following command. CMD: ${CMD}"
    fi

    if [[ ${RC} != 0 ]]; then
        echo "[ERROR] Failed to python command. CMD: ${CMD}"
        exit 1
    fi

}

# MAIN
RESULT_A=$(python_test ${PY_CODE} ${TXT_A})
RESULT_B=$(python_test ${PY_CODE} ${TXT_B})

if [[ ${RESULT_A} != 1999999998 ]]; then
    exit 1
fi

if [[ ${RESULT_B} != 39 ]]; then
    exit 1
fi

exit 0
