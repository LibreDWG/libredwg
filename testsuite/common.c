void
xml_header(FILE* file)
{
	fprintf(file, "<?xml version='1.0' encoding='UTF-8'?>\n<DwgData>\n");
}

void
xml_footer(FILE *file)
{
	fprintf(file, "</DwgData>\n" );
}

//Function to start the entity
void
entity_header(FILE* file)
{
	fprintf(file, "<DwgEntity ");
}


//This won't work right now as we can't store the output of printf
/*void
entity_attr(FILE *file, const char* key, const char* value)
{
	fprintf(file, "%s='%s' ", key, value);
}*/

void
entity_footer(FILE *file)
{
	fprintf(file,"/>\n");
}