/*
 * gen_functions.c
 * Converts md files for the Mental Health and Wellness Knowledge Base
 * https://github.com/andy5995/mhwkb
 *
 * Copyright 2017 Andy Alt <andy400-dev@yahoo.com>
 * With contributions from
 *
 * Daniel Kelly <myself@danielkelly.me>
 *
 * and others mentioned in
 * https://github.com/andy5995/mhwkb/blob/master/CONTRIBUTING.md
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

#include "gen_functions.h"
#include "template_functions.h"

/**
 * Erases characters from the beginning of a string
 * (i.e. shifts the remaining string to the left
 */
void
del_char_shift_left (char *str, char c)
{
  int c_count = 0;

  /* count how many instances of 'c' */
  while (str[c_count] == c)
    c_count++;

  /* if no instances of 'c' were found... */
  if (!c_count)
    return;

  int len = strlen (str);
  int pos;

  for (pos = 0; pos < len - c_count; pos++)
    str[pos] = str[pos + c_count];

  str[len - c_count] = '\0';

  return;
}

/**
 * Trim a trailing character if present
 */
void
trim_char (char *str, char c)
{
  int len;
  len = strlen(str) - 1;

  if (str[len] != c)
    return;

  str[len] = '\0';

  return;
}

void buf_check (const char *str, const int len)
{
  if (strlen (str) >= len)
  {
    printf ("error: Buffer overflow caught\n");
    printf ("string length: %lu\n", strlen (str));

    int pos = 0;
    int chars_to_print = 0;
    chars_to_print = (len >= 80) ? 80 : 10;

    for (pos = 0; pos < chars_to_print; pos++)
      printf ("%c", str[pos]);

    printf ("\n");

    exit (1);
  }

  return;
}

/* tokenize the "tags" line */
int parse_tags_line (char *line, char tags[][TAG_MAX_LEN])
{
  char *tag_Ptr;
  tag_Ptr = strtok (line, ",");

  /* pointer to each tag. Used to check for a space in a tag and
   * replace to an underscore
   *
   * FIXME: It might be slightly better if this were done in make_tags_real()
   */
  char *tp;
  int local_tag_ctr = 0;

  while (tag_Ptr != NULL)
  {

    tp = &tag_Ptr[0];

    while (*tp != '\0')
    {
      if (*tp != ' ')
      {}
      else
        *tp = '_';

      tp++;
    }

    strcpy (tags[local_tag_ctr], tag_Ptr);

    tag_Ptr = strtok (NULL, ",");

    local_tag_ctr++;
  }

  return local_tag_ctr;
}
char* make_tags_real (const int tag_ctr, char tags[][TAG_MAX_LEN], char *article_links,
  struct p *page_calc)
{
  int tag;

  for (tag = 0; tag < tag_ctr; tag++)
  {
    char tag_html[HTML_FILENAME_MAX_LEN + 1];

    while (tags[tag][0] != '[' && tag == 0)
    {
      del_char_shift_left (tags[tag], tags[tag][0]);
    }

    del_char_shift_left (tags[tag], '[');

    /* if there's any white space between the [ and the " */

    while (tags[tag][0] != '"')
    {
      del_char_shift_left (tags[tag], tags[tag][0]);
    }

    del_char_shift_left (tags[tag], '"');

    /* check to see if we're on the last tag */
    if (tag < tag_ctr - 1)
    {
      trim_char (tags[tag], '"');
      sprintf (tag_html, "%s.html", tags[tag]);
    }
    else
    {
      int pos = 0;

      while (tags[tag][pos] != '"')
      {
        pos++;
      }

      /* if we're on the last tag, cut off the "]<br />"
       */
      tags[tag][pos] = '\0';
      sprintf (tag_html, "%s.html", tags[tag]);
    }

    const char *keys[] = { "link", "title" };

    buf_check (tags[tag], TAG_MAX_LEN);

    const char *values[] = { tag_html, tags[tag] };
    char *article_link = render_template_file(TEMPLATE_ARTLNK_PATH, 2, keys, values);

    buf_check (article_link, LINK_MAX_LEN);

    sprintf (strchr(article_links, '\0'), "%s\n", article_link);

    free(article_link);

    buf_check (article_links, TAGS_COMBINED_MAX_LEN);
  }

