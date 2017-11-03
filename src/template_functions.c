/*
 * template_functions.c
 * https://github.com/dafky2000/simplectemplate
 * Provides HTML template functionality for https://github.com/andy5995/mhwkb
 *
 * Copyright 2017 Daniel Kelly <myself@danielkelly.me>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "template_functions.h"

int exists(const char* filename) {
#ifdef linux
	struct stat st;
	return (lstat(filename, &st));
#endif

#ifdef _WIN32
	LPCTSTR name = filename;
	GET_FILEEX_INFO_LEVELS fInfoLevelId = GetFileExInfoStandard;
	WIN32_FILE_ATTRIBUTE_DATA InfoFile; 
	/* GetFileAttributesEx returns 0 on fail */
	return GetFileAttributesEx(name,fInfoLevelId,&InfoFile) != 0 ? 0 : -1;
#endif

	return -1;
}

char* read_file_contents(const char* filename) {
	if(exists(filename) != 0) return NULL;

	// Open ze file
	FILE* fp = fopen(filename, "r");
	if(fp == NULL) return NULL;

  // Get the content length
	fseek(fp, 0, SEEK_END);
	long length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// Get the contents
	char* contents = malloc(length+1);
	memset(contents, 0, length+1);

  fread(contents, 1, length, fp);

	if(fclose(fp) != 0) {
		perror("failure: close file\n");
		exit(1);
	}
  
  if(!contents) return contents;

  // Remove the newline from the end but preserve a valid pointer so we can tell the difference between an "error" (when NULL) and simply an empty file.
  if(length) {
    char* removednl = malloc(length);
    memset(removednl, 0, length);
    memcpy(removednl, contents, length-1);
    free(contents);
    return removednl;
  }

  return contents;
}

char* set_template_var(char* template, const char* key, const char* value) {
	int keylen = strlen("{{data.}}") + strlen(key) + 1;
	char* fullkey = malloc(keylen);
	memset(fullkey, 0, keylen);

	strcat(fullkey, "{{data.");
	strcat(fullkey, key);
	strcat(fullkey, "}}");
	char* ret = str_replace(template, fullkey, value);
	free(fullkey);
	return ret;
}

char* render_template(const char* template_data, int len, const char* keys[], const char* values[]) {
  int template_length = strlen(template_data) + 1;
  char* template = malloc(template_length);
  memset(template, 0, template_length);

  strcpy(template, template_data);

	if(template != NULL) {
		int x = 0;
		for(x = 0; x < len; ++x) {
			char* processed = set_template_var(template, keys[x], values[x]);
			free(template);
			template = processed;
		}
	}

  return template;
}

char* render_template_file(const char* filename, int len, const char* keys[], const char* values[]) {
  char* contents = read_file_contents(filename);
  if(!contents) return NULL;

  char* rendered = render_template(contents, len, keys, values);
  free(contents);
  
  return rendered;
}

// "Stolen" from https://stackoverflow.com/questions/779875/what-is-the-function-to-replace-string-in-c
// You must free the result if result is non-NULL.
char* str_replace(char* orig, const char* rep, const char* with) {
	char* result;  // the return string
	char* ins;     // the next insert point
	char* tmp;     // varies
	int len_rep;   // length of rep (the string to remove)
	int len_with;  // length of with (the string to replace rep with)
	int len_front; // distance between rep and end of last rep
	int count;     // number of replacements

	// sanity checks and initialization
	if(!orig || !rep) return NULL;

	len_rep = strlen(rep);
	if(len_rep == 0) return NULL; // empty rep causes infinite loop during count

	if(!with) with = "";
	len_with = strlen(with);

	// count the number of replacements needed
	ins = orig;     // the next insert point
	for(count = 0; (tmp = strstr(ins, rep)); ++count) {
		ins = tmp + len_rep;
	}

	tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

	if(!result) return NULL;

	// first time through the loop, all the variable are set correctly from here on,
	//    tmp points to the end of the result string
	//    ins points to the next occurrence of rep in orig
	//    orig points to the remainder of orig after "end of rep"
	while(count--) {
			ins = strstr(orig, rep);
			len_front = ins - orig;
			tmp = strncpy(tmp, orig, len_front) + len_front;
			tmp = strcpy(tmp, with) + len_with;
			orig += len_front + len_rep; // move to next "end of rep"
	}
	strcpy(tmp, orig);

	return result;
}
