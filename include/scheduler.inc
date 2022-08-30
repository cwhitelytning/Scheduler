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
 * Returns the number of days until the specified month.
 * @param   int end   up to which month to sum the days?
 * @param   int leap  it's a leap year?
 * @return  int
 */
stock get_count_days_until_month(const end = MONTHS_IN_YEAR, const leap = false)
{
  new count = 0;
  for(new index = 1; index <= end; ++index) {
    count += leap ? DAYS_ON_MONTHS_IN_LEAP_YEAR[index] : DAYS_ON_MONTHS[index];
  }
  return count;
}

/**
 * Returns the number of days elapsed until year.
 * @param   int year  until what year do you count?
 * @return  int
 */
stock get_days_elapsed_until_year(year)
{
  new days = 0;
  while(year-- > INITIAL_DATE_YEAR) {
    days += is_leap_year(year) ? DAYS_IN_LEAP_YEAR : DAYS_IN_YEAR;
  }
  return days;
}

/**
 * Returns the duration of the hours.
 * @param   int hours
 * @return  int
 */
stock get_hours_duration(const hours)
{
  return hours * SECONDS_IN_HOUR;
}

/**
 * Returns the duration of the minutes.
 * @param   int minutes
 * @return  int
 */
stock get_minutes_duration(const minutes)
{
  return minutes * SECONDS_IN_MINUTE;
}

/**
 * Converts hours, minutes, seconds to duration.
 * @param   int hours
 * @param   int minutes
 * @param   int seconds
 * @return  int
 */
stock get_time_duration(const hours, const minutes, const seconds)
{
  return get_hours_duration(hours) + get_minutes_duration(minutes) + seconds;
}

/**
 * Returns the duration of the year.
 * @param   int year
 * @return  int
 */
stock get_year_duration(const year)
{
  return get_days_elapsed_until_year(year) * SECONDS_IN_DAY;
}

/**
 * Returns the duration of the month.
 * @param   int year
 * @param   int month
 * @return  int
 */
stock get_month_duration(const year, const month)
{
  return get_count_days_until_month(month, is_leap_year(year)) * SECONDS_IN_DAY;
}

/**
 * Returns the duration of the days.
 * @param   int day
 * @return  int
 */
stock get_day_duration(const day)
{
  return INITIAL_DATE_DAY < day ? day * SECONDS_IN_DAY : 0;
}

/**
 * Converts year, month, day to duration.
 * @param   int year
 * @param   int month
 * @param   int day
 * @return  int
 */
stock get_date_duration(const year, const month, const day)
{
  return get_year_duration(year) + get_month_duration(year, month - 1) + get_day_duration(day - 1);
}

/**
 * Converts year, month, day, hours, minutes, seconds into duration.
 * @param   int year
 * @param   int month
 * @param   int day
 * @param   int hour
 * @param   int minute
 * @param   int second
 * @return  int
 */
stock get_datetime_duration(const year, const month, const day, const hour, const minute, const second)
{
  return get_date_duration(year, month, day) + get_time_duration(hour, minute, second);
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
 * @param   string  string datetime
 * @param   string  format set of %Y, %m, %d, %H, %M, %S
 * @param   int     year
 * @param   int     month
 * @param   int     day
 * @param   int     hours
 * @param   int     minutes
 * @param   int     seconds
 * @return  int
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
  return get_datetime_duration(year, month, day, hours, minutes, seconds);
}

/**
 * Returns the duration of the date and time now in the format (with UTC).
 * @param  format string
 * @return int
 */
stock get_now_duration_ex(const format[])
{
  new string[31];
  get_time(format, string, charsmax(string)); /* with UTC */

  return parse_datetime(string, format);  
}

/**
 * Returns the duration of the date and time now.
 * An analogue of the get_systime function.
 * @return int
 */
stock get_now_duration()
{  
  return get_now_duration_ex("%Y/%m/%d %H:%M:%S");
}

/**
 * Returns UTC in duration.
 * @return int
 */
stock get_utc_duration()
{
  return get_now_duration() - get_systime(); // without UTC
}

/**
 * Converts duration to days, hours, minutes and seconds.
 * @param int seconds
 * @param int days
 * @param int hours
 * @param int minutes
 * @noreturn
 */
stock time_from_duration(&seconds, &days = 0, &hours = 0, &minutes = 0) 
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
 * Converts duration to day, hours, minutes and seconds.
 * @param int days
 * @param int year
 * @param int month
 */
stock date_from_duration(&days, &year = 0, &month = 0)
{
  year = INITIAL_DATE_YEAR;
  if (days >= DAYS_IN_YEAR) {
    new const passed = days / DAYS_IN_YEAR;
    year += passed;
    days -= get_days_elapsed_until_year(year);
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
stock datetime_from_duration(&seconds, &year = 0, &month = 0, &days = 0, &hours = 0, &minutes = 0)
{
  time_from_duration(seconds, days, hours, minutes);
  date_from_duration(days, year, month);
}

/**
 * Returns UTC in hours.
 * @return int
 */
stock get_utc()
{
  new hours = 0, duration = get_utc_duration();
  time_from_duration(duration, .hours = hours);
  return hours;
}

/**
 * Checks whether the time point is within the time.
 * @param   int begin
 * @param   int time
 * @param   int end
 * @return  bool
 */
stock within_time_duration(const begin, const time, const end)
{
  return begin > end ? (begin <= time || time < end) : (begin <= time && time < end);
}