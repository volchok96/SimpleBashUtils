#!/bin/bash

COUNTER_SUCCESS=0
COUNTER_FAIL=0
DIFF_RES=""
TEST_FILES="src/cat/test1.txt src/cat/test2.txt"
echo "" > log.txt

# Проверка на существование файлов
if [ ! -f src/cat/test1.txt ] || [ ! -f src/cat/test2.txt ]; then
    echo "Один или оба тестовых файла отсутствуют!" | tee -a log.txt
    exit 1
fi

# Функция для выполнения тестов
run_test() {
    local flags="$1"
    echo "Testing with flags: $flags" | tee -a log.txt

    for file in $TEST_FILES; do
        echo "Running: ./src/cat/s21_cat $flags $file" | tee -a log.txt
        ./src/cat/s21_cat $flags "$file" > s21_cat.txt
        echo "Running: cat $flags $file" | tee -a log.txt
        cat $flags "$file" > cat.txt

        echo "Comparing files..." | tee -a log.txt
        DIFF_RES="$(diff -s s21_cat.txt cat.txt)"
        echo "Diff result: $DIFF_RES" | tee -a log.txt

        if [ "$DIFF_RES" == "Files s21_cat.txt and cat.txt are identical" ]; then
            echo "SUCCESS" | tee -a log.txt
            (( COUNTER_SUCCESS++ ))
        else
            echo "FAIL" | tee -a log.txt
            echo "$flags for $file" >> log.txt
            (( COUNTER_FAIL++ ))
        fi
    done
}

# Тестирование с одним флагом
for var in -v -b -e -n -s -t; do
    run_test "$var"
done

# Тестирование с двумя флагами
for var in -b -e -n -s -t -v; do
    for var2 in -b -e -n -s -t -v; do
        if [ "$var" != "$var2" ]; then
            run_test "$var $var2"
        fi
    done
done

# Тестирование с тремя флагами
for var in -b -e -n -s -t -v; do
    for var2 in -b -e -n -s -t -v; do
        for var3 in -b -e -n -s -t -v; do
            if [ "$var" != "$var2" ] && [ "$var2" != "$var3" ] && [ "$var" != "$var3" ]; then
                run_test "$var $var2 $var3"
            fi
        done
    done
done

# Тестирование с четырьмя флагами
for var in -b -e -n -s -t -v; do
    for var2 in -b -e -n -s -t -v; do
        for var3 in -b -e -n -s -t -v; do
            for var4 in -b -e -n -s -t -v; do
                if [ "$var" != "$var2" ] && [ "$var2" != "$var3" ] && [ "$var" != "$var3" ] && [ "$var" != "$var4" ] && [ "$var2" != "$var4" ] && [ "$var3" != "$var4" ]; then
                    run_test "$var $var2 $var3 $var4"
                fi
            done
        done
    done
done

echo "SUCCESS: $COUNTER_SUCCESS" | tee -a log.txt
echo "FAIL: $COUNTER_FAIL" | tee -a log.txt
