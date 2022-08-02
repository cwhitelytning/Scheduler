#include <time>

// string_stocks.inc

/**
 * Returns the numeric code of the first character from a string
 * @return integer
 */
stock ord(const string[])
{
    static chars[2];
    chars[0] = string[0];
    return isdigit(chars[0]) ? str_to_num(chars) : -1; 
}

// scheduler.inc

/**
 * Converts time chunks (hours, minutes, seconds) to seconds
 * @param integer[] chunks
 * @return integer
 */
stock time_duration(const chunks[]) 
{
  return chunks[0] * SECONDS_IN_HOUR + chunks[1] * SECONDS_IN_MINUTE + chunks[2];
}

/**
 * Explode date or time
 * @param string string
 * @param integer shift
 * @param integer size
 * @return integer[]
 */
stock explode_datetime(const string[], shift = 0, size = sizeof string)
{
  new chunks[3];

  for(new symbol = 0; symbol < size; ++symbol) {
    if (sizeof chunks > shift) {
      if (isdigit(string[symbol])) {
        chunks[shift] = chunks[shift] * 10 + ord(string[symbol]);
      } else {
        shift++;
      }
    } else
      break;
  }

  return chunks;
}

/**
 * Defines the time components used by the format and splits them into chunks
 * @param string string
 * @param string format
 * @param integer size
 */
stock explode_time(const string[], const format[], size = sizeof string)
{
  new shift = 0;
  if (contain(format, "%H") == -1) {
    shift++;

    if (contain(format, "%M") == -1) shift++;
    if (contain(format, "%S") == -1) shift++;
  }
  return explode_datetime(string, shift, size);
}

/**
 * Converts seconds to hours, minutes, and seconds and returns an array of time chunks
 * @param integer seconds
 * @return integer[]
 */
stock time_from_duration(seconds) 
{
  new chunks[3];
    
  chunks[0] = seconds / SECONDS_IN_HOUR;
  chunks[2] = seconds - (chunks[0] * SECONDS_IN_HOUR);
  chunks[1] = chunks[2] / SECONDS_IN_MINUTE;
  chunks[2] -= chunks[1] * SECONDS_IN_MINUTE;
  
  return chunks;
}

/**
 * Checks whether the time point is within the time.
 * @param integer start
 * @param integer time
 * @param integer end
 * @return bool
 */
stock within_time_duration(start, time, end)
{
  return start > end ? start <= time || time < end : start <= time && time < end;
}