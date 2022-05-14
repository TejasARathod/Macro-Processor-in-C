#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME_LENGTH 16
#define MAX_LINE_LENGTH 260

struct lineMap {
   char label[MAX_NAME_LENGTH + 1]; // created an array to define the table where we will be storing opcode,operands..
   char opcode[MAX_NAME_LENGTH + 1];
   char arguments[MAX_LINE_LENGTH + 1];
   int argOffs; // the purpose of lm.argOffs is to hold the index of the last element before the arguments to output the file with correct spacing.
} lm;  // lm here is the object created to interact with the above table.

char rdline[MAX_LINE_LENGTH]; //rdline is an array that is used to store the complete line which is scanned by the program.
int lineCnt = 0;//this is a counter

FILE *NAMTAB, *DEFTAB; // definition of file pointers(inptr : input, outptr : output, NAMTAB : NAMTAB, DEFTAB : DEFTAB)
FILE *inptr, *outptr;
char ARGTAB[MAX_LINE_LENGTH][MAX_NAME_LENGTH + 1]; // This is a 2d array

void getLineArgs() { // This function is used to separate different parts of a line (label, opcode and operands).
   lm.label[0] = '\0';  // initializing myself so that program does not assign garbage value
   lm.opcode[0] = '\0';  // for the lm.labels variable, it it necessary to initialize it to be empty. the other two are not necessary('\0' represents the end of a string)
   lm.arguments[0] = '\0';

   int strOffs = 0, readCount = 0, i = 0; // counters // in order to scanf through a string, i had to save the offset of the last read character and reuse it to skip through the parts that were read earlier

   if (isalpha(rdline[0]) || rdline[0] == '_') { // Check if this line has a label
      sscanf(rdline, "%s%n", lm.label, &readCount); // read the label
      strOffs += readCount;
   }

   sscanf(rdline + strOffs, "%s%n", lm.opcode, &readCount); // read the opcode (this code block would scan the opcode and store it in the opcode array)
   strOffs += readCount;

   lm.argOffs = -1;

   for (; rdline[strOffs] != 0 && rdline[strOffs] != '\n'; i++, strOffs++) { // Read the operands/arguments and separate with whitespace and commas
      if (rdline[strOffs] == ',') {
         lm.arguments[i] = ' ';
      } else {
         lm.arguments[i] = rdline[strOffs];
      }
      if ((isalpha(rdline[strOffs]) || rdline[strOffs] == '_') && lm.argOffs == -1) { // to set lm.argOffs to the last element before the arguments
         lm.argOffs = strOffs;
      }
   }

   lm.arguments[i] = '\0';// for terminating the array. /0 indicates the end of the array
   return;
}


int getMacroIndex(char *nm) { // This function checks whether the label exists in NAMTAB
   char tmpLine[2 * MAX_NAME_LENGTH + 1], tmp[MAX_NAME_LENGTH + 1]; //definition of two strings needed to loop thru NAMTAB // we are storing the information in nametab by going to the position accordingly ; stores it in tmpline as its scope is only restricted locally
   int index = -1; // index counter has been initialized to -1

   fseek(NAMTAB, 0, SEEK_SET);

   while (fgets(tmpLine, 2 * MAX_NAME_LENGTH + 1, NAMTAB)) { // 61-63: first of all, i read one line from the NAMTAB and then read the name and offset of the macro(offset from the beginning of DEFTAB), after that i check whether the name is a match or not, and then return the offset of the macro, if nothing matches, -1 is returned
      sscanf(tmpLine, "%s %d", tmp, &index);
      if (strcmp(tmp, nm) == 0) {
         return index;
      }
   }

   return -1;
}


int defineMacro() { // in defineMacro i am updating the DEFTAB data structure, which is copying every line of the macro definition except the first line and replacing macro variable names with numbers in this format : "?%d"
   int argOffs = 0, readCount = 0, argCount = 0;

   while (sscanf(lm.arguments + argOffs, "%s%n", ARGTAB[argCount], &readCount) != EOF) { // load macro variables into ARGTAB
      argOffs += readCount; // this loop is for scanning of the arguments array(operand) into the argtab
      argCount++;
   }

   fseek(DEFTAB, 0, SEEK_END); // fseek(FILE *ptr , long int offset , int position) Setting the position of the ptrs from start till searching for the end in DEFTAB
   fseek(NAMTAB, 0, SEEK_END);
   fprintf(NAMTAB, "%s %d\n", lm.label, ftell(DEFTAB)); // fprintf for writing into the file // ftell we r using to determine the position of the ptrs set above

   while (fgets(rdline, MAX_LINE_LENGTH, inptr)) { // traversing through the lines that has been scanned
      getLineArgs();
      lineCnt++;

      if (strcmp(lm.opcode, "MACRO") == 0) { // check if there is a nested macro
         return 1;
      }

      fprintf(DEFTAB, "%*.*s", lm.argOffs, lm.argOffs, rdline);

      argOffs = 0;
      char tmp[MAX_NAME_LENGTH + 1];
      while (sscanf(lm.arguments + argOffs, "%s%n", tmp, &readCount) != EOF) { // replace macro variables
         int argIndex = -1;
         for (int i = 0; i < argCount; i++) {
            if (strcmp(tmp, ARGTAB[i]) == 0) {
               argIndex = i;
               break;
            }
         }
         if (argIndex != -1) {
            fprintf(DEFTAB, "?%d ", argIndex);
         } else {
            fprintf(DEFTAB, "%s ", tmp);
         }
         argOffs += readCount;
      }

      fprintf(DEFTAB, "\n");

      if (strcmp("MEND", lm.opcode) == 0) { // check if the macro definition is complete
         return 0;
      }
   }
   return 2;
}


