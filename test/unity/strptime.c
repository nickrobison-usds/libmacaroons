#include <time.h>
#include <stdio.h>

char*
strptime(const char* buf, const char* fmt, struct tm* tm)
{

    int m = 0;
    int day = 0;
    int y = 0;
    int h = 0;
    int mm = 0;
    sscanf_s(buf, "%d-%d-%dT%d:%d", &y, &m, &day, &h, &mm);
    tm->tm_year = y - 1900;
    tm->tm_mon = m - 1;
    tm->tm_mday = day;
    tm->tm_hour = h;
    tm->tm_mon = mm;


    return "";
}