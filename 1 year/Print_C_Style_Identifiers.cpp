// 8 & 10
// 8. У текстовому файлі PROG - "Сі-програма", з ідентифікаторами довжиною не більше 9. Надрукувати в алфавітному порядку всі ідентифікатори, вказавши кількість входжень.
// 10. Розв'язати попередню задачу, якщо максимальна довжина ідентифікаторів заздалегідь невідома.
#include <iostream>
struct Node
{
    int occurrenceCount;
    char* value;
    Node* left;
    Node* right;
};

int Compare(const char* a, const char* b)
{
    return strcmp(a, b);
}

Node* Insert(Node* root, char* value)
{
    if (!root)
    {
        root = new Node;
        root->value = value;
        root->occurrenceCount = 1;
        root->left = NULL;
        root->right = 0;

        return root;
    }

    int res = Compare(root->value, value);
    if (res < 0) // перший рядок більше ніж другий
    {
        root->right = Insert(root->right, value);
    }
    else if (res == 0) // якщо вже є такий же самий рядок, то оновлюємо кількість входжень
    {
        root->occurrenceCount++;
        return root;
    }
    else
    {
        root->left = Insert(root->left, value);
    }

    return root;
}

Node* Search(Node* root, char* str)
{
    if (!root)
    {
        return NULL;
    }

    int result = Compare(root->value, str);
    if (result == 0)
    {
        return root; 
    }
    else if (result < 0)
    {
        return Search(root->right, str);
    }
    else if (result > 0)
    {
        return Search(root->left, str);
    }
}

char reservedWords[32][10] = {"auto", "break", "case", "char", "const", "continue", "default",
"do", "double", "else", "enum", "extern", "float", "for", "goto",
"if", "int", "long", "register", "return", "short", "signed",
"sizeof", "static", "struct", "switch", "typedef", "union",
"unsigned", "void", "volatile", "while"};

char preprocessorDirectives[11][10] = {"include", "define", "undef", "ifdef", "ifndef", "if", "else",
"elif", "endif", "error", "pragma"};

// чи є рядок зарезервованим словом
bool IsReserved(char* buf)
{
    for (int i = 0; i < 32; i++)
    {
        if (!strcmp(buf, reservedWords[i]))
        {
            return true;
        }
    }

    return false;
}
 
bool IsPreprocessorDirective(char* buf)
{
    for (int i = 0; i < 11; i++)
    {
        if (!strcmp(buf, preprocessorDirectives[i]))
        {
            return true;
        }
    }

    return false;
}

int readOffset = 0;
int GetChar(FILE* file)
{
    int ch = fgetc(file);

    readOffset++;

    return ch;
}

// Достатньо використати правила задання ідентифікаторів та перевірити на зарезервовані слова
Node* ParseCode(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        return NULL;
    }

    Node* root = NULL;
    const int chunkSize = 16;
    char chunk[chunkSize];
    char* str = NULL;
    int strLen = 0;
    int i = 0;
    bool directive = false;
    bool doubleQuotes = false;

    int ch;
    while ((ch = GetChar(file)) != EOF)
    {
        if ((isalnum(ch) || ch == '_') && !(i == 0 && isdigit(ch)) && !doubleQuotes)
        {
            // копіюємо дані з тимчасового буферу до рядка, щоб не вийти за межі тимчасового буферу 
            if (i >= chunkSize)
            {
                str = (char*)realloc(str, sizeof(char) * i);
                memcpy(str + strLen, chunk, i);
                strLen += i;
                i = 0;
            }

            // записуємо символ до тимчасового буферу
            chunk[i] = ch;
            i++;
        }
        else if (i != 0) // кінець ідентифікатора
        {
            str = (char*)realloc(str, sizeof(char) * (i + strLen));
            memcpy(str + strLen, chunk, i);
            strLen += i;
            str[strLen] = '\0';

            if (!IsReserved(str) && !directive && !IsPreprocessorDirective(str))
            {
                printf("%s\n", str);

                root = Insert(root, str);
            }

            i = 0;
            str = NULL;
            strLen = 0;
            directive = false;
        }
        else if (ch == '"')
        {
            doubleQuotes = !doubleQuotes;
        }
        else if (ch == '#')
        {
            directive = true;
        }
    }

    fclose(file);

    return root;
}

// симетричний обхід
void PrintIdentifiers(Node* root)
{
    if (!root)
    {
        return;
    }

    PrintIdentifiers(root->left);
    printf("%s, %d\n", root->value, root->occurrenceCount);
    PrintIdentifiers(root->right);
}

int main()
{
    Node* root = ParseCode("PROG.cpp");
    PrintIdentifiers(root);
    
    Node* res = Search(root, "var2");
    if (res)
    {
        printf("%s\n", res->value);
    }

    return 0;
}