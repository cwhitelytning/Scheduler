#include <time>

/**
 * Returns the numeric code of the first character from a string
 * @return integer
 */
stock ord(const string[])
{
    static chars[2]
    chars[0] = string[0]
    return isdigit(chars[0]) ? str_to_num(chars) : -1; 
}

/**
 * Converts time chunks (hours, minutes, seconds) to seconds
 * @param integer[] chunks
 * @return integer
 */
stock getSecondsTime(const chunks[]) 
{
  return chunks[0] * SECONDS_IN_HOUR + chunks[1] * SECONDS_IN_MINUTE + chunks[2]
}

/**
 * Explode time by separator
 * @param string string
 * @param char separator
 * @param integer shift
 * @param integer size
 * @return integer[]
 */
stock explodeTime(const string[], separator = ':', shift = 0, size = sizeof string)
{
  new chunks[3]

  for(new symbol = 0; symbol < size; ++symbol) {
    
    if (sizeof chunks > shift) {
      if (string[symbol] == separator) {
        shift++
      } else if (isdigit(string[symbol])) {
        chunks[shift] = chunks[shift] * 10 + ord(string[symbol])
      }
    } else
      break
  }

  return chunks
}

/**
 * Defines the time components used by the format and splits them into chunks
 * @param string string
 * @param string format
 * @param char separator
 * @param integer size
 */
stock splitTime(const string[], const format[], separator = ':', size = sizeof string)
{
  new shift = 0
  if (contain(format, "%H") == -1) shift++
  if (contain(format, "%M") == -1) shift++
  if (contain(format, "%S") == -1) shift++
  return explodeTime(string, separator, shift, size)
}

/**
 * Converts seconds to hours, minutes, and seconds and returns an array of time chunks
 * @param integer seconds
 * @return integer[]
 */
stock getTimeSeconds(seconds) 
{
  new chunks[3]
    
  chunks[0] = seconds / SECONDS_IN_HOUR
  chunks[2] = seconds - (chunks[0] * SECONDS_IN_HOUR)
  chunks[1] = chunks[2] / SECONDS_IN_MINUTE
  chunks[2] -= chunks[1] * SECONDS_IN_MINUTE 
  
  return chunks
}