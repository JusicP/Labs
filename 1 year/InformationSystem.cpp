// Складський облік війського майна військової частини X
#include <iostream>
#include <io.h>

#ifdef WIN32
#include <Windows.h>
#endif

#define DATABASE_FILENAME "MilitaryProperty_Database.dat"

enum MilitaryPropertyCategory
{
	CATEGORY_NONE = -1,
	CATEGORY_OTHER,
	CATEGORY_AMMUNITION,
	CATEGORY_WEAPON,
	CATEGORY_AIRCRAFT,
	CATEGORY_TANK,
	CATEGORY_ARMORED_VEHICLE,
	CATEGORY_BULLETPROOF_VEST,
	CATEGORY_MAXCOUNT,
};

struct Date
{
	int year;
	int month;
};

struct MilitaryPropertyEntry 
{
	unsigned int id; // унікальний ідентифікатор
	char name[96]; // повна назва
	MilitaryPropertyCategory category; // Боєприпаси, стрілецька зброя, літаки, танки, бронетехніка
	int count; // кількість
	Date productionDate; // дата виробництва
	Date arrivalDate; // дата постачання
	int partNumber; // заводський номер
};

MilitaryPropertyEntry defaultEntries [] = {
	{1, "АК-74", CATEGORY_WEAPON, 6534, Date {1974, 6}, Date{1991, 6}, 66564},
	{2, "АК-47", CATEGORY_WEAPON, 2356, Date {1954, 1}, Date{1960, 5}, 2324},
	{3, "Бронежилети 5 классу", CATEGORY_BULLETPROOF_VEST, 1000, Date {2004, 12}, Date{2005, 1}, 22223},
	{4, "Т-64", CATEGORY_TANK, 554, Date {1968, 3}, Date{1969, 6}, 100001},
	{5, "Т-80", CATEGORY_TANK, 999, Date {1985, 6}, Date{1985, 8}, 200002},
	{6, "БМП-1", CATEGORY_ARMORED_VEHICLE, 3054, Date {1975, 1}, Date{1977, 5}, 5009},
	{7, "АН-12", CATEGORY_AIRCRAFT, 10, Date {1964, 8}, Date{1964, 9}, 101},
	{8, "152-мм снаряд", CATEGORY_AMMUNITION, 30000, Date {1992, 4}, Date{1992, 4}, 4433}
};

bool WriteDefaultEntries();
bool AddEntry(MilitaryPropertyEntry& entry);
bool RemoveEntryByID(int _id);
bool ModifyEntry(const MilitaryPropertyEntry& entry);
bool FindEntryByID(int idToFind, MilitaryPropertyEntry& entry);
void PrintEntryByID(int id);
void PrintAllEntries(int category = CATEGORY_NONE);
void PrintReport();

// запис бази даних за умовчуванням (для прикладу), якщо бази не існує
bool WriteDefaultEntries()
{
	FILE* f = fopen(DATABASE_FILENAME, "r");
	if (f)
	{
		// файл вже існує
		fclose(f);
		return false;
	}

	f = fopen(DATABASE_FILENAME, "wb+");
	if (!f)
	{
		printf("WriteDefaultEntries(): не вдалося відкрити файл\n");
		return false;
	}

	int nextID = sizeof(defaultEntries) / sizeof(MilitaryPropertyEntry) + 1;
	fwrite(&nextID, sizeof(nextID), 1, f);
	size_t retValue = fwrite(&defaultEntries, sizeof(MilitaryPropertyEntry), sizeof(defaultEntries) / sizeof(MilitaryPropertyEntry), f);
	if (retValue != (sizeof(defaultEntries) / sizeof(MilitaryPropertyEntry)))
	{
		printf("WriteDefaultEntries(): не вдалося записати базу даних\n");
		fclose(f);
		return false;
	}

	fclose(f);

	return true;
}

