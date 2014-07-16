char *
spointprepare(double x, double y, double z)
{
    char *result;

    result = malloc(100 * sizeof(char));
    int n = sprintf(result, "(%.4f %.4f %.4f)", x, y, z);

    //Check if it was transfered properly
    if (n < 0)
    {
        strcpy(result, "");
    }
    
    return result;
}

char*
doubletochar(double x)
{
  char *result;
  int n;

  result = malloc(50 * sizeof(char));
  n = sprintf(result, "%.4f", x);

  if (n < 0)
  {
    strcpy(result, "");
  }

  return result;
}
char*
spointprepare2(double x, double y)
{
    char *result;
    result = malloc(50 * sizeof(char));
    int n = sprintf(result, "(%.4f %.4f)", x, y);

    //Check if it was transfered properly
    if (n < 0)
    {
        strcpy(result, "");
    }

    return result;

}

char*
doubletohex (double handle)
{
  char *result;
  result = malloc(20 * sizeof(char));

  int n = sprintf(result, "%03x", handle)

  //Check if it was transfered properly

  if (n < 0)
  {
    strcpy(result, "");
  }

  return result;
}