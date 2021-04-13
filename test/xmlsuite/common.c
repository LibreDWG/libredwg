// This file contains the helper function which are used in the XML Suite
// to prepare the result

xmlChar *spointprepare (double x, double y, double z);
xmlChar *doubletochar (double x);
xmlChar *inttochar (int x);
xmlChar *spointprepare2 (double x, double y);
xmlChar *doubletohex (double handle);

/*
 * This functions contverts 3D Point to the following format
 *
 *  Format: (x y z)
 *
 *  To emit this in the XML file
 *
 *  @param double x The X coordinate
 *  @param double y The Y coordinate
 *  @param double z The Z coordinate
 *
 *  @return xmlChar* Converted string in the specified pattern
 */
xmlChar *
spointprepare (double x, double y, double z)
{
  xmlChar *result;
  int n;

  // Allocating memory for the string
  result = MALLOC (100 * sizeof (xmlChar));
  n = sprintf ((char *)result, "(%.4f %.4f %.4f)", x, y, z);

  // Check if it was transferred properly
  if (n < 0)
    {
      strcpy ((char *)result, ""); /* return empty string on error */
    }

  return result;
}

/*
 * This functions converts double format in char to be emitted in the XML
 * @param double x The double digis to be converted in string
 *
 * @return char* Return the converted double in string. Return empty string if
 * error
 */
xmlChar *
doubletochar (double x)
{
  xmlChar *result;
  int n;

  /* Allocate memory for the string */
  result = MALLOC (50 * sizeof (xmlChar));
  n = sprintf ((char *)result, "%.4f", x);

  if (n < 0)
    {
      strcpy ((char *)result, ""); /* Return empty string on error */
    }

  return result;
}

/*
 * This functions converts int format in char to be emitted in the XML
 *
 * @return char* Return the converted double in string. Return empty string if
 * error
 */
xmlChar *
inttochar (int x)
{
  xmlChar *result;
  int n;

  /* Allocate memory for the string */
  result = MALLOC (16 * sizeof (xmlChar));
  n = sprintf ((char *)result, "%d", x);
  if (n < 0)
    {
      strcpy ((char *)result, ""); /* Return empty string on error */
    }

  return result;
}

/*
 * This function converts 2D point in the following format so as to be used
 * in the XML
 *
 * Format: (x y)
 *
 * @param double x The x coordinate of the point
 * @param double y The y coordinate of the point
 *
 * @return xmlChar* Returns the string in the given format. Return empty string
 * on error
 */
xmlChar *
spointprepare2 (double x, double y)
{
  xmlChar *result;
  int n;

  /* allocate memory for the string */
  result = MALLOC (50 * sizeof (xmlChar));
  n = sprintf ((char *)result, "(%.4f %.4f)", x, y);

  // Check if it was transferred properly
  if (n < 0)
    {
      strcpy ((char *)result, ""); /* return empty string on error */
    }

  return result;
}

/*
 * This function converts double in hex to be used in the XML
 * @param double handle The handle value which is to be converted in Hex
 *
 * @return char* The converted hex in string
 */
xmlChar *
doubletohex (double handle)
{
  xmlChar *result;
  int n;

  /* allocate memory for the string */
  result = MALLOC (20 * sizeof (xmlChar));
  n = sprintf ((char *)result, "%03X", (unsigned int)handle);

  // Check if it was transferred properly

  if (n < 0)
    {
      strcpy ((char *)result, ""); /* return empty string on error */
    }

  return result;
}
