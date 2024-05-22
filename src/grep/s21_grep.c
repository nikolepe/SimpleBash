#include "s21_grep.h"

int main(int argc, char **argv) {
  if (argc > 1) {       //проверка что есть аргументы
    char *temp = NULL;  //объявляем указатель temp для хранения шаблона
    Flags options = {0};  //заполняем структуру нулями

    int x = ScanFlag(argc, argv, &options, &temp);
    while (x < argc) {
      FILE *file = fopen(argv[x], "r");
      if (file != NULL) {
        ReadFile(file, &options, temp, argv[x]);
        fclose(file);
      } else if (options.flag_s == 0) {
        fprintf(stderr, "s21_grep: %s: No such file or directory\n", argv[x]);
      }
      x++;
    }

    if (temp != NULL) free(temp);  //освобождаем память выделенную для шаблона
  }
  return 0;
}

void ReadFile(FILE *file, Flags *options, char *pattern, char *path) {
  for (char ch = '0'; (ch = getc(file)) != EOF;) {  //чтение символов из файла
    char *line = calloc(256, sizeof(char));  //выделяем память для строки
    options->numberLine += 1;  //счетчик строк
    int length = 0;

    for (line[length] = '\0'; ch != EOF && ch != '\n';
         ch = getc(file)) {  //читаем символы до конца строки или файла и
                             //сохраняем в массив line. если строка больше 255,
                             //выделяется доп память
      line[length] = ch;
      line[length += 1] = '\0';
      if (length % 255 == 0) {
        line = reallocString(line, length + 256);
      }
    }

    WorkWithLines(line, options, pattern, path);
    free(line);
  }
  auxProcess(*options, path);
}

