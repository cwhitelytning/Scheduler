#include <time>

// string_stocks.inc

/**
 * Returns the numeric code of the first character from a string
 * @return integer
 */
stock char_to_digit(const string[])
{
  static chars[2];
  chars[0] = string[0];
  return isdigit(chars[0]) ? str_to_num(chars) : -1; 
}

// sheduler.inc

#if !defined HOURS_IN_DAY
#define HOURS_IN_DAY 24
#endif

#if !defined MONTHS_IN_YEAR
#define MONTHS_IN_YEAR 12
#endif

#if !defined DAYS_IN_LEAP_YEAR
#define DAYS_IN_LEAP_YEAR 366
#endif

#if !defined DAYS_IN_YEAR
#define DAYS_IN_YEAR 365
#endif

#if !defined INITIAL_DATE_YEAR
#define INITIAL_DATE_YEAR 1970
#endif

#if !defined INITIAL_DATE_MONTH
#define INITIAL_DATE_MONTH 1
#endif

#if !defined INITIAL_DATE_DAY
#define INITIAL_DATE_DAY 1
#endif

#define is_leap_year(%1) !(%1 % 4) || !(%1 % 100) || !(%1 % 400)
#define DATETIME_SIZE 7

new const DAYS_ON_MONTHS[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
new const DAYS_ON_MONTHS_IN_LEAP_YEAR[] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/**
 * Returns the day of the week from date.
 * @link    https/www.geeksforgeeks.org/find-day-of-the-week-for-a-given-date/
 * @param   int year
 * @param   int month
 * @param   int day
 * @return  int
 */
stock get_day_of_week_from_date(year, const month, const day)
{
  static const codes[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  
  if (month < 3) year--;
  return (year + year / 4 - year / 100 + year / 400 + codes[month - 1] + day) % 7;
}

/**
 * Returns the number of elapsed days until the specified month.
 * @param   month number in the year up to which you want to count the days (1 - 12 range)
 * @param   leap Is it a leap year? 
 * @return  days
 */
stock get_elapsed_days_until_month(const month = MONTHS_IN_YEAR, const leap = false)
{
  new count = 0;
  for(new index = 1; index <= month; ++index) {
    count += leap ? DAYS_ON_MONTHS_IN_LEAP_YEAR[index] : DAYS_ON_MONTHS[index];
  }
  return count;
}

/**
 * Returns the number of elapsed days until the specified year.
 * @param   year until which the number of elapsed days will be counted
 * @return  days
 */
stock get_elapsed_days_until_year(year)
{
  new days = 0;
  while(year-- > INITIAL_DATE_YEAR) {
    days += is_leap_year(year) ? DAYS_IN_LEAP_YEAR : DAYS_IN_YEAR;
  }
  return days;
}

/**
 * Returns the duration of the hours.
 * @param   hours
 * @return  duration
 */
stock get_duration_of_hours(const hours)
{
  return hours * SECONDS_IN_HOUR;
}

/**
 * Returns the duration of the minutes.
 * @param   minutes
 * @return  duration
 */
stock get_duration_of_minutes(const minutes)
{
  return minutes * SECONDS_IN_MINUTE;
}

/**
 * Converts hours, minutes, seconds to duration.
 * @param   hours
 * @param   minutes
 * @param   seconds
 * @return  duration
 */
stock get_duration_of_time(const hours, const minutes, const seconds)
{
  return get_duration_of_hours(hours) + get_duration_of_minutes(minutes) + seconds;
}

/**
 * Returns the duration of the year.
 * @param   year
 * @return  duration
 */
stock get_duration_of_elapsed_days_until_year(const year)
{
  return get_elapsed_days_until_year(year) * SECONDS_IN_DAY;
}

/**
 * Returns the duration of the month.
 * @param   year
 * @param   month
 * @return  duration
 */
stock get_duration_of_elapsed_days_until_month(const year, const month)
{
  return get_elapsed_days_until_month(month, is_leap_year(year)) * SECONDS_IN_DAY;
}

/**
 * Returns the duration of the days.
 * @param   days
 * @return  duration
 */
stock get_duration_of_days(const days)
{
  return INITIAL_DATE_DAY < days ? days * SECONDS_IN_DAY : 0;
}

/**
 * Converts year, month, day to duration.
 * @param   year
 * @param   month
 * @param   day
 * @return  duration
 */
stock get_duration_of_date(const year, const month, const day)
{
  return get_duration_of_elapsed_days_until_year(year) + get_duration_of_elapsed_days_until_month(year, month - 1) + get_duration_of_days(day - 1);
}

/**
 * Converts year, month, day, hours, minutes, seconds into duration.
 * @param   year
 * @param   month
 * @param   day
 * @param   hour
 * @param   minute
 * @param   second
 * @return  duration
 */
stock get_duration_of_datetime(const year, const month, const day, const hour, const minute, const second)
{
  return get_duration_of_date(year, month, day) + get_duration_of_time(hour, minute, second);
}

/**
 * Parses the date and time as a string.
 * @param output string
 * @param string datetime
 * @param length maximum length 6
 */
stock parse_datetime_string(output[], const string[], const length)
{
  new const string_size = strlen(string);
  for(new index = 0, offset = 0; index < string_size && offset < length; ++index) {
   if (isdigit(string[index])) {
      output[offset] = output[offset] * 10 + char_to_digit(string[index]);
    } else
      offset++;
  }
}

/**
 * Parses the date and time format into an array.
 * @param output dual array
 * @param format string
 */
stock parse_datetime_format(output[][], const format[])
{
  new const size = strlen(format);
  for (new index = 0, offset = 0; index < size; ++index) {
    if (format[index] == '%') copy(output[offset++], 2, format[index++]);
  }
}

/**
 * Parses the date and time by format and returns the duration.
 * @param   string
 * @param   format set of %Y, %m, %d, %H, %M, %S
 * @param   year
 * @param   month
 * @param   day
 * @param   hours
 * @param   minutes
 * @param   seconds
 * @return  duration
 */
stock parse_datetime(const string[], const format[], &year = 0, &month = 0, &day = 0, &hours = 0, &minutes = 0, &seconds = 0)
{
  new datetime[DATETIME_SIZE];
  parse_datetime_string(datetime, string, charsmax(datetime));

  new parsed[DATETIME_SIZE][3];
  parse_datetime_format(parsed, format);

  for(new index = 0; index < DATETIME_SIZE; ++index) {
    if (equal(parsed[index], "%Y")) {
      year = datetime[index];
    } else if (equal(parsed[index], "%m")) {
      month = datetime[index];
    } else if (equal(parsed[index], "%d")) {
      day = datetime[index];
    } else if (equal(parsed[index], "%H")) {
      hours = datetime[index];
    } else if (equal(parsed[index], "%M")) {
      minutes = datetime[index];
    } else if (equal(parsed[index], "%S")) {
      seconds = datetime[index];
    }
  }
  return get_duration_of_datetime(year, month, day, hours, minutes, seconds);
}

/**
 * Returns the duration of the date and time now in the format (with UTC).
 * @param  format
 * @return duration
 */
stock get_duration_of_now(const format[])
{
  new string[31];
  get_time(format, string, charsmax(string)); /* with UTC */

  return parse_datetime(string, format);  
}

/**
 * Returns the duration of the time now.
 * @return duration
 */
stock get_duration_of_time_now()
{
  return get_duration_of_now("%H:%M:%S");
}

/**
 * Returns the duration of the date and time now.
 * An analogue of the get_systime function.
 * @return duration
 */
stock get_duration_of_datetime_now()
{  
  return get_duration_of_now("%Y/%m/%d %H:%M:%S");
}

/**
 * Returns UTC in duration.
 * @return int
 */
stock get_duration_of_utc()
{
  return get_duration_of_datetime_now() - get_systime(); // without UTC
}

/**
 * Converts duration to days, hours, minutes and seconds.
 * @param seconds duration from which days, hours and minutes will be extracted
 * @param days
 * @param hours
 * @param minutes
 * @noreturn
 */
stock get_time_from_duration(&seconds, &days = 0, &hours = 0, &minutes = 0) 
{    
  if (seconds >= SECONDS_IN_MINUTE) {
    minutes = seconds / SECONDS_IN_MINUTE;
    seconds -= minutes * SECONDS_IN_MINUTE;
  }

  if (minutes >= SECONDS_IN_MINUTE) {
    hours = minutes / SECONDS_IN_MINUTE;
    minutes -= hours * SECONDS_IN_MINUTE;
  }

  if (hours >= HOURS_IN_DAY) {
    days = hours / HOURS_IN_DAY;
    hours -= days * HOURS_IN_DAY;
  }
}

/**
 * Converts duration to days, years and months.
 * @param days duration of the days from which the year and month will be extracted
 * @param year
 * @param month
 */
stock get_date_from_duration(&days, &year = 0, &month = 0)
{
  year = INITIAL_DATE_YEAR;
  if (days >= DAYS_IN_YEAR) {
    new const passed = days / DAYS_IN_YEAR;
    year += passed;
    days -= get_elapsed_days_until_year(year);
  }

  for(; month < MONTHS_IN_YEAR + 1; ++month) {
    new const count = is_leap_year(year) ? DAYS_ON_MONTHS_IN_LEAP_YEAR[month] : DAYS_ON_MONTHS[month];
    if (days > count) days -= count; else break;
  }
  
  days++;
}

/**
 * Converts duration to year, month, day, hours, minutes and seconds.
 * @param int seconds
 * @param int year
 * @param int month
 * @param int days
 * @param int hours
 * @param int minutes
 */
stock get_datetime_from_duration(&seconds, &year = 0, &month = 0, &days = 0, &hours = 0, &minutes = 0)
{
  get_time_from_duration(seconds, days, hours, minutes);
  get_date_from_duration(days, year, month);
}

/**
 * Returns UTC in hours.
 * @return hours
 */
stock get_utc()
{
  new hours = 0, duration = get_duration_of_utc();
  get_time_from_duration(duration, .hours = hours);
  return hours;
}

/**
 * Checks whether the time point is within the time.
 * @param   begin duration
 * @param   stamp duration
 * @param   end duration
 * @return  bool
 */
stock bool:is_in_time_range(const begin, const stamp, const end)
{
  return begin > end ? (begin <= stamp || stamp < end) : (begin <= stamp && stamp < end);
}

/**
 * Checks if the time is currently in the range.
 * @param   begin duration
 * @param   end duration
 * @return  bool
 */
stock bool:is_in_now_time_range(const begin, const end)
{
  return is_in_time_range(begin, get_duration_of_time_now(), end);
}