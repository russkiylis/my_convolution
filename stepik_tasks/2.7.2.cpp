//2.7 Многомерные массивы
// Реализуйте функцию swap_min, которая принимает на вход двумерный массив целых чисел, 
// ищет в этом массиве строку, содержащую наименьшее среди всех элементов массива значение, 
// и меняет эту строку местами с первой строкой массива.

// Подумайте, как обменять строки массива, не обменивая элементы строк по-отдельности.

// Требования к реализации: при выполнении этого задания вы можете определять любые вспомогательные функции. 
// Вводить или выводить что-либо не нужно. Реализовывать функцию main не нужно.

void swap_min(int *m[], unsigned rows, unsigned cols)
{
    int * firstRowPtr = m[0];  
    int * swapRowPtr = m[0];
    int swapRowIndex = 0;
    int min = m[0][0];
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (m[i][j] < min) {
                min = m[i][j];
                swapRowPtr = m[i];
                swapRowIndex = i;
            }
        }
    }
    
    if (firstRowPtr == swapRowPtr)
        return;
    
    m[0] = swapRowPtr;
    m[swapRowIndex] = firstRowPtr;
    
    return;
}
