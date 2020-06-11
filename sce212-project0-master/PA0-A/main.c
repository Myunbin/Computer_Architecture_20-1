#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Point1 {
    int x;
    int y;
};

struct Point2 {
    int *x;
    int *y;
};

int is_large(int num1, int num2)
{
    return num1 > num2;
}


int sum_x(int x1, int x2)
{
    int sum = 0;
    /* Fill this function */
    um = x1 + x2;s
    return sum;
}

void sum_y(int y1, int y2, int *sum)
{
    *sum = y1 + y2;
    /* Fill this function */
}


void Point1_to_Point2(struct Point1 *P1, struct Point2 *P2)
{
    /* Fill this function */
    P2 -> x = &P1 -> x;
    P2 -> y = &P1 -> y;
}

void Point2_to_Point1(struct Point1 *P1, struct Point2 *P2)
{
    /* Fill this function */
    P1 -> x = *P2 -> x;
    P1 -> y = *P2 -> y;
}

int calc_area1(struct Point1 *P1, struct Point1 *P2)
{
    int area = 0;
    /* Fill this function */
    int x1 = P1 -> x, x2 = P2 -> x;
    int y1 = P1 -> y, y2 = P2 -> y;

    int width = (is_large(x1, x2) ? -1 : 1) * (x2 - x1);
    int height = (is_large(y1, y2) ? -1 : 1) * (y2 - y1);

    area = width * height;

    return area;
}

void calc_area2(struct Point2 *P1, struct Point2 *P2, int *area)
{
    /* Fill this function */
    int x1 = *P1 -> x, x2 = *P2 -> x;
    int y1 = *P1 -> y, y2 = *P2 -> y;

    int width = (is_large(x1, x2) ? -1 : 1) * (x2 - x1);
    int height = (is_large(y1, y2) ? -1 : 1) * (y2 - y1);

    *area = width * height;
}

char* reverse(char *word)
{
    /* Fill this function */
    char temp[32];
    int len = strlen(word);

    for (int i = 0; i < len; i++) temp[i] = word[len - 1 - i];
    for (int i = 0; i < len; i++)word[i] = temp[i];
    return word;
}

int main(int argc, char **argv)
{
    int sum = 0, area = 0;
    int x1, x2, y1, y2;
    char word[32];

    x1 = 1, y1 = 2, x2 = 0, y2 = 8;
    printf("x1: %d, y1: %d, x2: %d, y2: %d\n", x1, y1, x2, y2);

    struct Point1 P1 = { .x = x1, .y = y1 };
    struct Point2 P2 = { .x = &x2, .y = &y2 };
    struct Point2 P1_tmp = { .x = NULL, .y = NULL };
    struct Point1 P2_tmp = { .x = 0, .y = 0 };

    // P1. sum = x1 + x2
    sum = sum_x(x1, x2);
    printf("sum_x: %d\n", sum);

    // P2. sum = y1 + y2
    sum_y(y1, y2, &sum);
    printf("sum_y: %d\n", sum);

    // P3. Convert variable type from struct Point2 to struct Point1
    Point2_to_Point1(&P2_tmp, &P2);

    // P4. Calculate Area with struct Point1 type
    area = calc_area1(&P1, &P2_tmp);
    printf("calc_area1: %d\n", area);

    // P5. Convert variable type from struct Point1 to struct Point2
    Point1_to_Point2(&P1, &P1_tmp);

    // P6. Calculate Area with struct Point2 type
    calc_area2(&P1_tmp, &P2, &area);
    printf("calc_area2: %d\n", area);

    strcpy(word, "erutcetihcra retupmoc evol i");
    printf("The reverse is %s\n", reverse(word));

    return 0;
}
