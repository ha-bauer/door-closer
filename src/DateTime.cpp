#include "DateTime.h"
#include "TimeSpan.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define memcpy_P(dest, src, count) memcpy(dest, src, count)
#define min(a, b) ((a) < (b) ? (a) : (b))

namespace DateTimeUnitTesting
{

    /**************************************************************************/
    /*!
    @brief  Convert a string containing two digits to uint8_t, e.g. "09" returns
   9
    @param p Pointer to a string containing two digits
*/
    /**************************************************************************/
    static uint8_t conv2d(const char *p)
    {
        uint8_t v = 0;
        if ('0' <= *p && *p <= '9')
            v = *p - '0';
        return 10 * v + *++p - '0';
    }

    /**
  Number of days in each month, from January to November. December is not
  needed. Omitting it avoids an incompatibility with Paul Stoffregen's Time
  library. C.f. https://github.com/adafruit/RTClib/issues/114
*/
    const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30};

    /**************************************************************************/
    /*!
    @brief  Given a date, return number of days since 2000/01/01,
            valid for 2000--2099
    @param y Year
    @param m Month
    @param d Day
    @return Number of days
*/
    /**************************************************************************/
    static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d)
    {
        if (y >= 2000U)
            y -= 2000U;
        uint16_t days = d;
        for (uint8_t i = 1; i < m; ++i)
            days += pgm_read_byte(daysInMonth + i - 1);
        if (m > 2 && y % 4 == 0)
            ++days;
        return days + 365 * y + (y + 3) / 4 - 1;
    }

    /**************************************************************************/
    /*!
    @brief  Constructor from
        [Unix time](https://en.wikipedia.org/wiki/Unix_time).
    This builds a DateTime from an integer specifying the number of seconds
    elapsed since the epoch: 1970-01-01 00:00:00. This number is analogous
    to Unix time, with two small differences:
     - The Unix epoch is specified to be at 00:00:00
       [UTC](https://en.wikipedia.org/wiki/Coordinated_Universal_Time),
       whereas this class has no notion of time zones. The epoch used in
       this class is then at 00:00:00 on whatever time zone the user chooses
       to use, ignoring changes in DST.
     - Unix time is conventionally represented with signed numbers, whereas
       this constructor takes an unsigned argument. Because of this, it does
       _not_ suffer from the
       [year 2038 problem](https://en.wikipedia.org/wiki/Year_2038_problem).
    If called without argument, it returns the earliest time representable
    by this class: 2000-01-01 00:00:00.
    @see The `unixtime()` method is the converse of this constructor.
    @param t Time elapsed in seconds since 1970-01-01 00:00:00.
*/
    /**************************************************************************/
    DateTime::DateTime(uint32_t t)
    {
        t -= SECONDS_FROM_1970_TO_2000; // bring to 2000 timestamp from 1970

        ss = t % 60;
        t /= 60;
        mm = t % 60;
        t /= 60;
        hh = t % 24;
        uint16_t days = t / 24;
        uint8_t leap;
        for (yOff = 0;; ++yOff)
        {
            leap = yOff % 4 == 0;
            if (days < 365U + leap)
                break;
            days -= 365 + leap;
        }
        for (m = 1; m < 12; ++m)
        {
            uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
            if (leap && m == 2)
                ++daysPerMonth;
            if (days < daysPerMonth)
                break;
            days -= daysPerMonth;
        }
        d = days + 1;
    }

    /**************************************************************************/
    /*!
    @brief  Constructor from (year, month, day, hour, minute, second).
    @warning If the provided parameters are not valid (e.g. 31 February),
           the constructed DateTime will be invalid.
    @see   The `isValid()` method can be used to test whether the
           constructed DateTime is valid.
    @param year Either the full year (range: 2000--2099) or the offset from
        year 2000 (range: 0--99).
    @param month Month number (1--12).
    @param day Day of the month (1--31).
    @param hour,min,sec Hour (0--23), minute (0--59) and second (0--59).
*/
    /**************************************************************************/
    DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour,
                       uint8_t min, uint8_t sec)
    {
        if (year >= 2000U)
           year -= 2000U;
        yOff = year;
        m = month;
        d = day;
        hh = hour;
        mm = min;
        ss = sec;
    }

    /**************************************************************************/
    /*!
    @brief  Copy constructor.
    @param copy DateTime to copy.
*/
    /**************************************************************************/
    DateTime::DateTime(const DateTime &copy)
        : yOff(copy.yOff), m(copy.m), d(copy.d), hh(copy.hh), mm(copy.mm),
          ss(copy.ss) {}

    /**************************************************************************/
    /*!
    @brief  Constructor for generating the build time.
    This constructor expects its parameters to be strings in the format
    generated by the compiler's preprocessor macros `__DATE__` and
    `__TIME__`. Usage:
    ```
    DateTime buildTime(__DATE__, __TIME__);
    ```
    @note The `F()` macro can be used to reduce the RAM footprint, see
        the next constructor.
    @param date Date string, e.g. "Apr 16 2020".
    @param time Time string, e.g. "18:34:56".
*/
    /**************************************************************************/
    DateTime::DateTime(const char *date, const char *time)
    {
        yOff = conv2d(date + 9);
        // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
        switch (date[0])
        {
        case 'J':
            m = (date[1] == 'a') ? 1 : ((date[2] == 'n') ? 6 : 7);
            break;
        case 'F':
            m = 2;
            break;
        case 'A':
            m = date[2] == 'r' ? 4 : 8;
            break;
        case 'M':
            m = date[2] == 'r' ? 3 : 5;
            break;
        case 'S':
            m = 9;
            break;
        case 'O':
            m = 10;
            break;
        case 'N':
            m = 11;
            break;
        case 'D':
            m = 12;
            break;
        }
        d = conv2d(date + 4);
        hh = conv2d(time);
        mm = conv2d(time + 3);
        ss = conv2d(time + 6);
    }

    /**************************************************************************/
    /*!
    @brief  Constructor for creating a DateTime from an ISO8601 date string.
    This constructor expects its parameters to be a string in the
    https://en.wikipedia.org/wiki/ISO_8601 format, e.g:
    "2020-06-25T15:29:37"
    Usage:
    ```
    DateTime dt("2020-06-25T15:29:37");
    ```
    @note The year must be > 2000, as only the yOff is considered.
    @param iso8601dateTime
           A dateTime string in iso8601 format,
           e.g. "2020-06-25T15:29:37".
*/
    /**************************************************************************/
    DateTime::DateTime(const char *iso8601dateTime)
    {
        char ref[] = "2000-01-01T00:00:00";
        memcpy(ref, iso8601dateTime, min(strlen(ref), strlen(iso8601dateTime)));
        yOff = conv2d(ref + 2);
        m = conv2d(ref + 5);
        d = conv2d(ref + 8);
        hh = conv2d(ref + 11);
        mm = conv2d(ref + 14);
        ss = conv2d(ref + 17);
    }

    /**************************************************************************/
    /*!
    @brief  Check whether this DateTime is valid.
    @return true if valid, false if not.
*/
    /**************************************************************************/
    bool DateTime::isValid() const
    {
        if (yOff >= 100)
            return false;
        DateTime other(unixtime());
        return yOff == other.yOff && m == other.m && d == other.d && hh == other.hh &&
               mm == other.mm && ss == other.ss;
    }

    /**************************************************************************/
    /*!
    @brief  Writes the DateTime as a string in a user-defined format.
    The _buffer_ parameter should be initialized by the caller with a string
    specifying the requested format. This format string may contain any of
    the following specifiers:
    | specifier | output                                                 |
    |-----------|--------------------------------------------------------|
    | YYYY      | the year as a 4-digit number (2000--2099)              |
    | YY        | the year as a 2-digit number (00--99)                  |
    | MM        | the month as a 2-digit number (01--12)                 |
    | MMM       | the abbreviated English month name ("Jan"--"Dec")      |
    | DD        | the day as a 2-digit number (01--31)                   |
    | DDD       | the abbreviated English day of the week ("Mon"--"Sun") |
    | AP        | either "AM" or "PM"                                    |
    | ap        | either "am" or "pm"                                    |
    | hh        | the hour as a 2-digit number (00--23 or 01--12)        |
    | mm        | the minute as a 2-digit number (00--59)                |
    | ss        | the second as a 2-digit number (00--59)                |
    If either "AP" or "ap" is used, the "hh" specifier uses 12-hour mode
    (range: 01--12). Otherwise it works in 24-hour mode (range: 00--23).
    The specifiers within _buffer_ will be overwritten with the appropriate
    values from the DateTime. Any characters not belonging to one of the
    above specifiers are left as-is.
    __Example__: The format "DDD, DD MMM YYYY hh:mm:ss" generates an output
    of the form "Thu, 16 Apr 2020 18:34:56.
    @see The `timestamp()` method provides similar functionnality, but it
        returns a `String` object and supports a limited choice of
        predefined formats.
    @param[in,out] buffer Array of `char` for holding the format description
        and the formatted DateTime. Before calling this method, the buffer
        should be initialized by the user with the format string. The method
        will overwrite the buffer with the formatted date and/or time.
    @return A pointer to the provided buffer. This is returned for
        convenience, in order to enable idioms such as
        `Serial.println(now.toString(buffer));`
*/
    /**************************************************************************/

    char *DateTime::toString(char *buffer)
    {
        uint8_t apTag =
            (strstr(buffer, "ap") != NULL) || (strstr(buffer, "AP") != NULL);
        uint8_t hourReformatted = 0, isPM = false;
        if (apTag)
        { // 12 Hour Mode
            if (hh == 0)
            { // midnight
                isPM = false;
                hourReformatted = 12;
            }
            else if (hh == 12)
            { // noon
                isPM = true;
                hourReformatted = 12;
            }
            else if (hh < 12)
            { // morning
                isPM = false;
                hourReformatted = hh;
            }
            else
            { // 1 o'clock or after
                isPM = true;
                hourReformatted = hh - 12;
            }
        }

        for (size_t i = 0; i < strlen(buffer) - 1; i++)
        {
            if (buffer[i] == 'h' && buffer[i + 1] == 'h')
            {
                if (!apTag)
                { // 24 Hour Mode
                    buffer[i] = '0' + hh / 10;
                    buffer[i + 1] = '0' + hh % 10;
                }
                else
                { // 12 Hour Mode
                    buffer[i] = '0' + hourReformatted / 10;
                    buffer[i + 1] = '0' + hourReformatted % 10;
                }
            }
            if (buffer[i] == 'm' && buffer[i + 1] == 'm')
            {
                buffer[i] = '0' + mm / 10;
                buffer[i + 1] = '0' + mm % 10;
            }
            if (buffer[i] == 's' && buffer[i + 1] == 's')
            {
                buffer[i] = '0' + ss / 10;
                buffer[i + 1] = '0' + ss % 10;
            }
            if (buffer[i] == 'D' && buffer[i + 1] == 'D' && buffer[i + 2] == 'D')
            {
                static const char day_names[] = "SunMonTueWedThuFriSat";
                const char *p = &day_names[3 * dayOfTheWeek()];
                buffer[i] = pgm_read_byte(p);
                buffer[i + 1] = pgm_read_byte(p + 1);
                buffer[i + 2] = pgm_read_byte(p + 2);
            }
            else if (buffer[i] == 'D' && buffer[i + 1] == 'D')
            {
                buffer[i] = '0' + d / 10;
                buffer[i + 1] = '0' + d % 10;
            }
            if (buffer[i] == 'M' && buffer[i + 1] == 'M' && buffer[i + 2] == 'M')
            {
                static const char month_names[] =
                    "JanFebMarAprMayJunJulAugSepOctNovDec";
                const char *p = &month_names[3 * (m - 1)];
                buffer[i] = pgm_read_byte(p);
                buffer[i + 1] = pgm_read_byte(p + 1);
                buffer[i + 2] = pgm_read_byte(p + 2);
            }
            else if (buffer[i] == 'M' && buffer[i + 1] == 'M')
            {
                buffer[i] = '0' + m / 10;
                buffer[i + 1] = '0' + m % 10;
            }
            if (buffer[i] == 'Y' && buffer[i + 1] == 'Y' && buffer[i + 2] == 'Y' &&
                buffer[i + 3] == 'Y')
            {
                buffer[i] = '2';
                buffer[i + 1] = '0';
                buffer[i + 2] = '0' + (yOff / 10) % 10;
                buffer[i + 3] = '0' + yOff % 10;
            }
            else if (buffer[i] == 'Y' && buffer[i + 1] == 'Y')
            {
                buffer[i] = '0' + (yOff / 10) % 10;
                buffer[i + 1] = '0' + yOff % 10;
            }
            if (buffer[i] == 'A' && buffer[i + 1] == 'P')
            {
                if (isPM)
                {
                    buffer[i] = 'P';
                    buffer[i + 1] = 'M';
                }
                else
                {
                    buffer[i] = 'A';
                    buffer[i + 1] = 'M';
                }
            }
            else if (buffer[i] == 'a' && buffer[i + 1] == 'p')
            {
                if (isPM)
                {
                    buffer[i] = 'p';
                    buffer[i + 1] = 'm';
                }
                else
                {
                    buffer[i] = 'a';
                    buffer[i + 1] = 'm';
                }
            }
        }
        return buffer;
    }

    /**************************************************************************/
    /*!
      @brief  Return the hour in 12-hour format.
      @return Hour (1--12).
*/
    /**************************************************************************/
    uint8_t DateTime::twelveHour() const
    {
        if (hh == 0 || hh == 12)
        { // midnight or noon
            return 12;
        }
        else if (hh > 12)
        { // 1 o'clock or later
            return hh - 12;
        }
        else
        { // morning
            return hh;
        }
    }

    /**************************************************************************/
    /*!
    @brief  Return the day of the week.
    @return Day of week as an integer from 0 (Sunday) to 6 (Saturday).
*/
    /**************************************************************************/
    uint8_t DateTime::dayOfTheWeek() const
    {
        uint16_t day = date2days(yOff, m, d);
        return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
    }

    /**************************************************************************/
    /*!
    @brief  Given a number of days, hours, minutes, and seconds, return the
   total seconds
    @param days Days
    @param h Hours
    @param m Minutes
    @param s Seconds
    @return Number of seconds total
*/
    /**************************************************************************/
    static uint32_t time2ulong(uint16_t days, uint8_t h, uint8_t m, uint8_t s)
    {
        return ((days * 24UL + h) * 60 + m) * 60 + s;
    }

    /**************************************************************************/
    /*!
    @brief  Return Unix time: seconds since 1 Jan 1970.
    @see The `DateTime::DateTime(uint32_t)` constructor is the converse of
        this method.
    @return Number of seconds since 1970-01-01 00:00:00.
*/
    /**************************************************************************/
    uint32_t DateTime::unixtime(void) const
    {
        uint32_t t;
        uint16_t days = date2days(yOff, m, d);
        t = time2ulong(days, hh, mm, ss);
        t += SECONDS_FROM_1970_TO_2000; // seconds from 1970 to 2000

        return t;
    }

    /**************************************************************************/
    /*!
    @brief  Convert the DateTime to seconds since 1 Jan 2000
    The result can be converted back to a DateTime with:
    ```cpp
    DateTime(SECONDS_FROM_1970_TO_2000 + value)
    ```
    @return Number of seconds since 2000-01-01 00:00:00.
*/
    /**************************************************************************/
    uint32_t DateTime::secondstime(void) const
    {
        uint32_t t;
        uint16_t days = date2days(yOff, m, d);
        t = time2ulong(days, hh, mm, ss);
        return t;
    }

    /**************************************************************************/
    /*!
    @brief  Add a TimeSpan to the DateTime object
    @param span TimeSpan object
    @return New DateTime object with span added to it.
*/
    /**************************************************************************/
    DateTime DateTime::operator+(const TimeSpan &span)
    {
        return DateTime(unixtime() + span.totalseconds());
    }

    /**************************************************************************/
    /*!
    @brief  Subtract a TimeSpan from the DateTime object
    @param span TimeSpan object
    @return New DateTime object with span subtracted from it.
*/
    /**************************************************************************/
    DateTime DateTime::operator-(const TimeSpan &span)
    {
        return DateTime(unixtime() - span.totalseconds());
    }

    /**************************************************************************/
    /*!
    @brief  Subtract one DateTime from another
    @note Since a TimeSpan cannot be negative, the subtracted DateTime
        should be less (earlier) than or equal to the one it is
        subtracted from.
    @param right The DateTime object to subtract from self (the left object)
    @return TimeSpan of the difference between DateTimes.
*/
    /**************************************************************************/
    TimeSpan DateTime::operator-(const DateTime &right)
    {
        return TimeSpan(unixtime() - right.unixtime());
    }

    /**************************************************************************/
    /*!
    @author Anton Rieutskyi
    @brief  Test if one DateTime is less (earlier) than another.
    @warning if one or both DateTime objects are invalid, returned value is
        meaningless
    @see use `isValid()` method to check if DateTime object is valid
    @param right Comparison DateTime object
    @return True if the left DateTime is earlier than the right one,
        false otherwise.
*/
    /**************************************************************************/
    bool DateTime::operator<(const DateTime &right) const
    {
        return (yOff + 2000U < right.year() ||
                (yOff + 2000U == right.year() &&
                 (m < right.month() ||
                  (m == right.month() &&
                   (d < right.day() ||
                    (d == right.day() &&
                     (hh < right.hour() ||
                      (hh == right.hour() &&
                       (mm < right.minute() ||
                        (mm == right.minute() && ss < right.second()))))))))));
    }

    /**************************************************************************/
    /*!
    @author Anton Rieutskyi
    @brief  Test if two DateTime objects are equal.
    @warning if one or both DateTime objects are invalid, returned value is
        meaningless
    @see use `isValid()` method to check if DateTime object is valid
    @param right Comparison DateTime object
    @return True if both DateTime objects are the same, false otherwise.
*/
    /**************************************************************************/
    bool DateTime::operator==(const DateTime &right) const
    {
        return (right.year() == yOff + 2000U && right.month() == m &&
                right.day() == d && right.hour() == hh && right.minute() == mm &&
                right.second() == ss);
    }
}