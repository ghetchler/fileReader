This program takes a input file and an output file.  If no output file is provided, the results are printed into the console. Can also take a file called a batch file that contains multiple files that need to be scanned  
Flags for program
  -B  runs all commands within the batch file
  -f  this is the input file
  -o  this is the output file
  -c  prints number of unique characters in the input file
  -w  prints the number of unique words in the input file
  -l  prints the number of unique lines in the input file
  -Lw prints the longest word in the input file
  -Ll prints the longest line in the input file

command input in form -f<input.txt> -o <output.txt> -c -w -l -Lw -Ll   or  -B <batchfile.txt>

example of batch file contents: -f welcome.txt -o output.txt -c -w -l -Lw -Ll
                                -f hollow.txt -o output.txt -c -w -l -Lw -Ll
                                -f welcome.txt -o output.txt -l -w -c
                                -f movies.txt -o output.txt -c
                                -f welcome.txt -o output.txt -Lw -Ll
