// Нормальні алгоритми маркова
#include <iostream>
#include <Windows.h>

struct Rule
{
	char pattern[128];
	char replace[128];
	bool terminate;
};

typedef struct Node
{
	Rule rule;
	Node *next;
} *pNode;

pNode CreateNode(char* pattern, char* replace, bool terminate)
{
	pNode node = new Node;
	strcpy(node->rule.pattern, pattern);
	strcpy(node->rule.replace, replace);
	node->rule.terminate = terminate;
	node->next = NULL;
	return node;
}

// зчитування правил з файлу та створення списку правил
pNode ReadRules(const char* fileName)
{
	FILE* file = fopen(fileName, "r");
	if (!file)
	{
		printf("Файла '%s' не існує\n");
		return NULL;
	}
	
	fseek(file, 0L, SEEK_END);
	int sz = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buf = new char[sz + 2];
	fread(buf, sz, 1, file);

	buf[sz] = '\n';
	buf[sz + 1] = '\0';

	pNode head = NULL, node = NULL;
	int i = 0, j = 0, k = 0, tmp = 0;
	for (i = j = 0; buf[i] != '\0'; i++)
	{
		// переходимо до кінця рядка
		if (buf[i] != '\n')
			continue;

		for (k = j + 1; k < i - 3; k++)
		{
			if (isspace(buf[k]) && !strncmp(buf + k + 1, "->", 2))
				break;
		}

		if (k >= i - 3)
		{
			break;
		}

		for (tmp = k; tmp > j && isspace(buf[--tmp]););
		if (tmp < j)
		{
			break;
		}
		buf[++tmp] = '\0';

		for (k += 3; k < i && isspace(buf[++k]););
		buf[i] = '\0';

		if (node)
		{
			node->next = CreateNode(buf + j, buf[k] == '.' ? buf + k + 1 : buf + k, buf[k] == '.');
			node = node->next;
		}
		else
		{
			node = CreateNode(buf + j, buf[k] == '.' ? buf + k + 1 : buf + k, buf[k] == '.');
		}

		if (!head)
		{
			head = node;
		}

		printf("rule: %s -> %s\n", buf + j, buf + k);

		j = i + 1;
	}

	return head;
}

void AppendStr(char*& str, char* s, int len)
{
	int l = strlen(str);
	if (len == -1)
		len = strlen(s);

	str = (char*)realloc(str, l + len + 1);

	memcpy(str + l, s, len);
	str[l + len] = '\0';
}

void Markov(pNode node, char* text)
{
	char* temp = (char*)calloc(1, 1);
	bool done = false;

	while (node && !done)
	{
		int patternLen = strlen(node->rule.pattern);
		int textLen = strlen(text);

		for (int i = 0; i < strlen(text); i++)
		{
			if (strncmp(text + i, node->rule.pattern, patternLen))
				continue;

			AppendStr(temp, text, i);
			AppendStr(temp, node->rule.replace, -1);
			AppendStr(temp, text + i + patternLen, -1);

			printf("%s\n", temp);

			strcpy(text, temp);
			temp[0] = '\0';

			if (node->rule.terminate)
				done = true;
		}

		node = node->next;
	}

	free(temp);
}

int main()
{
	setlocale(LC_ALL, "ukr");

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	printf("Нормальні алгоритми Маркова\n");

	pNode rules = ReadRules("rules.txt");
	if (!rules)
	{
		system("pause");
		return 0;
	}

	printf("Введіть текст: \n");

	char text[128];
	scanf("%128[^\n]", text);

	Markov(rules, text);

	system("pause");

	return 0;
}