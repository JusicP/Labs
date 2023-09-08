// 9. Задаються коло та лінія. Якщо вони перетинаються, то вивести точки перетину.
#include <iostream>
#include <locale>
#include <Windows.h>

using namespace std;

struct Point
{
    double x, y;
};

struct Circle
{
    Point center;
    double r;
};

struct Line
{
    Point p1;
    Point p2;
};

// Функція перевіряє, чи перетинає пряма коло, якщо так, то записує в p1, p2 точки перетину та повертає кількість точок перетину
int IsLineIntersectsCircle(Circle circle, Line line, Point& p1, Point& p2)
{
    double cx = line.p1.x - circle.center.x;
    double cy = line.p1.y - circle.center.y;
    double vx = line.p2.x - line.p1.x;
    double vy = line.p2.y - line.p1.y;
    double a = vx * vx + vy * vy;
    double b = 2 * (vx * cx + vy * cy);
    double c = cx * cx + cy * cy - circle.r * circle.r;
    double det = b * b - 4 * a * c;

    if (det < 0)
    {
        // немає рішень
        return 0;
    }
    else if (det == 0) // один корінь
    {
        // пряма є дотичною до кола
        double t = -b / (2 * a);
        p1.x = line.p1.x + t * vx;
        p1.y = line.p1.y + t * vy;

        return 1;
    }
    else // два корені
    {
        // пряма перетинає коло
        det = sqrt(det);
        double t1 = (-b - det) / (2 * a);
        double t2 = (-b + det) / (2 * a);

        p1.x = line.p1.x + t1 * vx;
        p1.y = line.p1.y + t1 * vy;
        p2.x = line.p1.x + t2 * vx;
        p2.y = line.p1.y + t2 * vy;

        return 2;
    }
}

int main()
{
    setlocale(LC_CTYPE, "ukr");

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    Circle circle;
    printf("Введіть координати центру кола та його радіус.\n");
    cin >> circle.center.x >> circle.center.y >> circle.r;

    Line line;
    printf("Введіть координати двох точок прямої.\n");
    cin >> line.p1.x >> line.p1.y >> line.p2.x >> line.p2.y;

    printf("Задано коло, центр: O(%f, %f), радіус: %f.\nЗадано пряму: M1(%f, %f), M2(%f, %f).\n", circle.center.x, circle.center.y, circle.r, line.p1.x, line.p1.y, line.p2.x, line.p2.y);

    Point p1, p2;
    int pointCount = IsLineIntersectsCircle(circle, line, p1, p2);
    if (pointCount > 0)
    {
        printf("Пряма перетинає коло у точці/точках:\nA(%f, %f)\n", p1.x, p1.y);

        if (pointCount == 2)
        {
            printf("B(%f, %f)\n", p2.x, p2.y);
        }
    }
    else
    {
        printf("Пряма не перетинає коло\n");
    }

    system("pause");

    return 0;
}