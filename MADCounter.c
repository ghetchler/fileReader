#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

typedef struct line{
  char * contents; // the line
  int numChars; // number of chars in the entire line
  int frequency; // num times this line appears in the file
  int initialPosition; // first location seen in file
  struct line * nextLine; // next line in file
  struct line * prevLine; // prev line in file
} LINE;
typedef struct word{
  char * contents; // the word 
  int numChars; // num of chars in word
  int frequency; // num times apperars in file
  int orderAppeared; // set at the first apperance of word
  struct word * nextWord; // next word in file
  struct word * prevWord; // prev word in file
} WORD;
int charList[128][3]; // list of all chars in file with desired info
//charList[x][0] = char, [x][1] = count, [x][2] = initial position 
int trackAscii = 0;
int trackWS = 0;
int trackNL = 0;
int trackLL = 0;
int trackLW = 0;
int outputFileFound = 0;
int numUniqueChars = 0; // track number of unique chars
int numUniqueWords = 0; // track number of unique words
int numUniqueLines = 0; // track number of unique lines
WORD * firstWord; // pointer to first word in file
LINE * firstLine; // pointer to first line in file
int totNumChars = 0; // total number of chars in the file
int totNumWords = 0; // total number of words in the file
int totNumLines = 0; // total number of lines in the file
FILE* fpOut; // Output file to put results into
FILE* fpIn; // Input file with desired text
void getInfoFromFile(FILE*);
void findFileInfo(char *);
void printIntoOutput(FILE*);
void printIntoConsole(FILE*);
/*
* creates and allocates a struct 
* init each member of the new word
* returns the newly created word
*/
WORD *newWord() {
	WORD *word;
	word = (WORD*)malloc(sizeof(WORD));
	if(word) {
		word->frequency = 0;
		word->numChars = 0;
		word->nextWord = (WORD*)malloc(sizeof(WORD)); 
		word->prevWord = (WORD*)malloc(sizeof(WORD));
		word->contents = (char*)malloc(sizeof(char) * 256);
		word->orderAppeared = 0;
	}
	return word; 
}
/*
* creates and allocates a struct
* initialize each member of the new line
* returns the newly created line
*/
LINE *newLine() {
	LINE *line;
	line = (LINE*)malloc(sizeof(LINE));
	if(line) {
		line->contents = (char*)malloc(sizeof(char) * 256);
		line->nextLine = (LINE*)malloc(sizeof(LINE));
		line->prevLine = (LINE*)malloc(sizeof(LINE));
		line->numChars = 0;
		line->frequency = 0;
		line->initialPosition = 0;
	}
	return line;
}
/*
*finds and sets the flags that are provided
*/
void findFileInfo(char *fileInfo) { 
	char * command = strtok(strdup(fileInfo), " "); // splits fileInfo up
	int inputFileFound = 0;
	while(command != NULL) {	
		if(command != NULL && strcmp( command, "-f") == 0) {
			command = strtok(NULL, " "); // if -f, next is input file
			if(command == NULL || strncmp(command, "-", 1) == 0) { // check to make sure valid input file
				printf("ERROR: No Input File Provided\n");
				exit(1);
            }
            fpIn = fopen(command, "r");
            if(fpIn == NULL) {
				//printf("command: %s\n", command);
				printf("ERROR: Can't open input file\n");
                exit(1);
				//printf("shouldn't hit");
            }
		inputFileFound = 1;
        command = strtok(NULL, " "); // sets command to next word
		}
        if(command != NULL && strcmp(command, "-o") == 0) { 
			command = strtok(NULL, " "); // if -o, next is input file
			if(command == NULL || strncmp(command, "-", 1) == 0) { // check to make sure valid output file
				printf("ERROR: No Output File Provided\n");
				exit(1);
			}
			fpOut = fopen(command, "w");
			outputFileFound = 1;
			command = strtok(NULL, " "); // sets command to next word
		}
	command = strtok(NULL, " ");
	}
	if(inputFileFound == 0) {
		printf("ERROR: No Input File Provided\n");
		exit(1);
	}
	command = strtok(strdup(fileInfo), " ");
	while(command != NULL) {
		if(strncmp(command, "-", 1) == 0) {
			if(strcmp(command, "-f") != 0 && strcmp(command, "-o") != 0 && strcmp(command, "-l") != 0 && strcmp(command, "-Ll") != 0 && strcmp(command, "-c") != 0 && strcmp(command, "-Lw") != 0 && strcmp(command, "-w") != 0) {
				printf("ERROR: Invalid Flag Types\n");
				exit(1);
			}
		}
		command = strtok(NULL, " ");
	}	
	getInfoFromFile(fpIn); // gets all info from input file
	command = strtok(strdup(fileInfo), " "); // reset command to whole fileInfo string
	while(command != NULL) { // loop to check for all desired flags
		if(strncmp(command, "-", 1) == 0) {
			if(strcmp(command, "-f") == 0 || strcmp(command, "-o") == 0) {
				command = strtok(NULL, " ");
			}
			if(strcmp(command, "-c") == 0) { 
				trackAscii = 1;
				if(outputFileFound == 1) {
					printIntoOutput(fpOut);
			} else {
				printIntoConsole(fpOut);	
			}
			trackAscii = 0; // now that it's printed, no need to have it flagged
			command = strtok(NULL, " ");
			if(command != NULL) { // check to see if another flag after
				if(strncmp(command, "-", 1) == 0) {
						if(strcmp(command, "-f") != 0 && strcmp(command, "-o") != 0 ) {
							if(outputFileFound == 1) {
								fprintf(fpOut, "\n");
							} else {printf("\n");}
						}
					}
				}
			} else if(strcmp(command, "-w") == 0) { // track whitespace
				trackWS = 1;
		    if(outputFileFound == 1) {
				printIntoOutput(fpOut);
	        } else {printIntoConsole(fpOut);}
				trackWS = 0;
				command = strtok(NULL, " ");
            if(command != NULL) { // check to see if another flag after
                if(strncmp(command, "-", 1) == 0) {
                       if(strcmp(command, "-f") != 0 && strcmp(command, "-o") != 0 ) {
                            if(outputFileFound == 1) {
                                fprintf(fpOut, "\n");
                            } else {printf("\n");}
                        }
                    }
                }
			} else if(strcmp(command, "-l") == 0) { // track newline
			    trackNL = 1;
		    if(outputFileFound == 1) {
				printIntoOutput(fpOut);
	        } else {printIntoConsole(fpOut);}
				trackNL = 0;
			command = strtok(NULL, " ");
            if(command != NULL) {
                if(strncmp(command, "-", 1) == 0) {
                        if(strcmp(command, "-f") != 0 && strcmp(command, "-o") != 0 ) {
                            if(outputFileFound == 1) {
                                fprintf(fpOut, "\n");
                            } else {printf("\n");}
                        }
                    }
                }
			} else if(strcmp(command, "-Lw") == 0) { // track longest word
				trackLW = 1;
			if(outputFileFound == 1) {
				printIntoOutput(fpOut);
	        } else {printIntoConsole(fpOut);}
				trackLW = 0;
				command = strtok(NULL, " ");
            if(command != NULL) { // check to see if another flag after
                if(strncmp(command, "-", 1) == 0) {
                        if(strcmp(command, "-f") != 0 && strcmp(command, "-o") != 0 ) {
                            if(outputFileFound == 1) {
                                fprintf(fpOut, "\n");
                            } else {printf("\n");}
                        }
                    }
                }
			} else if(strcmp(command, "-Ll") == 0) { // track longest line
			    trackLL = 1;
		    if(outputFileFound == 1) {
				printIntoOutput(fpOut);
			} else {printIntoConsole(fpOut);}
				trackLL = 0;
				command = strtok(NULL, " ");
            if(command != NULL) {
                if(strncmp(command, "-", 1) == 0) {
                        if(strcmp(command, "-f") != 0 && strcmp(command, "-o") != 0 ) {
                            if(outputFileFound == 1) {
                                fprintf(fpOut, "\n");
                            } else {printf("\n");}
                        }
                    }
                }
			}
		} else {
			command = strtok(NULL, " "); // if none of the commands matched, need to mannually increment to next word}
		}
	}
}
void printIntoConsole(FILE* fpOut) {
	if(trackAscii == 1) {
        printf("Total Number of Chars = %i\nTotal Unique Chars = %i\n\n",totNumChars, numUniqueChars);
		for(int i = 0; i < 128; i++) {
            if(charList[i][1] != 0) {
				printf("Ascii Value: %i, Char: %c, Count: %i, Initial Position: %i\n", i, charList[i][0], charList[i][1], charList[i][2]);
			}	
		}
     } else if(trackWS == 1) {
        printf("Total Number of Words: %i\nTotal Unique Words: %i\n\n", totNumWords, numUniqueWords);
        WORD * temp = firstWord;
        while(temp != NULL) { // prints in order of lower ascii value
            printf("Word: %s, Freq: %d, Initial Position: %d\n", temp->contents, temp->frequency, temp->orderAppeared);
            temp = temp->nextWord;
        }
			
        //printf("\n");
     } else if(trackNL == 1) {
        printf("Total Number of Lines: %i\nTotal Unique Lines: %d\n\n", totNumLines, numUniqueLines);
        LINE *line = firstLine;
        while(line != NULL) { // this prints in order of lower ascii value
            printf("Line: %s, Freq: %i, Initial Position: %i\n", line->contents, line->frequency, line->initialPosition);
            line = line->nextLine;
        }
    } else if(trackLW == 1) {
        WORD * temp = firstWord; // temp first word
        WORD * longestWord = temp; // longest word in file, init to first word in file
        //printf("trackLW\n");
		while(temp->nextWord != NULL) { // loop to find word with longest length
				if(longestWord->numChars < temp->numChars) { // if temp is larger, make temp new longest word
                longestWord = temp;
            }
            temp = temp->nextWord;
        }
        printf("Longest Word is %ld characters long:\n", strlen(longestWord->contents));
        temp = firstWord;
        while(temp != NULL) { // checks for multiple words of same longest length
            if(strlen(longestWord->contents) == strlen(temp->contents)) {
                printf("\t%s\n", temp->contents);
            }
			temp = temp->nextWord;
        }
     } else if(trackLL == 1) {
        LINE * temp = firstLine; // temp first line
        LINE * longestLine = temp; // longest line in file, init to first lien in file
        while(temp != NULL) {
            if(longestLine->numChars < temp->numChars) {
                longestLine = temp;
            }
            temp = temp->nextLine;
		}
        printf("Longest Line is %ld characters long:\n", strlen(longestLine->contents));
        temp = firstLine;
        while(temp != NULL) {
            if(strlen(longestLine->contents) == strlen(temp->contents)) {
                printf("\t%s\n", temp->contents);
            }
        temp = temp->nextLine;
        }
    }
}
/*
*prints the message corresponding to the flags into fpOut 
*/
void printIntoOutput(FILE* fpOut) {
	if(trackAscii == 1) {
        fprintf(fpOut, "Total Number of Chars = %i\nTotal Unique Chars = %i\n\n",totNumChars, numUniqueChars);
        for(int i = 0; i < 128; i++) {
			if(charList[i][1] != 0) {
				fprintf(fpOut, "Ascii Value: %i, Char: %c, Count: %i, Initial Position: %i\n", i, charList[i][0], charList[i][1], charList[i][2]);
			}
		}
     } else if(trackWS == 1) {
        fprintf(fpOut, "Total Number of Words: %i\nTotal Unique Words: %i\n\n", totNumWords, numUniqueWords);
        WORD * temp = firstWord;
        while(temp != NULL) { // prints in order of lower ascii value
            fprintf(fpOut, "Word: %s, Freq: %d, Initial Position: %d\n", temp->contents, temp->frequency, temp->orderAppeared);
            temp = temp->nextWord;
        }
        //fprintf(fpOut, "\n");
     } else if(trackNL == 1) {
        fprintf(fpOut, "Total Number of Lines: %i\nTotal Unique Lines: %d\n\n", totNumLines, numUniqueLines);
		LINE *line = firstLine;
		while(line != NULL) { // this prints in order of lower ascii value
			fprintf(fpOut, "Line: %s, Freq: %i, Initial Position: %i\n", line->contents, line->frequency, line->initialPosition);
			line = line->nextLine;
		}
	} else if(trackLW == 1) {
		WORD * temp = firstWord; // temp first word
		WORD * longestWord = temp; // longest word in file, init to first word in file  
		while(temp != NULL) { // loop to find word with longest length
			if(longestWord->numChars < temp->numChars) { // if temp is larger, make temp new longest word
				longestWord = temp;
			}
			temp = temp->nextWord;
		}
		fprintf(fpOut, "Longest Word is %ld characters long:\n", strlen(longestWord->contents));
		temp = firstWord;
		while(temp != NULL) {	// checks for multiple words of same longest length
			if(strlen(longestWord->contents) == strlen(temp->contents)) {
				fprintf(fpOut, "\t%s\n", temp->contents); 
			}
			temp = temp->nextWord;
		}
	 } else if(trackLL == 1) {
		LINE * temp = firstLine; // temp first line
		LINE * longestLine = temp; // longest line in file, init to first lien in file
		while(temp != NULL) {
			if(strlen(longestLine->contents) < strlen(temp->contents)) {
				longestLine = temp;
			}
			temp = temp->nextLine;
		}
		fprintf(fpOut, "Longest Line is %ld characters long:\n", strlen(longestLine->contents));
		temp = firstLine;	
		while(temp != NULL) {
			if(strlen(longestLine->contents) == strlen(temp->contents)) {
				fprintf(fpOut, "\t%s\n", temp->contents);	
			}
			temp = temp->nextLine;
		}
		}
	}
	/*
	*gets the desired info (char, word, line counts) from the input file
	*/
	void getInfoFromFile(FILE* fpIn) {
		totNumWords = 0;
		numUniqueWords = 0;
		//numUniqueChars = 0;
		int enterLoop = 0;
		int loopCounter = 0; // track initial position of the word
		firstWord = (WORD*)malloc(sizeof(WORD));
		firstWord = NULL;
		char scannedWord[256];
		int dupFound = 0; // 0 if no duplicate is found, 1 if found 
		while(fscanf(fpIn, "%s", scannedWord) > 0) { // while not EOF of input file
			dupFound = 0;
			enterLoop = 1;
			if(firstWord == NULL) { //if NULL, first word scanned 
				WORD *word = newWord();
				word->contents = strdup(scannedWord); // copies string into word->contents
				word->prevWord = NULL;
				word->nextWord = NULL;
				word->frequency = 1;
				word->orderAppeared = 0;
				word->numChars = strlen(word->contents);
				numUniqueWords++;
				totNumWords++;
				firstWord = word; // sets first word equal to newly created word
			} else { // if not the first word, need to check for duplicates or make a new word
				WORD * temp = firstWord;
				while(temp != NULL) { //checking if word is duplicate
					if(strcmp(scannedWord, temp->contents) == 0) { // if they are the same don't need to make a new word, just update count
						temp->frequency += 1;
						dupFound = 1;
						totNumWords++;
						break;
					}
					temp = temp->nextWord;
				}
				if(dupFound == 0) { // if no dup found need to make new word
					WORD *word = newWord(); // initialize new word to be added
					word->contents = strdup(scannedWord);
					word->frequency = 1;
					word->orderAppeared = loopCounter;
					word->numChars = strlen(word->contents);
					word->prevWord = NULL;
					word->nextWord = NULL;
					numUniqueWords++;
					totNumWords++;
					WORD * current = firstWord; // temp Var
					if (strcmp(word->contents, firstWord->contents) < 0){ // need to add before firstWord
						word->nextWord = current;
						current->prevWord = word;
						firstWord = word;
					} else { // need to find where to add it into the sorted linked list
						while(current->nextWord != NULL) {// find end of list
							if(strcmp(word->contents, current->nextWord->contents) < 0) { // if word is larger than 0, word is larger and should be after current 
								word->nextWord = current->nextWord;
								current->nextWord->prevWord = word;
								word->prevWord = current;
								current->nextWord = word;
								break; // if we found a match, no need to go through rest of while loop
							}
							current = current->nextWord;
						}
						if(current->nextWord == NULL && strcmp(current->contents, word->contents) < 0) { // add to end
							current->nextWord = word;
							word->prevWord = current;
							word->nextWord = NULL;
						}	
					}
				}
			}
			loopCounter++; 
		}
		if(enterLoop == 0) {
			printf("ERROR: Input File Empty\n");
			exit(1);
		}
		fseek(fpIn, 0, SEEK_SET); // resets the file pointer to beginning of file
		totNumLines = 0;
		numUniqueLines = 0;
		loopCounter = 0; // restet loopCounter to 0
		dupFound = 0; // reset dupFound to 0
		firstLine = (LINE*)malloc(sizeof(LINE));
		firstLine = NULL;
		while(!feof(fpIn)){ // now get info for all lines in file
			char * scannedLine;
			dupFound = 0;
			scannedLine = (char*)malloc(sizeof(char) * 256);
			int lineSize;
			size_t buffer = 256;
			lineSize = getline(&scannedLine, &buffer, fpIn); // gets line from file
			if(strcmp(scannedLine, "") == 0) {
				break;
			}
			scannedLine[lineSize - 1] = '\0';
			//printf("%s\n", scannedLine);
			if(firstLine == NULL) {
				LINE *line = newLine();
				line->contents = strdup(scannedLine); 
				line->frequency = 1;
				line->numChars = lineSize - 1;
				line->prevLine = NULL;
				line->nextLine = NULL;
				line->initialPosition = 0;
				numUniqueLines++;
				totNumLines++;
				firstLine = line; // set firstLine equal to newly created line
				//printf("firstLine: %s\n", firstLine->contents);
			} else {
				LINE * temp = firstLine;
				//printf("temp: %s\n", temp->contents);
				while(temp->nextLine != NULL) { //checking if line is duplicate
					//printf("inside loop\n");
					if(strcmp(scannedLine, temp->contents) == 0) { // if they are the same don't need to make a new line, just update count
						//printf("in if\n");
						temp->frequency += 1;
						dupFound = 1;
						totNumLines++;
						//printf("dup found\n");
						break;
					}
					//printf("%s\n", temp->contents);
					//printf("%i\n",temp->nextLine->numChars);
					//printf("%s\n", temp->nextLine->contents);
					temp = temp->nextLine;
					//printf("set temp to nextLine\n");
				}
				//printf("temp: %s\n", temp->contents);
				if(dupFound == 0) { // if no dup found need to make new line
					LINE *line = newLine(); // initialize new line to be added
					line->contents = strdup(scannedLine);
					line->frequency = 1;
					line->initialPosition = loopCounter;
					line->numChars = lineSize - 1;
					line->prevLine = NULL;
					line->nextLine = NULL;
					numUniqueLines++;
					totNumLines++;
					LINE * current = firstLine; // temp Var
					
					if (strcmp(line->contents, firstLine->contents) < 0){ // need to add before firstLine
						line->nextLine = current;
						current->prevLine = line;
						firstLine = line;
					} else { // need to find where to add it into the sorted linked list
						//printf("before while loop\n");
						while(current != NULL) {// find end of list
							//printf("In while loop, current: %s\n", current->contents);
							if(current->nextLine == NULL) { // if nextLine is null, need to add after
                                line->prevLine = current;
                                current->nextLine = line;
                                line->nextLine = NULL;
                                break;
                            }
							if(strcmp(line->contents, current->nextLine->contents) < 0) { // if line is larger than 0, line is larger and should be after current
								line->nextLine = current->nextLine;
								current->nextLine->prevLine = line;
								line->prevLine = current;
								current->nextLine = line;
								break; // if we found a match, no need to go through rest of while loop
							}// if(current->nextLine == NULL) { // if nextLine is null, need to add after
							//	line->prevLine = current;
							//	current->nextLine = line;
							//	line->nextLine = NULL;
							//	break;
							//}
							current = current->nextLine;
						}
						if(current->nextLine == NULL && strcmp(current->contents, line->contents) < 0) { // add to end
							current->nextLine = line;
							line->prevLine = current;
							line->nextLine = NULL;
						}
					}
				}
			}
			loopCounter++;	
		}
		totNumChars = 0;
		for(int i = 0; i < 127; i ++) {
			charList[i][1] = 0;
		}
		numUniqueChars = 0;
		//numUniqueChars = 0;		
		fseek(fpIn, 0, SEEK_SET); // resets the file pointer to beginning of file
		//finds all chars in file and puts them into charList
		char current = fgetc(fpIn);
		//printf("got first char from fpIn: current = %c\n", current);
		int varLoop = 0; // tracks position of char in file
		while(!feof(fpIn)) {
			if((int)current > -1 && (int)current < 128) {
				//printf("found match for: %c\n", current);
				//printf("current char count: %i\n", charList[(int)current][1]);
				if(charList[(int)current][1] == 0) { // if 0, new char
					//printf("current char count: %i\n", charList[(int)current][1]);
					charList[(int)current][0] = current;
					//printf("current char: %c\n", current);
					charList[(int)current][1] = 1;
					charList[(int)current][2] = varLoop;
					numUniqueChars++;
				}else{ // if not, already in list and just need to update count
					charList[(int)current][1] += 1;
				}
			} else {
				numUniqueChars++;
			}
		totNumChars++;
		varLoop++;
		current = fgetc(fpIn);
		}
	}
	int main(int argc, char *argv[] ){
		if(argc < 3) {
			printf("USAGE:\n\t./MADCounter -f <input file> -o <output file> -c -w -l -Lw -Ll\n\t\tOR\n\t./MADCounter -B <batch file>\n");
			exit(1);
		} 
		if(argc == 3) { // if 3 CLA's using batch
			FILE* batchFile = fopen(argv[2], "r");
			if(batchFile == NULL) {
				printf("ERROR: Can't open batch file\n");
				exit(1);
			}
			char * batchInfo;
			batchInfo = (char*)malloc(sizeof(char) * 256);	
			if(fgetc(batchFile) == EOF) {// if EOF, no lines
				printf("ERROR: Batch File Empty\n");
				exit(1);
			}
			fseek(batchFile, 0, SEEK_SET); // reset to start of file
			//batchInfo[strlen(batchInfo) - 1] = '\0';
			//printf("%s\n",batchInfo);
			while(fgets(batchInfo, 256, batchFile) != NULL) {
				batchInfo[strlen(batchInfo) - 1] = '\0';
				findFileInfo(batchInfo);
				//fgets(batchInfo, 256, batchFile);
				//batchInfo[strlen(batchInfo) - 1] = '\0';
			}
			//fprintf(fpOut, "made it through the batchfilen\n");
			free(batchInfo);
		} else {
			char * inputString = (char *)calloc(256, sizeof(char));
			for(int i = 0; i < argc; i++) { // put all args into string
				strcat(inputString, argv[i]);
				strcat(inputString, " "); 
			}
			//printf("Input string: %s\n", inputString);
			findFileInfo(inputString);
			free(inputString);
		}
	//printf("made it out time to free\n");
	//frees all words
	WORD * currWord = firstWord; // temp var
	while(currWord != NULL) { // frees each Word
		if(currWord->prevWord != NULL) {
			free(currWord->prevWord);
		}
		free(currWord->contents);
		currWord = currWord->nextWord;		
	}
	free(currWord);
	//printf("I was able to free all the words\n");
	//frees all lines
	LINE * currLine = firstLine->nextLine;
	while(currLine != NULL) { // frees each allocated line
		if(currLine->prevLine != NULL) {
			free(currLine->prevLine);
		}
		free(currLine->contents);
		currLine = currLine->nextLine;
	}
	free(currLine);
	//printf("I was able to free all the lines\n");
	fclose(fpIn);
	if(outputFileFound == 1) {
		fclose(fpOut);
	}	
	//printf("all files closed\n");
}
