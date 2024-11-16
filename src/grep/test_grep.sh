#!/bin/bash

COUNTER_SUCCESS=0
COUNTER_FAIL=0
DIFF_RES=""
FILENAME1="src/grep/texts/1.txt"
FILENAME2="src/grep/texts/2.txt"
FILENAME3="src/grep/texts/3.txt"
FILENAME4="src/grep/texts/4.txt"
FILENAME5="src/grep/texts/5.txt"
LOG_DIR="testtxt"

mkdir -p $LOG_DIR

SHABLO=""
SHABLON="123"
SHABLON2="abc"
SHABLON3="[Xx]"
SHABLON4="[A-Z]"
SHABLON5="^[A-Z]"

for var in -v -c -l -n -i -h -s -o
do
    TEST1="$var $SHABLO \"$FILENAME1\" \"$FILENAME2\" \"$FILENAME3\" \"$FILENAME4\" \"$FILENAME5\""
    ./s21_grep $TEST1 > "$LOG_DIR/s21_grep$var.txt"
    grep $TEST1 > "$LOG_DIR/grep$var.txt"
    DIFF_RES="$(diff -s "$LOG_DIR/s21_grep$var.txt" "$LOG_DIR/grep$var.txt")"
    if [ "$DIFF_RES" == "Files $LOG_DIR/s21_grep$var.txt and $LOG_DIR/grep$var.txt are identical" ]
    then
        echo "SUCCESS $TEST1"
        (( COUNTER_SUCCESS++ ))
    else
        echo "FAIL $TEST1"
        echo "$TEST1" >> log.txt
        (( COUNTER_FAIL++ ))
    fi
    rm "$LOG_DIR/s21_grep$var.txt" "$LOG_DIR/grep$var.txt"
done

for var in -v -c -l -n -i -h -s -o
do
    for var2 in -v -c -l -n -i -h -s -o
    do
        if [ "$var" != "$var2" ]
        then
            TEST1="$var $var2 $SHABLO \"$FILENAME1\" \"$FILENAME2\" \"$FILENAME3\" \"$FILENAME4\" \"$FILENAME5\""
            ./s21_grep $TEST1 > "$LOG_DIR/s21_grep${var}${var2}.txt"
            grep $TEST1 > "$LOG_DIR/grep${var}${var2}.txt"
            DIFF_RES="$(diff -s "$LOG_DIR/s21_grep${var}${var2}.txt" "$LOG_DIR/grep${var}${var2}.txt")"
            if [ "$DIFF_RES" == "Files $LOG_DIR/s21_grep${var}${var2}.txt and $LOG_DIR/grep${var}${var2}.txt are identical" ]
            then
                echo "SUCCESS $TEST1"
                (( COUNTER_SUCCESS++ ))
            else
                echo "FAIL $TEST1"
                echo "$TEST1" >> log.txt
                (( COUNTER_FAIL++ ))
            fi
            rm "$LOG_DIR/s21_grep${var}${var2}.txt" "$LOG_DIR/grep${var}${var2}.txt"
        fi
    done
done

for SHABLON_VAR in "$SHABLON" "$SHABLON2" "$SHABLON3" "$SHABLON4" "$SHABLON5"
do
    for var in -v -c -l -n -i -h -s -o
    do
        TEST1="$var \"$SHABLON_VAR\" \"$FILENAME1\" \"$FILENAME2\" \"$FILENAME3\" \"$FILENAME4\" \"$FILENAME5\""
        ./s21_grep $TEST1 > "$LOG_DIR/s21_grep$var.txt"
        grep $TEST1 > "$LOG_DIR/grep$var.txt"
        DIFF_RES="$(diff -s "$LOG_DIR/s21_grep$var.txt" "$LOG_DIR/grep$var.txt")"
        if [ "$DIFF_RES" == "Files $LOG_DIR/s21_grep$var.txt and $LOG_DIR/grep$var.txt are identical" ]
        then
            echo "SUCCESS $TEST1"
            (( COUNTER_SUCCESS++ ))
        else
            echo "FAIL $TEST1"
            echo "$TEST1" >> log.txt
            (( COUNTER_FAIL++ ))
        fi
        rm "$LOG_DIR/s21_grep$var.txt" "$LOG_DIR/grep$var.txt"
    done
done

echo "SUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"
