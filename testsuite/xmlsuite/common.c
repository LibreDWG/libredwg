//This file cotains the helper function which are used in the XML Suite
//to prepare the result

/*
 * This functions contverts 3D Point to the following format
 *
 *  Format: (x y z)
 *
 *  To emit this in the XML file
 *
 *  @param double x The X Co-ordinate
 *  @param double y The Y Co-ordinate
 *  @param double z The Z Co-ordinate
 *
 *  @return char* Converted string in the specified pattern
 */
char *
spointprepare (double x, double y, double z)
{
  char *result;

  // Allocating memory for the string
  result = malloc (100 * sizeof (char));
  int n = sprintf (result, "(%.4f %.4f %.4f)", x, y, z);

  //Check if it was transfered properly
  if (n < 0)
    {
      strcpy (result, ""); /* return empty string on error */
    }

  return result;
}

/*
 * This functions coverts double format in char to be emitted in the XML
 * @param double x The double digis to be converted in string
 *
 * @return char* Return the converted double in string. Return empty string if error
 */
char *
doubletochar (double x)
{
  char *result;
  int n;

  /* Allocate memory for the string */
  result = malloc (50 * sizeof (char));
  n = sprintf (result, "%.4f", x);

  if (n < 0)
    {
      strcpy (result, ""); /* Return empty string on error */
    }

  return result;
}

/*
 * This function converts 2D point in the following format so as to be used
 * in the XML
 *
 * Format: (x y)
 *
 * @param double x The x co-ordinate of the point
 * @param doubel y The y co-ordinate of the point
 *
 * @return char* Returns the string in the given format. Return empty string on error
 */
char *
spointprepare2 (double x, double y)
{
  char *result;

  /* allocate memory for the string */
  result = malloc (50 * sizeof (char));
  int n = sprintf (result, "(%.4f %.4f)", x, y);

  //Check if it was transfered properly
  if (n < 0)
    {
      strcpy (result, ""); /* return empty string on error */
    }

  return result;

}


/*
 * This function converts double in hex to be used in the XML
 * @param double handle The handle value which is to be converted in Hex
 *
 * @return char* The converted hex in string
 */
char *
doubletohex (double handle)
{
  char *result;

  /* allocate memory for the string */
  result = malloc (20 * sizeof (char));

  int n = sprintf (result, "%03x", handle);

  //Check if it was transfered properly

  if (n < 0)
    {
      strcpy (result, ""); /* return empty string on error */
    }

  return result;
}
