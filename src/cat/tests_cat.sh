#!/bin/bash


S21_CAT="./s21_cat" #присваивание пути переменной


compare_output() {
    local flag=$1 #функция принимает два аргумента флаги и имя файла
    local file=$2


    cat $flag "$file" > original_output.txt
    $S21_CAT $flag "$file" > custom_output.txt #наша программа

    # Сравниваем результаты обеих программ
    if diff -u original_output.txt custom_output.txt > /dev/null; then
        echo "Тест [Флаги: $flag, Файл: $file]: Успешно"
    else
        echo "Тест [Флаги: $flag, Файл: $file]: Ошибка"
        echo "Различия:"
        diff -u original_output.txt custom_output.txt
    fi


    rm original_output.txt custom_output.txt #удаление временных файлов
}

files=(
    "tests/test_1_cat.txt"     #массивы files и flags с списками путей к файлам и флагов
    "tests/test_2_cat.txt"
    "tests/test_3_cat.txt"
    "tests/test_4_cat.txt"
    "tests/test_5_cat.txt"
    "tests/test_case_cat.txt"

)

flags=(
    ""
    "-b"
    "-n"
    "-s"
    "-e"
    "-t"
    "-v"
)


echo "AUTO TESTS"
echo "---------------------------------------------------------" #загаловок и разделительная линия

for file in "${files[@]}"; do 
    for flag in "${flags[@]}"; do #циклы по файлам и флагам
        if [[ -f $file ]]; then
            compare_output "$flag" "$file"
        else
            echo "Файл не найден: $file"
        fi
    done
    echo "---------------------------------------------------------"
done

echo "Тестирование завершено."
