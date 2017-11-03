/*
 * mhwkb.h
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

#ifndef INC_RMW_H
#define INC_RMW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

#define EXIT_INVALID_ARGS 2
#define EXIT_OPENDIR_FAILURE 4

#define VERSION ".0.0.18"
#define DATE "2017-10-21"

#define DEBUG 0

#define TEMPLATE_INDEX_PATH "../templates/index.html"
#define TEMPLATE_ARTICLE_PATH "../../templates/article.html"
#define TEMPLATE_ARTLNK_PATH "../../templates/article_link.html"

#define MAX_ARTICLES 500
#define MAX_TAG_COUNT 500

/* max tags per article */
#define TAG_MAX_NUM 10

/* max string length of a tag */
#define TAG_MAX_LEN 80

/* after the tags has been concatenated and the html code is applied */
#define TAGS_COMBINED_MAX_LEN 1280

#define HTML_FILENAME_MAX_LEN 512

#define LINK_MAX_LEN 512

/* line length when reading the md file */
#define LINE_MAX_LEN 512

/* pagination not yet implemented */
struct p
{
  char tag[TAG_MAX_LEN];
  int instances;
};


#endif