  /* The tags should be stripped of all extra chars now
   * pagination: Add to the tag database
   */
  for (tag = 0; tag < tag_ctr; tag++)
  {
    int pos = 0;
    int existing =  0;

    while (page_calc[pos].tag[0] != '\0' && pos < MAX_TAG_COUNT)
    {
      if (strcmp (page_calc[pos].tag, tags[tag]) == 0)
      {
        existing = 1;
        break;
      }
      pos++;
    }
    if (!existing)
    {
      page_calc[pos].instances = 1;

      /* Add a NULL terminator to the next position to
       * mark where the last tag is located. Used for the
       * while loop above.
       */
      page_calc[pos + 1].tag[0] = '\0';
    }
    else
    {
      page_calc[pos].instances++;
    }

    strcpy (page_calc[pos].tag, tags[tag]);
    existing = 0;
  }
  /*
   */

  return article_links;
}

void create_tag_html_files (const int tag_ctr, const char *output_dir,
  char tags[][TAG_MAX_LEN],
  const char *link_href, const char *link_title)
{
  /* Now that we know all the tags for one entry, make the <tag>.html
   * files
   */
  int tag;
  for (tag = 0; tag < tag_ctr; tag++)
  {
    char tag_html[HTML_FILENAME_MAX_LEN + 1];
    char html_tag_file[HTML_FILENAME_MAX_LEN + 1];
    sprintf (html_tag_file, "%s%s.html", output_dir, tags[tag]);

#if DEBUG == 1
  printf ("%s : %d\n", __func__, __LINE__);
  printf ("$html_tag_file:%s\n", html_tag_file);
#endif

    char tags_tag[TAGS_COMBINED_MAX_LEN + 1];
    memset(tags_tag, 0, TAGS_COMBINED_MAX_LEN + 1);
    int tag;
    for (tag = 0; tag < tag_ctr; tag++)
    {
      snprintf (tag_html, HTML_FILENAME_MAX_LEN, "%s.html", tags[tag]);

#if DEBUG == 1
  printf ("%s : %d\n", __func__, __LINE__);
  printf ("$tag_html:%s\n\n", tag_html);
#endif

      const char *link_keys[] = { "link", "title" };
      const char *link_values[] = { tag_html, tags[tag] };
      char *link_template = render_template_file(TEMPLATE_ARTLNK_PATH, 2, link_keys, link_values);

#if DEBUG == 1
  printf ("%s : %d\n", __func__, __LINE__);
  printf ("$link_template:%s\n\n", link_template);
#endif

      sprintf (strchr (tags_tag, '\0'), "%s\n", link_template);

#if DEBUG == 1
  printf ("%d\n", __LINE__);
  printf ("$tags_tag:%s\n\n", tags_tag);
#endif

      free(link_template);
    }

    // Render the article templates
    const char *article_keys[] = { "link", "title", "article_links" };
    const char *article_values[] = { link_href, link_title, tags_tag };
    char *article_template = render_template_file(TEMPLATE_ARTICLE_PATH, 3, article_keys, article_values);

#if DEBUG == 1
  printf ("%s : %d\n", __func__, __LINE__);
  printf ("$link_template:%s\n\n", article_template);
#endif

    // Save the file
    FILE *fp = fopen (html_tag_file, "a");
    if (fp == NULL)
    {
      perror ("failure: open file\n");
      printf ("%s\n", html_tag_file);
      exit (1);
    }

#if DEBUG == 1
  printf ("%s : %d\n", __func__, __LINE__);
  printf ("$article_template%s\n\n", article_template);
#endif

    fprintf (fp, "%s", article_template);
    free(article_template);

    if (fclose (fp) != 0)
    {
      perror ("failure: close file\n");
      exit (1);
    }
  }

  return;
}
