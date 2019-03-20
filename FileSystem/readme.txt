How to compile:

gcc *.c -o filesys  

How to run:
./filesys -f file_list.txt -d dir_list.txt -s 102400000 -b 512


File structure:
main.c         --   the main entry, parse the arguments
dtree.h/.c     --   implement the tree data structure
lfile.h/.c     --   implement the lfile list data structure
ldisk.h/.c     --   implement the ldisk list data structure
filesys.h/.c   --   implement the file system


