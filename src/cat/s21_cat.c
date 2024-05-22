#include "s21_cat.h"

int main(int argc, char *argv[]) {
  Flag options = ScanFlag(argc, argv);

  if (optind == argc) {  // optind - глобальная переменная, которая указывает на
                         // текущий индекс в массиве аргументов командной строки
                         // argv после обработки всех опций
    WorkWithFile(NULL,
                 options);  //если нет файлов в аргументов, читаем из stdin
  } else {
    while (optind < argc) {  //обработка всех указанных файлов
      if (WorkWithFile(argv[optind], options) != 0) {
        return 1;  //ошибка при обработке файла
      }
      optind++;  //переход к следующему аргументу
    }
  }

  return 0;
}

Flag ScanFlag(
    int argc,
    char *argv[]) {  //функция анализирует флаги и возвращает структуру Flags
  Flag options = {0};  //инициализируем структуру нулями

  struct option long_options[] = {
      {"number-nonblank", no_argument, 0, 'b'},  //определение длинных опций
      {"number", no_argument, 0, 'n'},
      {"squeeze-blank", no_argument, 0, 's'},
      {0, 0, 0, 0}};

  int option;  //переменная хранит результат каждого вызова гетопт
  while ((option = getopt_long(argc, argv, "benstvET", long_options, NULL)) !=
         -1) {  //цикл вызывает getopt_long для разборки каждой опции
    switch (option) {
      case 'b':
        options.flag_b = 1;
        break;
      case 'e':
        options.flag_e = 1;
        options.flag_v = 1;
        break;
      case 'n':
        options.flag_n = 1;
        break;
      case 's':
        options.flag_s = 1;
        break;
      case 't':
        options.flag_t = 1;
        options.flag_v = 1;
        break;
      case 'v':
        options.flag_v = 1;
        break;
      case 'E':
        options.flag_e = 1;
        break;
      case 'T':
        options.flag_t = 1;
        break;
      default:
        break;
    }
  }
  return options;
}

int WorkWithFile(const char *filename,
                 Flag options) {  //функция читает и изменяет файл
  FILE *file;
  if (filename == NULL) {
    file = stdin;
  } else {
    file = fopen(filename, "r");
    if (file == NULL) {
      printf("Error opening file: %s\n", filename);
      return 1;
    }
  }

  options.countLines = 1;  //инициализируем счетчики
  options.countEmptyLines = 0;

  int last_char = '\n';

  while (1) {
    int current_char = fgetc(file);  //чтение текущего символа из файла

    if (current_char == EOF) {  //файл закончился, выход из цикла
      break;
    }

    if (options.flag_s && current_char == '\n' &&
        last_char == '\n') {  //если текущий и предыдущий символ это переход на
                              //новую строку, то счетчик растет
      options.countEmptyLines++;
      if (options.countEmptyLines > 1) {
        continue;
      }
    } else {
      options.countEmptyLines = 0;
    }

    if (last_char == '\n' &&
        ((options.flag_b && current_char != '\n') || options.flag_n)) {
      printf("%6d\t",
             options.countLines++);  //если предыдущий символ новая строка а
                                     //текущий нет, то выводится номер строки
    }

    if (options.flag_t && current_char == '\t') {
      printf("^");
      current_char =
          'I';  //если текущий символ это табуляция то он заменяется на
    }

    if (options.flag_e && current_char == '\n') {
      printf(
          "$");  //если текущий символ это новая строка то выводится долларррррр
    }

    if (options.flag_v) {
      if ((current_char >= 0 && current_char < 9) ||
          (current_char > 10 && current_char < 32)) {
        printf("^");
        current_char += 64;
      } else if (current_char == 127) {
        printf("^");
        current_char = '?';
      }
    }
    printf("%c", current_char);
    last_char = current_char;
  }

  if (file != stdin) {
    fclose(file);
  }
  return 0;
}
