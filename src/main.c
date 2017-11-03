/*
 * mhwkb_md2html.c
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

#include "main.h"

int
main (int argc, char **argv)
{
  /* FIXME: Check for existence of directories */
  if (argc != 3)
  {
    printf ("Usage: %s <md_file_dir> <output-dir>\n", argv[0]);
    exit (EXIT_INVALID_ARGS);
  }

  char output_dir[PATH_MAX + 1];

  if (realpath (argv[2], output_dir) == NULL)
  {
    perror ("realpath: failure");
    exit (EXIT_FAILURE);
  }

  strcat (output_dir, "/");
  /* FIXME: display this and ask for confirmation */
  printf ("  html files will be output to: %s\n\n", output_dir);

  struct dirent *entry;
  DIR *files;
  files = opendir (argv[1]);
  if (files == NULL)
  {
    perror ("opendir");
    exit (EXIT_OPENDIR_FAILURE);
  }

  /* Used for pagination: Not yet implemented
   */
  struct p page_calc[MAX_ARTICLES];

  /* used later to stop a while loop in make_tags_real()
   */
  page_calc[0].tag[0] = '\0';

  char *articles[MAX_ARTICLES];
  int article_length = 0;

  chdir (argv[1]);
  while ((entry = readdir (files)) != NULL)
  {
    /* Only read .md files */
    if (strstr (entry->d_name, ".md") == NULL)
      continue;

    FILE *md_file = fopen (entry->d_name, "r");

    if (md_file == NULL)
    {
      perror ("failure: open file\n");
      printf ("%s\n", entry->d_name);
      exit (1);
    }

    char link_href[LINK_MAX_LEN + 1];
    char link_title[256];
    char md_line[LINE_MAX_LEN + 1];

    char tags[TAG_MAX_NUM][TAG_MAX_LEN];

    int i, href_pos;

    /* Pagination:
     * First pass through the while loop only count the number of links,
     * and get the number of links that will appear in each <tag.html> file
     *
     * Not yet implemented
     */
    int pass;
    for (pass = 1; pass <= PASSES; pass++)
    {
      while (fgets (md_line, LINE_MAX_LEN, md_file) != NULL)
      {
        link_href[0] = '\0';
        link_title[0] = '\0';

        /* check to see if there's a link on that line (GitHub markdown) */
        if (md_line[0] == '[')
        {
          for (i = 0; md_line[i] != ']'; i++)
          {
            if (md_line[i] != '[')
            {
              link_title[i - 1] = md_line[i];
            }
          }

          link_title[i - 1] = '\0';

          i++;

          href_pos = 0;

          for (; md_line[i] != ')'; i++)
          {
            if (md_line[i] != '(')
              link_href[href_pos++] = md_line[i];
          }
          link_href[href_pos++] = '\0';

          /* get the tags */
          fgets (md_line, LINE_MAX_LEN, md_file);

          int tag_ctr;
          tag_ctr = parse_tags_line (md_line, tags);

          char *article_links = malloc(TAGS_COMBINED_MAX_LEN + 1);
          memset(article_links, 0, TAGS_COMBINED_MAX_LEN + 1);

          /* Strip any extra chars from the tags */
          article_links = make_tags_real (tag_ctr, tags, article_links, page_calc);

          // Render the article templates
          const char *keys[] = { "link", "title", "article_links" };
          const char *values[] = { link_href, link_title, article_links };
          char *article_template = render_template_file(TEMPLATE_ARTICLE_PATH, 3, keys, values);
          free(article_links);

          // Error if we are about to over flow MAX_ARTICLES
          if(article_length >= MAX_ARTICLES) {
            perror ("failure: over MAX_ARTICLES");
            exit(1);
          }

          articles[article_length++] = article_template;

          create_tag_html_files (tag_ctr, output_dir, tags, link_href, link_title);

        }
      }
    }

    if (fclose (md_file) != 0)
    {
      perror ("failure: close file\n");
      exit (1);
    }
  }
  free(files);

  // Read the directory for all articles that are not index
  // Overwrite with the contents inserted into the index template
  // for the header and footer
  DIR *article_files = opendir (output_dir);
  if (article_files == NULL)
  {
    perror ("opendir");
    exit (EXIT_OPENDIR_FAILURE);
  }

  chdir (output_dir);
  while ((entry = readdir (article_files)) != NULL)
  {
    // Get the file contents to insert inside the template and rewrite
    if (strstr (entry->d_name, ".html") == NULL)
      continue;

    int title_len = strlen(entry->d_name) - 5;
    char title_tag[title_len + 1];
    memset(title_tag, 0, title_len + 1);
    strncpy(title_tag, entry->d_name, title_len);

    char fullfilename[strlen(output_dir) + 1 + strlen(entry->d_name) + 1];
    memset(fullfilename, 0, strlen(output_dir) + 1 + strlen(entry->d_name) + 1);
    sprintf (strchr (fullfilename, '\0'), "%s%s", output_dir, entry->d_name);

#if VERBOSE == 1
  printf ("%s : %d\n", __func__, __LINE__);
  printf ("%s\n\n", fullfilename);
#endif

    char* tag_contents = read_file_contents(entry->d_name);
    if(tag_contents == NULL)
    {
      perror("failure: file not read");
      exit(1);
    }

    // Render the index templates
    const char *index_keys[] = { "title", "body" };
    const char *index_values[] = { title_tag, tag_contents };
    char *index_template = render_template_file (TEMPLATE_INDEX_PATH, 2, index_keys, index_values);
    free(tag_contents);

    FILE *fp = fopen (entry->d_name, "w+");
    if (fp == NULL)
    {
      perror ("failure: open file\n");
      printf ("%s\n", entry->d_name);
      exit (1);
    }

#if DEBUG == 1
  printf ("%s : %d\n", __func__, __LINE__);
  printf ("$index_template%s\n\n", index_template);
#endif

    fprintf (fp, "%s", index_template);
    free(index_template);

    if (fclose (fp) != 0)
    {
      perror ("failure: close file\n");
      exit (1);
    }
  }
  free(article_files);

  // Write the index file
  char index_html[256];
  sprintf (index_html, "%s/index.html", output_dir);

  char title_main[256];
  title_main[0] = '\0';
  if (exists (index_html) != 0)
  {
    strcpy (title_main, "Home");
    strcat (title_main, " - Mental Health and Wellness Knowledge Base");
  }

  FILE *fp_index = fopen (index_html, "a");
  if (fp_index == NULL)
  {
    perror ("failure: open file\n");
    printf ("%s\n", index_html);
    exit (1);
  }

  int all_content_length = 0;
  int i = 0;
  for(i = 0; i < article_length; ++i)
  {
    all_content_length += strlen(articles[i]);
  }
  char all_articles[all_content_length + 1];
  memset(all_articles, 0, all_content_length + 1);
  for(i = 0; i < article_length; ++i)
  {
    strcat(all_articles, articles[i]);
    free(articles[i]);
  }

  const char *index_keys[] = { "title", "body" };
  const char *index_values[] = { title_main, all_articles };
  char *index_template = render_template_file(TEMPLATE_INDEX_PATH, 2, index_keys, index_values);
  fprintf (fp_index, "%s", index_template);
  free(index_template);

  if (fclose (fp_index) != 0)
  {
    perror ("failure: close file\n");
    exit (1);
  }

  /* test the "tag database" (this will be removed)
  */
  /*
  int pos = 0;

  while (page_calc[pos].tag[0]!= '\0')
  {
    printf ("$tag : %s | $instances : %d\n", page_calc[pos].tag, page_calc[pos].instances);
    pos++;
  }
  */
  printf ("No fatal errors reported\n");
  return 0;
}