// додавання нових записів, запис їх до файлу
bool AddEntry(MilitaryPropertyEntry& entry)
{
	FILE* f = fopen(DATABASE_FILENAME, "rb+");
	if (!f)
	{
		printf("AddEntry(): не вдалося відкрити файл\n");
		return false;
	}

	int nextID;
	fread(&nextID, sizeof(nextID), 1, f);
	entry.id = nextID++;

	// переходимо на початок файлу
	rewind(f);

	// оновлюємо інформацію про наступний ідентифікатор
	fwrite(&nextID, sizeof(nextID), 1, f);

	// переходимо до кінця файлу
	fseek(f, 0, SEEK_END);

	// додаємо новий запис
	fwrite(&entry, sizeof(MilitaryPropertyEntry), 1, f);

	fclose(f);

	return true;
}

// Читаємо в буфер всі записи, окрім того, який нам потрібно видалити та записуємо у файл
bool RemoveEntryByID(int _id)
{
	FILE* f = fopen(DATABASE_FILENAME, "rb+");
	if (!f)
	{
		printf("RemoveEntryByID(): не вдалося відкрити файл\n");
		return false;
	}
	
	fseek(f, 0, SEEK_END);

	long newSize = ftell(f) - sizeof(MilitaryPropertyEntry);
	if (newSize <= sizeof(int))
	{
		printf("RemoveEntryByID(): файл пошкоджений\n");
		fclose(f);
		return false;
	}

	rewind(f);

	char* buf = new char[newSize];
	fread(buf, sizeof(int), 1, f);

	int id;
	int offset = sizeof(id);
	while (fread(&id, sizeof(id), 1, f))
	{
		if (id != _id)
		{
			fseek(f, -sizeof(id), SEEK_CUR);
			fread(buf + offset, sizeof(MilitaryPropertyEntry), 1, f);

			offset += sizeof(MilitaryPropertyEntry);
		}
		else
		{
			// переходимо до наступного ідентифікатора
			fseek(f, sizeof(MilitaryPropertyEntry) - sizeof(id), SEEK_CUR);
		}
	}

	rewind(f);

	// зменшуємо розмір файлу
	_chsize_s(_fileno(f), newSize);

	fwrite(buf, newSize, 1, f);

	fclose(f);

	delete[] buf;

	return true;
}

bool ModifyEntry(const MilitaryPropertyEntry& entry)
{
	FILE* f = fopen(DATABASE_FILENAME, "rb+");
	if (!f)
	{
		printf("ModifyEntry(): не вдалося відкрити файл\n");
		return NULL;
	}

	fseek(f, 4, SEEK_SET);

	int id;
	while (fread(&id, sizeof(id), 1, f))
	{
		if (id == entry.id)
		{
			// знайшли запис
			fseek(f, -4, SEEK_CUR);
			fwrite(&entry, sizeof(MilitaryPropertyEntry), 1, f);

			fclose(f);

			return true;
		}
		else
		{
			// переходимо до наступного ідентифікатора
			fseek(f, sizeof(MilitaryPropertyEntry) - sizeof(id), SEEK_CUR);
		}
	}

	fclose(f);

	return false;
}

// пошук запису за ідентифікатором
bool FindEntryByID(int idToFind, MilitaryPropertyEntry& entry)
{
	FILE* f = fopen(DATABASE_FILENAME, "rb");
	if (!f)
	{
		printf("FindEntryByID(): не вдалося відкрити файл\n");
		return false;
	}

	fseek(f, 4, SEEK_SET);

	int id;
	while (fread(&id, sizeof(id), 1, f))
	{
		if (id == idToFind)
		{
			// знайшли запис
			fseek(f, -4, SEEK_CUR);
			fread(&entry, sizeof(MilitaryPropertyEntry), 1, f);

			fclose(f);

			return true;
		}
		else
		{
			// переходимо до наступного ідентифікатора
			fseek(f, sizeof(MilitaryPropertyEntry) - sizeof(id), SEEK_CUR);
		}
	}

	fclose(f);

	return false;
}

