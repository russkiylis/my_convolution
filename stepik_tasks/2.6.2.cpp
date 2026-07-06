//2.6 Динамическая память
// Реализуйте функцию getline, которая считывает поток ввода посимвольно, 
// пока не достигнет конца потока или не встретит символ переноса строки ('\n'), 
// и возвращает C-style строку с прочитанными символами.

// Обратите внимание, что так как размер ввода заранее неизвестен, 
// то вам нужно будет перевыделять память в процессе чтения, 
// если в потоке ввода оказалось больше символов, чем вы ожидали.

// Память, возвращенная из функции будет освобождена оператором delete[]. 
// Символ переноса строки ('\n') добавлять в строку не нужно, но не забудьте, 
// что в конце C-style строки должен быть завершающий нулевой символ.

// Требования к реализации: при выполнении данного задания вы можете определять любые 
// вспомогательные функции, если они вам нужны. Определять функцию main не нужно.

#include <iostream>

char *getline()
{
    size_t symbols = 100;
    
    // Выделим изначальную некую память в куче
    char * str = new char[symbols];
    char * strPointer = str;
    
    int readedChar = 0;
    
    // Будем читать символы в цикле
    while (readedChar != EOF && readedChar != '\n') {
        readedChar = getchar();
        if (readedChar == EOF || readedChar == '\n')
            break;
        *strPointer = (char)readedChar;
        
        strPointer++;
        // Если надо, пересоздаём str но с большим количеством значений
        if (strPointer == str + symbols - 1) {    // - 1 потому что должно быть местечко под \0
            char * smallStr = new char[symbols];
            for (size_t k = 0; k < symbols; k++)
                smallStr[k] = str[k];
            delete [] str;
            
            symbols += 100;
            str = new char[symbols];
            strPointer = str + symbols - 101;
            
            for (size_t k = 0; k < symbols - 100; k++)
                str[k] = smallStr[k];
            delete [] smallStr;
        }
    }
    
    // Добавляем в конец символ завершения строки
    *strPointer = '\0';
    
    // Отрезаем лишнее
    size_t totalSymbols = strPointer - str;
    char * result = new char[totalSymbols+1];
    for (size_t i = 0; i <= totalSymbols; i++) {
        result[i] = str[i];
    }
    delete [] str;
    
    return result;
}