int ScanFlag(int argc, char **argv, Flags *options,
             char **pattern) {  //функция анализирует флаги и возвращает индекс
                                //первого неразобранного аргумента
  int option;
  while ((option = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (option) {
      case 'i':
        options->flag_i = 1;
        break;
      case 'v':
        options->flag_v = 1;
        break;
      case 'c':
        options->flag_c = 1;
        break;
      case 'l':
        options->flag_l = 1;
        break;
      case 'n':
        options->flag_n = 1;
        break;
      case 'h':
        options->flag_h = 1;
        break;
      case 's':
        options->flag_s = 1;
        break;
      case 'o':
        options->flag_o = 1;
        break;
      case 'e':
        Option_e(options, pattern, optarg);
        break;
      case 'f':
        Option_f(options, pattern, optarg);
        break;
    }
  }

  if ((options->flag_e || options->flag_f) == 0) {
    SetupPattern(pattern, argv[optind]);
    optind += 1;
  }

  WorkWithFlags(options, argc);
  return optind;
}

void Option_f(Flags *options, char **pattern,
              char *optarg) {  //функция обрабатывает опцию -f. принимает
                               //указатель на структуру, указатель на строку
                               //шаблона и аргумент опции(имя файла)
  FILE *file = fopen(optarg, "r");  //
  if (file != NULL) {
    for (char ch = '0';
         (ch = getc(file)) !=
         EOF;) {  //инициализируем переменную ch для чтения сомволов
      int length = 0;
      char *line = calloc(256, sizeof(char));

      for (line[length] = '\0'; ch != EOF && ch != '\n';
           ch = getc(file)) {  //чтение строки
        line[length] = ch;
        line[length += 1] = '\0';
        if (length % 255 == 0) line = (char *)reallocString(line, length + 256);
      }

      if (!(options->flag_e ||
            options->flag_f))  //если нет флагов е и ф то устанавливается шаблон
                               //функцией SetupPattern
        options->flag_f = SetupPattern(pattern, line);
      else
        AddPattern(pattern,
                   line);  //в противном случае шаблон дополняется новой строкой
      free(line);
    }
    fclose(file);
  } else if (options->flag_s == 0) {
    fprintf(stderr, "s21_grep: %s: No such file or directory\n", optarg);
  }
}

void Option_e(Flags *options, char **pattern,
              char *optarg) {  //фунцкия обрабатывает флаг е
  if (!(options->flag_e || options->flag_f))
    options->flag_e = SetupPattern(pattern, optarg);
  else
    AddPattern(pattern, optarg);
}

void auxProcess(
    Flags options,
    char *path) {  //функция обрабатывает вспомогательные действия после
                   //обработки файла, принимает указатель на имя файла
  if (options.flag_c) {  //проверка установлен ли с флаг(подсчет совпалений)
    if (options.flag_l) {  //если установлен л флаг (вывод только имен файлов с
                           //совпадениями), то выводится имя файла и 1
      options.countFiles > 1 ? printf("%s:1\n", path) : printf("1\n");
    } else {
      if (options.countFiles > 1)
        printf("%s:",
               path);  //в противном случае если количество файлов больше
                       //одного, перед подсчетом совпадений выводится имя файла
      printf("%i\n", options.countMatches);
    }
  }
  if (options.flag_l && options.countMatches) printf("%s\n", path);
}

void WorkWithLines(
    char *line, Flags *options, char *pattern,
    char *path) {  //функция обрабатывает строку файла, line - строка
  regex_t regex;   //компиляция pattern в структуру regex
  if (regcomp(&regex, pattern, options->flag_i ? REG_ICASE : REG_EXTENDED) ==
      0) {  //если утсановлен флаг i, то используется флаг REG_ICASE, иначе
            // REG_EXTENDED
    if (regexec(&regex, line, 0, NULL, 0) == options->flag_v) {
      options->countMatches +=
          1;  //поиск регулярного выражения с троке line. если флаг v
              //установлен, результат сравниваетс с 0, дальше идет увеличение
              //счетчика совпадений
      if ((options->flag_c || options->flag_l) == 0) {
        if (options->countFiles > 1 && !(options->flag_h)) printf("%s:", path);
        if (options->flag_n) printf("%i:", options->numberLine);
        if (!options->flag_o)
          printf("%s\n", line);
        else
          Option_o(regex, line, *options);
      }
    }
    regfree(&regex);  //освобождение памяти
  }
}

void Option_o(regex_t regex, char *line,
              Flags options) {  //функция обрабатывает флаг о(выводим только
                                //совпадающие части строки)
  while (regexec(&regex, line, 0, NULL, 0) ==
         options.flag_v) {  //цикл продолжается пока в строке лайн есть
                            //совпадения с рег выражением regex
    char *aux = (char *)calloc(
        strlen(line) + 1,
        sizeof(char));  //выделяем память и копируем в аукс строку лайн
    strcpy(aux, line);
    // int end = strlen(line);
    while (regexec(&regex, aux, 0, NULL, 0) == options.flag_v) {
      // end--;
      aux[strlen(aux) - 1] =
          0;  //уменьшаем строку на 1 символ пока есть совпадения
    }
    aux[strlen(aux)] = line[strlen(aux)];
    int start = 0;
    while (regexec(&regex, aux, 0, NULL, 0) ==
               options.flag_v &&  //нахождения начала совпадения и сдвиг строки
                                  // aux влево на 1
           strlen(aux) > 0) {
      start++;
      int j = 0;
      while (aux[j] != 0) {
        aux[j] = aux[j + 1];
        j++;
      }
    }
    start--;
    int i = strlen(aux);
    while (i != 0) {
      aux[i] = aux[i - 1];  //сдвиг строки вправо и восстановление первого
                            //сивола совпадения
      i--;
    }
    aux[0] = line[start];
    printf("%s\n", aux);  //вывод совпадения
    free(aux);
    i = start + 1;
    while (line[i] != 0) {
      line[i - start - 1] = line[i];
      i++;
    }
    line[i - start - 1] =
        0;  //удаление совпавшей части из строки line и продолжение поиска
  }
}

void WorkWithFlags(
    Flags *options,
    int argc) {  //функция устанавливает основные флаги для структуры
  if (options->flag_o &&
      (options->flag_l || options->flag_v || options->flag_c))
    options->flag_o = 0;
  options->countFiles =
      argc - optind;  //подсчитываем количестов файлов для обработки
}

int SetupPattern(
    char **str,
    char *optarg) {  //функция утсанавливает начальный шаблон, принимает
                     //аргумент опции и указатель на строку шаблона
  *str = calloc(strlen(optarg) + 1, sizeof(char));
  if (*str) strcpy(*str, optarg);
  return str ? 1 : 0;  //копируем аргумент опции в строку шаблона если память
                       //выделена
}

void AddPattern(
    char **str,
    char *optarg) {  // функция добавляет новый шаблон к существующему
  *str = reallocString(*str, strlen(*str) + strlen(optarg) + 2);
  if (*str) strcat(strcat(*str, "|"), optarg);
}

void *reallocString(void *str,
                    int size) {  //функция выделяет доп память для строки
  char *aux = realloc(str, size);
  return aux;
}