int FormatDate(char* buf, Date date)
{
	return sprintf(buf, "%d/%d", date.year, date.month);
}

void PrintAllEntries(int _category)
{
	FILE* f = fopen(DATABASE_FILENAME, "rb");
	if (!f)
	{
		printf("PrintAllEntries(): не вдалося відкрити файл\n");
		return;
	}

	// пропускаємо поле nextID
	fseek(f, 4, SEEK_SET);

	printf("Список майна:\n" \
		"%-10s| %-20s| %-5s| %-5s| %-5s| %-5s| %-5s\n", "Ідентифікатор", "Назва", "Категорія", "Кількість", "Дата виробництва", "Дата постачання", "Заводський номер");

	MilitaryPropertyEntry entry;
	while (fread(&entry, sizeof(MilitaryPropertyEntry), 1, f))
	{
		if (_category != CATEGORY_NONE && _category != entry.category)
		{
			continue;
		}

		char prodDate[16];
		char arrDate[16];
		FormatDate(prodDate, entry.productionDate);
		FormatDate(arrDate, entry.arrivalDate);

		printf("%-13d| %-20s| %-9d| %-9d| %-16s| %-15s| %-5d\n", entry.id, entry.name, entry.category, entry.count, prodDate, arrDate, entry.partNumber);
	}

	fclose(f);
}

void PrintEntryByID(int id)
{
	MilitaryPropertyEntry entry;
	if (!FindEntryByID(id, entry))
	{
		printf("Запис не знайдено\n");
		return;
	}

	char prodDate[16];
	char arrDate[16];
	FormatDate(prodDate, entry.productionDate);
	FormatDate(arrDate, entry.arrivalDate);

	printf("Список майна:\n" \
		"%-10s| %-20s| %-5s| %-5s| %-5s| %-5s| %-5s\n", "Ідентифікатор", "Назва", "Категорія", "Кількість", "Дата виробництва", "Дата постачання", "Заводський номер");

	printf("%-13d| %-20s| %-9d| %-9d| %-16s| %-15s| %-5d\n", entry.id, entry.name, entry.category, entry.count, prodDate, arrDate, entry.partNumber);
}

const char* CategoryToStr(int category)
{
	switch (category)
	{
	case CATEGORY_OTHER:
		return "Різне";
	case CATEGORY_AMMUNITION:
		return "Боєприпаси";
	case CATEGORY_WEAPON:
		return "Стрілецька зброя";
	case CATEGORY_AIRCRAFT:
		return "Літак";
	case CATEGORY_TANK:
		return "Танк";
	case CATEGORY_ARMORED_VEHICLE:
		return "Бронетехніка";
	case CATEGORY_BULLETPROOF_VEST:
		return "Бронежилет";
	default:
		return "Невідома";
	}
}

struct Report
{
	int entriesNum;
	int totalUnitNum;
};

void PrintReport()
{
	FILE* f = fopen(DATABASE_FILENAME, "rb");
	if (!f)
	{
		printf("PrintReport(): не вдалося відкрити файл\n");
		return;
	}

	int nextID;
	fread(&nextID, sizeof(nextID), 1, f);

	Report report[CATEGORY_MAXCOUNT] = {};
	MilitaryPropertyEntry entry;
	while (fread(&entry, sizeof(MilitaryPropertyEntry), 1, f))
	{
		report[entry.category].entriesNum++;
		report[entry.category].totalUnitNum += entry.count;
	}

	printf("Загальна кількість записів: %d, кількість записів за категоріями:\n", nextID);

	for (int i = 0; i < CATEGORY_MAXCOUNT; i++)
	{
		printf("%s: %d, %d шт.\n", CategoryToStr(i), report[i].entriesNum, report[i].totalUnitNum);
	}
}

