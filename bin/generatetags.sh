#!/bin/sh

CURRENT_DATE=`date +%Y-%m-%d`
FILENAME_AND_PATH="tags.txt"

/bin/echo -e "List of Existing Tags\n" >> $FILENAME_AND_PATH
/bin/echo -e "If you need suggestions for tags, this is a good list for reference.\n" >> $FILENAME_AND_PATH
/bin/echo -e "Last updated $CURRENT_DATE\n" >> $FILENAME_AND_PATH

ls >> $FILENAME_AND_PATH

sed -i 's/.html//g' $FILENAME_AND_PATH

sed -i '/tags.txt/d' $FILENAME_AND_PATH

sed -i '/LICENSE/d' $FILENAME_AND_PATH
sed -i '/README.md/d' $FILENAME_AND_PATH
sed -i '/mhwkb_style.css/d' $FILENAME_AND_PATH
sed -i '/$FILENAME_AND_PATH/d' $FILENAME_AND_PATH

exit 0;