void expandLabel() { // in expandLabel i am reading deftab from the first line of corresponding macro until ENDM and replacing arguments formatted this way("?%d") with the corresponding name(ARGTAB[d] where d is the number after question mark)
   int pointer = getMacroIndex(lm.opcode); // in expand label i am also writing the modified text into output


   int argOffs = 0, readCount = 0, argCount = 0;

   while (sscanf(lm.arguments + argOffs, "%s%n", ARGTAB[argCount], &readCount) != EOF) { // load arguments into ARGTAB
      argOffs += readCount;
      argCount++;
   }

   fseek(DEFTAB, pointer, SEEK_SET);

   while (fgets(rdline, MAX_LINE_LENGTH, DEFTAB)) {
      getLineArgs();
      if (strcmp("MEND", lm.opcode) == 0) { // Check if this line is fully expanded
         return;
      }

      fprintf(outptr, "%*.*s", lm.argOffs, lm.argOffs, rdline);

      argOffs = 0;
      char tmp[MAX_NAME_LENGTH + 1];
      while (sscanf(lm.arguments + argOffs, "%s%n", tmp, &readCount) != EOF) { // replace arguments
         if (tmp[0] == '?') {
            int argIndex = 10;
            sscanf(tmp, "%*c%d", &argIndex);
            fprintf(outptr, "%s, ", ARGTAB[argIndex]);
         } else {
            fprintf(outptr, "%s, ", tmp);
         }

         argOffs += readCount;
      }

      if (argOffs) {
         fseek(outptr,-2,SEEK_END);
      }

      fprintf(outptr, "\n");
   }
   return;
}


int main(void) {
   char inpf[10] = "input.txt";
   char outf[11] = "output.txt";

   inptr = fopen(inpf, "r");
   outptr = fopen(outf, "w");
   NAMTAB = fopen("NAMTAB.txt", "w+");
   DEFTAB = fopen("DEFTAB.txt", "w+");

   if (inptr == NULL) { // check that all of the files are opened successfully
      printf("Could not open file : %s", inpf);
      return 3;
   }
   if (outptr == NULL) {
      printf("Could not open file : %s", outf);
      return 4;
   }
   if (NAMTAB == NULL) {
      printf("Could not open file : NAMTAB.txt");
      return 5;
   }
   if (DEFTAB == NULL) {
      printf("Could not open file : DEFTAB.txt");
      return 6;
   }


   while (fgets(rdline, MAX_LINE_LENGTH, inptr)) { // PROCESSLINE
      lineCnt++;
      getLineArgs();// opcodes, labels and arguments are stored in their respective arrays namely lm.label, lm.opcode, lm.argument

      if (getMacroIndex(lm.opcode) != -1) { // if current lines opcode is found in NAMTAB, expand
         expandLabel();
      } else if (strcmp(lm.opcode, "MACRO") == 0) { // if the current line is a macro definition, define
         if (getMacroIndex(lm.label) != -1) { // if this macro already exists, terminate the program
            printf("Duplicate MACRO detected in line : %d\n", lineCnt);
            return 7;
         } // 203-211 : "DEFINE" process as described in the algorithm, this function return an error value, that is analyzed and handled correspondingly(zero means there was no error)
         int tmpLine = lineCnt;
         int err = defineMacro();
         if (err == 1) { // nested macros are not allowed
            printf("Nested Macro Detected in line : %d\n", lineCnt);
            return 8;
         } else if (err == 2) {
            printf("MEND does not exist for the macro defined in line : %d\n", tmpLine);
            return 9;
         }
      } else { // if the current line is not referring to a macro, write it directly to the output
         fprintf(outptr, "%s", rdline); // write source line to expanded file as described in the algorithm
      }
   }

   fclose(inptr);
   fclose(outptr);

   return 0;
}
