// Синтаксичний аналіз простих мов
#include <iostream>
#include <locale>
#include <Windows.h>

using namespace std;

enum AnalysisStatus
{
    STATUS_EXPECT_VAR,
    STATUS_EXPECT_ID_LIST,
    STATUS_EXPECT_COMMA,
    STATUS_EXPECT_COLON,
    STATUS_EXPECT_VAR_TYPE,
    STATUS_EXPECT_SEMICOLON,
    STATUS_SYNTAX_ERROR,
};

AnalysisStatus status = STATUS_EXPECT_VAR;
bool bufferEnd = false;
int readOffset = 0;
int GetNextChar(char* buffer)
{
    int len = strlen(buffer);
    if (len > readOffset)
    {
        return buffer[readOffset++];
    }

    bufferEnd = true;

    return EOF;
}

bool IsAlpha(char ch)
{
    return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z';
}

bool IsDigit(char ch)
{
    return ch >= '0' && ch <= '9';
}

bool IsDataTypeWord(const char* word)
{
    if (!strcmp(word, "Int") ||
        !strcmp(word, "Real") ||
        !strcmp(word, "Char"))
    {
        return true;
    }

    return false;
}

bool IsReservedWord(const char* word)
{
    if (!strcmp(word, "var") ||
        IsDataTypeWord(word))
    {
        return true;
    }

    return false;
}

void Error(int column)
{
    switch (status)
    {
    case STATUS_EXPECT_VAR:
        printf("Помилка аналізу 'стовпець %d': очіувалося слово 'var'\n", column);
        break;
    case STATUS_EXPECT_ID_LIST:
        printf("Помилка аналізу 'стовпець %d': очікувався ідентифікатор\n", column);
        break;
    case STATUS_EXPECT_COMMA:
        printf("Помилка аналізу 'стовпець %d': очікувалась кома\n", column);
        break;
    case STATUS_EXPECT_COLON:
        printf("Помилка аналізу 'стовпець %d': очікувалась двокрапка\n", column);
        break;
    case STATUS_EXPECT_VAR_TYPE:
        printf("Помилка аналізу 'стовпець %d': очікувався тип даних\n", column);
        break;
    case STATUS_EXPECT_SEMICOLON:
        printf("Помилка аналізу 'стовпець %d': очікувалась крапка с комою\n", column);
        break;
    default:
        printf("Невідома помилка '%d', 'стовпець %d'\n", status, column);
        break;
    }

    printf("------------------------------------------\n");

    status = STATUS_SYNTAX_ERROR;
    readOffset = 0;
}

bool ParseVariableDeclaration(char* buf)
{
    if (!buf)
    {
        printf("Помилка аналізу: пустий рядок\n");
        return false;
    }

    printf("Рядок: '%s'\n", buf);

    status = STATUS_EXPECT_VAR;
    bufferEnd = false;

    while (!bufferEnd && status != STATUS_SYNTAX_ERROR)
    {
        int ch = GetNextChar(buf);

        if (ch == EOF &&
            status == STATUS_EXPECT_VAR ||
            status == STATUS_EXPECT_COMMA ||
            status == STATUS_EXPECT_COLON ||
            status == STATUS_EXPECT_SEMICOLON)
        {
            Error(readOffset);
            return false;
        }

        // пропускаємо пробіли
        while (ch == ' ')
            ch = GetNextChar(buf);

        if (ch == EOF &&
            status == STATUS_EXPECT_VAR ||
            status == STATUS_EXPECT_COMMA ||
            status == STATUS_EXPECT_COLON ||
            status == STATUS_EXPECT_SEMICOLON)
        {
            Error(readOffset);
            return false;
        }

        if ((status == STATUS_EXPECT_COMMA && ch != ',') ||
            (status == STATUS_EXPECT_COLON && ch != ':') || 
            (status == STATUS_EXPECT_SEMICOLON && ch != ';'))
        {
            Error(readOffset);
            return false;
        }

        // ідентифікатор чи ключове слово
        if (IsAlpha(ch) || IsDigit(ch))
        {
            int i = 0;

            // читаємо слово
            char word[32] = {};
            do
            {
                word[i++] = ch;
            } while ((ch = GetNextChar(buf)) && i < 31 && ch != EOF && IsAlpha(ch) || IsDigit(ch));

            if (status == STATUS_EXPECT_VAR && !strcmp(word, "var"))
            {
                status = STATUS_EXPECT_ID_LIST;
            }
            else if (status == STATUS_EXPECT_ID_LIST && !IsReservedWord(word))
            {
                status = STATUS_EXPECT_COMMA;
            }
            else if (status == STATUS_EXPECT_VAR_TYPE && IsDataTypeWord(word))
            {
                status = STATUS_EXPECT_SEMICOLON;
            }
            else
            {
                Error(readOffset - i);
                return false;
            }
        }

        if (status == STATUS_EXPECT_COMMA) // список ідентифікаторів
        {
            if (ch == ',')
            {
                status = STATUS_EXPECT_ID_LIST;
            }
            else
            {
                status = STATUS_EXPECT_COLON;
            }
        }

        if (status == STATUS_EXPECT_COLON && ch == ':')
        {
            status = STATUS_EXPECT_VAR_TYPE;
        }

        if (status == STATUS_EXPECT_SEMICOLON && ch == ';') // кінець оператора
        {
            status = STATUS_EXPECT_ID_LIST;
        }
    }

    readOffset = 0;

    printf("Рядок є оголошенням змінної\n------------------------------------------\n");

    return true;
}

void Test()
{
    ParseVariableDeclaration("var a: Int;");
    ParseVariableDeclaration("var a, b: Int;");
    ParseVariableDeclaration("var a: Real; b: Int; c: Char;");
    ParseVariableDeclaration("var a: Real; b: Int; c: Char;   asd  "); // помилка
    ParseVariableDeclaration("var var: Int;"); // помилка якщо для ідентифікатора використано зарезервоване слово
    ParseVariableDeclaration("var Int: Int;"); // помилка якщо для ідентифікатора використано зарезервоване слово 2
    ParseVariableDeclaration("var qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq: Int;"); // помилка якщо довжина ідентифікатора більше ніж 32 байти
    ParseVariableDeclaration("var a: UnknownType;"); // помилка якщо для змінної використано невідомий тип даних
}

int main()
{
    setlocale(LC_CTYPE, "ukr");

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
        
    Test();

    system("pause");

    return 0;
}