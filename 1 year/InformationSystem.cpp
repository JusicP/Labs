// ���������� ���� �������� ����� �������� ������� X
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
	unsigned int id; // ��������� �������������
	char name[96]; // ����� �����
	MilitaryPropertyCategory category; // ���������, ��������� �����, �����, �����, �����������
	int count; // �������
	Date productionDate; // ���� �����������
	Date arrivalDate; // ���� ����������
	int partNumber; // ���������� �����
};

MilitaryPropertyEntry defaultEntries [] = {
	{1, "��-74", CATEGORY_WEAPON, 6534, Date {1974, 6}, Date{1991, 6}, 66564},
	{2, "��-47", CATEGORY_WEAPON, 2356, Date {1954, 1}, Date{1960, 5}, 2324},
	{3, "����������� 5 ������", CATEGORY_BULLETPROOF_VEST, 1000, Date {2004, 12}, Date{2005, 1}, 22223},
	{4, "�-64", CATEGORY_TANK, 554, Date {1968, 3}, Date{1969, 6}, 100001},
	{5, "�-80", CATEGORY_TANK, 999, Date {1985, 6}, Date{1985, 8}, 200002},
	{6, "���-1", CATEGORY_ARMORED_VEHICLE, 3054, Date {1975, 1}, Date{1977, 5}, 5009},
	{7, "��-12", CATEGORY_AIRCRAFT, 10, Date {1964, 8}, Date{1964, 9}, 101},
	{8, "152-�� ������", CATEGORY_AMMUNITION, 30000, Date {1992, 4}, Date{1992, 4}, 4433}
};

bool WriteDefaultEntries();
bool AddEntry(MilitaryPropertyEntry& entry);
bool RemoveEntryByID(int _id);
bool ModifyEntry(const MilitaryPropertyEntry& entry);
bool FindEntryByID(int idToFind, MilitaryPropertyEntry& entry);
void PrintEntryByID(int id);
void PrintAllEntries(int category = CATEGORY_NONE);
void PrintReport();

// ����� ���� ����� �� ������������ (��� ��������), ���� ���� �� ����
bool WriteDefaultEntries()
{
	FILE* f = fopen(DATABASE_FILENAME, "r");
	if (f)
	{
		// ���� ��� ����
		fclose(f);
		return false;
	}

	f = fopen(DATABASE_FILENAME, "wb+");
	if (!f)
	{
		printf("WriteDefaultEntries(): �� ������� ������� ����\n");
		return false;
	}

	int nextID = sizeof(defaultEntries) / sizeof(MilitaryPropertyEntry) + 1;
	fwrite(&nextID, sizeof(nextID), 1, f);
	size_t retValue = fwrite(&defaultEntries, sizeof(MilitaryPropertyEntry), sizeof(defaultEntries) / sizeof(MilitaryPropertyEntry), f);
	if (retValue != (sizeof(defaultEntries) / sizeof(MilitaryPropertyEntry)))
	{
		printf("WriteDefaultEntries(): �� ������� �������� ���� �����\n");
		fclose(f);
		return false;
	}

	fclose(f);

	return true;
}

// ��������� ����� ������, ����� �� �� �����
bool AddEntry(MilitaryPropertyEntry& entry)
{
	FILE* f = fopen(DATABASE_FILENAME, "rb+");
	if (!f)
	{
		printf("AddEntry(): �� ������� ������� ����\n");
		return false;
	}

	int nextID;
	fread(&nextID, sizeof(nextID), 1, f);
	entry.id = nextID++;

	// ���������� �� ������� �����
	rewind(f);

	// ��������� ���������� ��� ��������� �������������
	fwrite(&nextID, sizeof(nextID), 1, f);

	// ���������� �� ���� �����
	fseek(f, 0, SEEK_END);

	// ������ ����� �����
	fwrite(&entry, sizeof(MilitaryPropertyEntry), 1, f);

	fclose(f);

	return true;
}

// ������ � ����� �� ������, ���� ����, ���� ��� ������� �������� �� �������� � ����
bool RemoveEntryByID(int _id)
{
	FILE* f = fopen(DATABASE_FILENAME, "rb+");
	if (!f)
	{
		printf("RemoveEntryByID(): �� ������� ������� ����\n");
		return false;
	}
	
	fseek(f, 0, SEEK_END);

	long newSize = ftell(f) - sizeof(MilitaryPropertyEntry);
	if (newSize <= sizeof(int))
	{
		printf("RemoveEntryByID(): ���� �����������\n");
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
			// ���������� �� ���������� ��������������
			fseek(f, sizeof(MilitaryPropertyEntry) - sizeof(id), SEEK_CUR);
		}
	}

	rewind(f);

	// �������� ����� �����
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
		printf("ModifyEntry(): �� ������� ������� ����\n");
		return NULL;
	}

	fseek(f, 4, SEEK_SET);

	int id;
	while (fread(&id, sizeof(id), 1, f))
	{
		if (id == entry.id)
		{
			// ������� �����
			fseek(f, -4, SEEK_CUR);
			fwrite(&entry, sizeof(MilitaryPropertyEntry), 1, f);

			fclose(f);

			return true;
		}
		else
		{
			// ���������� �� ���������� ��������������
			fseek(f, sizeof(MilitaryPropertyEntry) - sizeof(id), SEEK_CUR);
		}
	}

	fclose(f);

	return false;
}