bool ProcessCommand(int command)
{
	int id;
	switch (command)
	{
	case 1:
		PrintAllEntries();
		break;
	case 2:
		printf("Введіть ідентифікатор запису: ");
		scanf("%d", &id);
		PrintEntryByID(id);
		break;
	case 3:
	{
		printf("Введіть категорію: ");
		scanf("%d", &id);

		PrintAllEntries(id);
		break;
	}
	case 4:
	{
		MilitaryPropertyEntry newEntry;

		printf("Введіть ім'я: ");
		scanf("%s", newEntry.name);

		printf("Введіть категорію: ");
		scanf("%d", &newEntry.category);

		printf("Введіть кількість: ");
		scanf("%d", &newEntry.count);

		printf("Введіть рік дати виробництва: ");
		scanf("%d", &newEntry.productionDate.year);

		printf("Введіть місяць дати виробництва: ");
		scanf("%d", &newEntry.productionDate.month);

		printf("Введіть рік дати постачання: ");
		scanf("%d", &newEntry.arrivalDate.year);

		printf("Введіть місяць дати постачання: ");
		scanf("%d", &newEntry.arrivalDate.month);

		printf("Введіть заводський номер: ");
		scanf("%d", &newEntry.partNumber);

		AddEntry(newEntry);
		break;
	}
	case 5:
		printf("Введіть ідентифікатор запису: ");
		scanf("%d", &id);
		RemoveEntryByID(id);
		break;
	case 6:
	{
		int fieldNum;

		printf("Введіть ідентифікатор запису: ");
		scanf("%d", &id);

		MilitaryPropertyEntry entry;
		if (!FindEntryByID(id, entry))
		{
			printf("Запис не знайдено\n");
			return true;
		}

		printf("Поля:\n1. Ім'я\n2. Категорія\n3. Кількість\n4. Дата виробництва\n5. Дата постачання\n6. Заводський номер\nВведіть номер поля, яке ви бажаєте змінити: ");
		scanf("%d", &fieldNum);

		if (fieldNum == 1)
		{
			printf("Введіть ім'я: ");
			scanf("%s", entry.name);
		}
		else if (fieldNum == 2)
		{
			printf("Введіть категорію: ");
			scanf("%d", &entry.category);
		}
		else if (fieldNum == 3)
		{
			printf("Введіть кількість: ");
			scanf("%d", &entry.count);
		}
		else if (fieldNum == 4)
		{
			printf("Введіть рік: ");
			scanf("%d", &entry.productionDate.year);

			printf("Введіть місяць: ");
			scanf("%d", &entry.productionDate.month);
		}
		else if (fieldNum == 5)
		{
			printf("Введіть рік: ");
			scanf("%d", &entry.arrivalDate.year);

			printf("Введіть місяць: ");
			scanf("%d", &entry.arrivalDate.month);
		}
		else if (fieldNum == 6)
		{
			printf("Введіть заводський номер: ");
			scanf("%d", &entry.partNumber);
		}

		ModifyEntry(entry);
		break;
	}
	case 7:
		PrintReport();
		break;
	case 8:
		return false;
	}

	return true;
}

int main()
{
	setlocale(LC_ALL, "ukr");

#ifdef WIN32
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
#endif

	WriteDefaultEntries();

	printf("База даних обліку військового майна, закріпленого за військовою частиною #x\n");

	int command = 0;
	do
	{
		printf("\nОберіть дію:\n" \
			"1. Вивести на екран усі записи\n" \
			"2. Вивести на екран запис за ідентифікатором\n" \
			"3. Вивести на екран записи за категорією\n" \
			"4. Додати запис\n" \
			"5. Видалити запис\n" \
			"6. Редагувати запис\n" \
			"7. Вивести звіт\n" \
			"8. Вихід\n");

		scanf("%d", &command);
	} while (ProcessCommand(command));

	system("pause");

	return 0;
}