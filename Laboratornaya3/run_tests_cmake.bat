@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ======================================
echo  Сборка и тестирование Calculator
echo ======================================

if exist build rmdir /s /q build
mkdir build
cd build

echo [1/2] Generating build files...
cmake -G "MinGW Makefiles" .. >nul 2>&1
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    cd .. & pause & exit /b 1
)

echo [2/2] Building project...
mingw32-make -j4 >nul 2>&1
if %errorlevel% neq 0 (
    echo Build failed!
    cd .. & pause & exit /b 1
)

cd ..
echo ✅ Compilation successful!
echo ======================================
echo  Запуск тестов
echo ======================================

set passed=0
set total=0

call :run_test "Тест 1: Пример из задания" "17" "evaluate 2 x y 3 4 x*x+y*2"
call :run_test "Тест 2: Тригонометрия" "2" "evaluate 1 x 0 sin(x)+cos(x)+exp(x)"
call :run_test "Тест 3: Приоритет операций" "0" "evaluate 0 10+-5*2"
call :run_test "Тест 4: Возведение в степень" "8" "evaluate 0 16^(3/4)"
call :run_test "Тест 5: Производная в точке" "12" "evaluate_derivative 1 x 2 x^3"
call :run_test "Тест 6: Производная sin(x)" "cos" "derivative 1 x 0 sin(x)"
call :run_test "Тест 7: Деление на ноль (+)" "inf" "evaluate 1 x 0 5/x"
call :run_test "Тест 8: Деление на ноль (-)" "-inf" "evaluate 1 x 0 -5/x"
call :run_test "Тест 9: Логарифм нуля" "-inf" "evaluate 0 log(0)"
call :run_test "Тест 10: Корень из отрицательного" "ERROR Domain error: sqrt(-1)" "evaluate 1 x -1 sqrt(x)"
call :run_test "Тест 11: Логарифм отрицательного" "ERROR Domain error: log(x) requires x >= 0" "evaluate 0 log(-5)"
call :run_test "Тест 12: acos вне диапазона" "ERROR Domain error: acos(x) requires |x| <= 1" "evaluate 0 acos(2)"
call :run_test "Тест 13: asin вне диапазона" "ERROR Domain error: asin(x) requires |x| <= 1" "evaluate 0 asin(2)"
call :run_test "Тест 14: Дробная степень" "ERROR Domain error: fractional power of negative number" "evaluate 0 (-2)^0.5"
call :run_test "Тест 15: 0/0" "ERROR Domain error: anything/0.0 is undefined" "evaluate 0 0/0"
call :run_test "Тест 16: Несколько точек" "ERROR: multiple dots in a number" "evaluate 0 1.2.3+4"
call :run_test "Тест 17: Ведущие нули" "ERROR: leading zeros are not allowed" "evaluate 0 005+1"
call :run_test "Тест 18: Число с точкой в конце" "ERROR: number cannot end with a dot" "evaluate 0 5.+1"
call :run_test "Тест 19: Буква после числа" "ERROR: number cannot be directly followed by a letter" "evaluate 0 2x+1"
call :run_test "Тест 20: Пустые скобки (функция)" "ERROR: Empty parentheses" "evaluate 0 sin()"
call :run_test "Тест 21: Пустые скобки" "ERROR: Empty parentheses" "evaluate 0 ()"
call :run_test "Тест 22: Функция без скобок" "ERROR: Function sin must be followed by '('" "evaluate 0 sin x"
call :run_test "Тест 23: Неизвестная функция" "ERROR Unknown function: unknown" "evaluate 0 unknown(5)"
call :run_test "Тест 24: Переменная-функция" "ERROR: Variable name cannot be a reserved function name (sin)" "evaluate 1 sin 5 sin+2"
call :run_test "Тест 25: Дублирование переменной" "ERROR: Duplicate variable defined: x" "evaluate 2 x x 1 2 x+5"
call :run_test "Тест 26: Неизвестная переменная" "ERROR Unknown variable: z" "evaluate 1 x 5 x+z"
call :run_test "Тест 27: Длинное выражение" "6" "evaluate 0 1+2+3+4+5+6-4-3-2-1"
call :run_test "Тест 28: Вложенные функции" "1" "evaluate 0 sin(asin(1))"
call :run_test "Тест 29: Сложная степень" "27" "evaluate 1 x 3 x^3"
call :run_test "Тест 30: Много переменных" "20" "evaluate 3 a b c 1 2 3 a*b+c"

echo ======================================
echo  Результаты: Успешно %passed% из %total%
echo ======================================
if %passed% equ %total% (
    echo  ✅ ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО!
    color 2A
) else (
    echo  ❌ ЕСТЬ ОШИБКИ В ТЕСТАХ!
    color 4C
)
pause
exit /b

:run_test
set test_name=%~1
set expected=%~2
set input=%~3

:: Безопасный ввод (работает даже с ^ и спецсимволами)
echo(!input! | build\calculator.exe > temp_output.txt 2>&1

:: Убираем переводы строк из конца вывода
for /f "delims=" %%A in (temp_output.txt) do set "actual=%%A"

:: Точное сравнение (без учёта регистра)
if /i "!actual!"=="%expected%" (
    echo [ УСПЕХ ] %test_name%
    set /a passed+=1
) else (
    echo [ОШИБКА] %test_name%
    echo         Вход: %input%
    echo         Ожидалось: "%expected%"
    echo         Получено:  "%actual%"
)
set /a total+=1
del temp_output.txt 2>nul
exit /b