// ����� ������ �� ���������������
bool FindEntryByID(int idToFind, MilitaryPropertyEntry& entry)
{
	FILE* f = fopen(DATABASE_FILENAME, "rb");
	if (!f)
	{
		printf("FindEntryByID(): �� ������� ������� ����\n");
		return false;
	}

	fseek(f, 4, SEEK_SET);

	int id;
	while (fread(&id, sizeof(id), 1, f))
	{
		if (id == idToFind)
		{
			// ������� �����
			fseek(f, -4, SEEK_CUR);
			fread(&entry, sizeof(MilitaryPropertyEntry), 1, f);

			fclose(f);

			return true;
		}
		else
		{
			// ���������� �� ���������� ��������������
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
		printf("PrintAllEntries(): �� ������� ������� ����\n");
		return;
	}

	// ���������� ���� nextID
	fseek(f, 4, SEEK_SET);

	printf("������ �����:\n" \
		"%-10s| %-20s| %-5s| %-5s| %-5s| %-5s| %-5s\n", "�������������", "�����", "��������", "ʳ������", "���� �����������", "���� ����������", "���������� �����");

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
		printf("����� �� ��������\n");
		return;
	}

	char prodDate[16];
	char arrDate[16];
	FormatDate(prodDate, entry.productionDate);
	FormatDate(arrDate, entry.arrivalDate);

	printf("������ �����:\n" \
		"%-10s| %-20s| %-5s| %-5s| %-5s| %-5s| %-5s\n", "�������������", "�����", "��������", "ʳ������", "���� �����������", "���� ����������", "���������� �����");

	printf("%-13d| %-20s| %-9d| %-9d| %-16s| %-15s| %-5d\n", entry.id, entry.name, entry.category, entry.count, prodDate, arrDate, entry.partNumber);
}

const char* CategoryToStr(int category)
{
	switch (category)
	{
	case CATEGORY_OTHER:
		return "г���";
	case CATEGORY_AMMUNITION:
		return "���������";
	case CATEGORY_WEAPON:
		return "��������� �����";
	case CATEGORY_AIRCRAFT:
		return "˳���";
	case CATEGORY_TANK:
		return "����";
	case CATEGORY_ARMORED_VEHICLE:
		return "�����������";
	case CATEGORY_BULLETPROOF_VEST:
		return "����������";
	default:
		return "�������";
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
		printf("PrintReport(): �� ������� ������� ����\n");
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

	printf("�������� ������� ������: %d, ������� ������ �� ����������:\n", nextID);

	for (int i = 0; i < CATEGORY_MAXCOUNT; i++)
	{
		printf("%s: %d, %d ��.\n", CategoryToStr(i), report[i].entriesNum, report[i].totalUnitNum);
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
		printf("������ ������������� ������: ");
		scanf("%d", &id);
		PrintEntryByID(id);
		break;
	case 3:
	{
		printf("������ ��������: ");
		scanf("%d", &id);

		PrintAllEntries(id);
		break;
	}
	case 4:
	{
		MilitaryPropertyEntry newEntry;

		printf("������ ��'�: ");
		scanf("%s", newEntry.name);

		printf("������ ��������: ");
		scanf("%d", &newEntry.category);

		printf("������ �������: ");
		scanf("%d", &newEntry.count);

		printf("������ �� ���� �����������: ");
		scanf("%d", &newEntry.productionDate.year);

		printf("������ ����� ���� �����������: ");
		scanf("%d", &newEntry.productionDate.month);

		printf("������ �� ���� ����������: ");
		scanf("%d", &newEntry.arrivalDate.year);

		printf("������ ����� ���� ����������: ");
		scanf("%d", &newEntry.arrivalDate.month);

		printf("������ ���������� �����: ");
		scanf("%d", &newEntry.partNumber);

		AddEntry(newEntry);
		break;
	}
	case 5:
		printf("������ ������������� ������: ");
		scanf("%d", &id);
		RemoveEntryByID(id);
		break;
	case 6:
	{
		int fieldNum;

		printf("������ ������������� ������: ");
		scanf("%d", &id);

		MilitaryPropertyEntry entry;
		if (!FindEntryByID(id, entry))
		{
			printf("����� �� ��������\n");
			return true;
		}

		printf("����:\n1. ��'�\n2. ��������\n3. ʳ������\n4. ���� �����������\n5. ���� ����������\n6. ���������� �����\n������ ����� ����, ��� �� ������ ������: ");
		scanf("%d", &fieldNum);

		if (fieldNum == 1)
		{
			printf("������ ��'�: ");
			scanf("%s", entry.name);
		}
		else if (fieldNum == 2)
		{
			printf("������ ��������: ");
			scanf("%d", &entry.category);
		}
		else if (fieldNum == 3)
		{
			printf("������ �������: ");
			scanf("%d", &entry.count);
		}
		else if (fieldNum == 4)
		{
			printf("������ ��: ");
			scanf("%d", &entry.productionDate.year);

			printf("������ �����: ");
			scanf("%d", &entry.productionDate.month);
		}
		else if (fieldNum == 5)
		{
			printf("������ ��: ");
			scanf("%d", &entry.arrivalDate.year);

			printf("������ �����: ");
			scanf("%d", &entry.arrivalDate.month);
		}
		else if (fieldNum == 6)
		{
			printf("������ ���������� �����: ");
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

	printf("���� ����� ����� ���������� �����, ����������� �� ��������� �������� #x\n");

	int command = 0;
	do
	{
		printf("\n������ ��:\n" \
			"1. ������� �� ����� �� ������\n" \
			"2. ������� �� ����� ����� �� ���������������\n" \
			"3. ������� �� ����� ������ �� ��������\n" \
			"4. ������ �����\n" \
			"5. �������� �����\n" \
			"6. ���������� �����\n" \
			"7. ������� ���\n" \
			"8. �����\n");

		scanf("%d", &command);
	} while (ProcessCommand(command));

	system("pause");

	return 0;
}