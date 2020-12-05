#include "date.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define MIN_DAYS 1
#define MAX_DAYS 30
#define MIN_MONTH 1
#define MAX_MONTH 12
#define DAYS_IN_YEAR 365

struct Date_t
{
    int day;
    int month;
    int year;
};

static bool checkIllegalDate(int day, int month, int year)
{
    if(day < MIN_DAYS || day > MAX_DAYS || month < MIN_MONTH || month > MAX_MONTH)
    {
        return true;
    }
    return false;
}

static int dateToDays(Date date)
{
    return (date->year * DAYS_IN_YEAR) + (date->month * MAX_DAYS) + date->day;
}

Date dateCreate(int day, int month, int year)
{
    if (checkIllegalDate(day, month, year))
    {
        return NULL;
    }
    Date date = malloc(sizeof(*date));
    if (!date)
    {
        return NULL;
    }
    date->day = day;
    date->month = month;
    date->year = year;

    return date;
}

void dateDestroy(Date date)
{
    free(date);
}

Date dateCopy(Date date)
{
    if (!date)
    {
        return NULL;
    }
    Date new_date = dateCreate(date->day, date->month, date->year);
    if (!new_date)
    {
        return NULL;
    }
    return new_date;
}

bool dateGet(Date date, int *day, int *month, int *year)
{
    if (!date || !day || !month || !year)
    {
        return NULL;
    }
    *day = date->day;
    *month = date->month;
    *year = date->year;
    return true;
}

int dateCompare(Date date1, Date date2)
{
    if (!date1 || !date2)
    {
        return 0;
    }
    return dateToDays(date1) - dateToDays(date2);
}

void dateTick(Date date)
{
    if (!date)
    {
        return;
    }
    date->day++;
    if (date->day > MAX_DAYS)
    {
        date->day = 1;
        date->month++;
        if (date->month > MAX_MONTH)
        {
            date->month = 1;
            date->year++;
        }
    }
}