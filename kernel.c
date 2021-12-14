// OS Kernel
// Jarrod LeComte

void printString(char*);
void readString(char*);
void readSector(char*, int);
void handleInterrupt21(int, int, int, int);
void readFile(char*, char*, int*);
void executeProgram(char*);
void terminate();
void writeSector(char*, int);
void deleteFile(char*);
void writeFile(char*, char*, int);
void handleTimerInterrupt(int, int);
void printChar(char);

int processActive[8];
int processStackPointer[8];
int currentProcess;

void main(){
/*
	int startVidMem = 0xb800;
	int vidMemOffset = 0x0;
	int white = 0x7;
	char* letters = "Hello World\0";
    char line[80];
    char buffer[13312];

    makeInterrupt21();

    //Write file test; currently only repeats the name and command'
    interrupt(0x21,8,"this is a test message","testmg",3);
*/

    
    int i;

    //Initializes the process table
    for(i = 0; i < 8; i++){
        processActive[i]=0;
        processStackPointer[i]=0xff00;
    }
    currentProcess = -1;


    makeInterrupt21();
    makeTimerInterrupt();

    //Runs the shell
    interrupt(0x21, 4, "shell", 0, 0);

	while(1);

}

void printChar(char c){

    interrupt(0x10, 0xe*256+c, 0, 0, 0);

}

void printString(char* chars){
    
    //Loops until \0 is reached. Uses 0x10 interrupt for each input character
    int i = 0;
    while(chars[i] != '\0'){
        int charvalue = chars[i];
        interrupt(0x10, 0xe*256+charvalue, 0, 0, 0);
        i++;
    }
}

void readString(char* line){

    //Values of keys
    int enter = 0xd;
    int backspace = 0x8;

    int i = 0;

    while(1){

        int asciivalue = interrupt(0x16, 0, 0, 0, 0);

        if(asciivalue == enter){
	        line[i] = 0xd;
            interrupt(0x10, 0xe*256+0xd, 0, 0, 0);
			i++;
			line[i] = 0xa;
            interrupt(0x10, 0xe*256+0xa, 0, 0, 0);
			i++;
			line[i] = 0x0;
            interrupt(0x10, 0xe*256+0x0, 0, 0, 0);
			break;
        }
        else if(asciivalue == backspace){
            if(i>0){
                line[i] = 0x0;
                i--;
                interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
				i++;
				interrupt(0x10, 0xe*256+0x0, 0, 0, 0);
				i--;
				interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
            }
        }
        else{
            line[i] = asciivalue;
            interrupt(0x10, 0xe*256+asciivalue, 0, 0, 0);
            i++;
        }

    }//end of while loop
}

void readSector(char* buffer, int sector){

	int ah = 2; // 2 is read
	int al = 1; // Number of sectors to read
	int bx = buffer; // Data storage address
	int ch = 0; // Track number
	int cl = sector + 1; //Relative sector number (sector + 1)
	int dh = 0; // Head number
	int dl = 0x80; //Device number (0x80 for hard disk)

	int ax = ah*256+al;
	int cx = ch*256+cl;
	int dx = dh*256+dl;

	interrupt(0x13, ax, bx, cx, dx);

}

void handleInterrupt21(int ax, int bx, int cx, int dx){
	if (ax == 0){
	    printString(bx);
    }
	else if (ax == 1){
		readString(bx);
	}
	else if (ax == 2){
		readSector(bx, cx);
	}
    else if (ax == 3){
        readFile(bx,cx,dx);
    }
    else if (ax == 4){
        executeProgram(bx);
    }
    else if (ax == 5){
        terminate();
    }
	else if (ax == 6){
		writeSector(bx, cx);
	}
    else if (ax == 7){
		deleteFile(bx);
	}
    else if (ax == 8){
		writeFile(bx, cx, dx);
	}
    else if (ax > 8){
		printString("Error\0");
	}
}

//TESTING FOR PROJECT C
void readFile(char* filename, char* buffer, int* sectornumber){
    char directorysector[512];
    int offset;
    int sectorpointer = 0;
    int bufferpointer = 0;

    readSector(&directorysector, 2);

    

    //Loops until a file match is made, or the sector is finished
    while(1){

        //Checks current location for file name. If found, save the position
        if(directorysector[sectorpointer]==filename[0] && directorysector[sectorpointer+1]==filename[1] && directorysector[sectorpointer+2]==filename[2] && directorysector[sectorpointer+3]==filename[3] && directorysector[sectorpointer+4]==filename[4] && directorysector[sectorpointer+5]==filename[5]){

            while (directorysector[sectorpointer+6] != 0){
		        readSector(buffer + bufferpointer, directorysector[sectorpointer+6]);
		        bufferpointer += 512;
		        sectorpointer++;
                *sectornumber = *sectornumber + 1;
	        }
            return;
        }

        //Went to far without finding anything!
        else if(sectorpointer>=512){
            char errorMessage[15];
		    errorMessage[0] = 'F';
		    errorMessage[1] = 'i';
		    errorMessage[2] = 'l';
		    errorMessage[3] = 'e';
		    errorMessage[4] = ' ';
		    errorMessage[5] = 'n';
		    errorMessage[6] = 'o';
		    errorMessage[7] = 't';
		    errorMessage[8] = ' ';
		    errorMessage[9] = 'f';
		    errorMessage[10] = 'o';
		    errorMessage[11] = 'u';
		    errorMessage[12] = 'n';
		    errorMessage[13] = 'd';
		    errorMessage[14] = '\0';
		    printString(errorMessage);

		    *buffer = '\0';
		    return;
        }

        //Nothing found, so keep skipping ahead
        else{
            sectorpointer+=32;
        }
    }
}

void executeProgram(char* name){
    char buffer[13312];
    int i;
    int z;
    int dataseg;
    int sectorsread;

    readFile(name,buffer,&sectorsread);

    for(i = 0; i < 8; i++){
        dataseg = setKernelDataSegment();
        if(processActive[i]==0){
            //Copies over the buffer to the segment
            for(z = 0; z < 13312; z++){
                putInMemory(((i+2)*0x1000), z, buffer[z]);
            }
            initializeProgram((i+2)*0x1000);
            processActive[i] = 1;
            processStackPointer[i] = 0xff00;
        }
        else{
            //Keep moving; it's being used
        }
        restoreDataSegment(dataseg);
    }

    /*
    for(i = 0; i < 13312 ; i++){
        putInMemory(0x2000, i, buffer[i]);   
    }
    */
    
}

void terminate(){

    char shellname[6];
	shellname[0] = 's';
	shellname[1] = 'h';
	shellname[2] = 'e';
	shellname[3] = 'l';
	shellname[4] = 'l';
	shellname[5] = '\0';

	executeProgram(shellname);

    while(1);
}

void writeSector(char* buffer, int sector){

	int ah = 3; // 2 is read
	int al = 1; // Number of sectors to read
	int bx = buffer; // Data storage address
	int ch = 0; // Track number
	int cl = sector + 1; //Relative sector number (sector + 1)
	int dh = 0; // Head number
	int dl = 0x80; //Device number (0x80 for hard disk)

	int ax = ah*256+al;
	int cx = ch*256+cl;
	int dx = dh*256+dl;

	interrupt(0x13, ax, bx, cx, dx);

}

void deleteFile(char* filename){
    char dir[512];
    char map[512];
    int y;
    int z;

    readSector(map, 1);
    readSector(dir, 2);

    //Loops through sector, checking for matching file names. If found, clear out the associated sector numbers and set map bytes to 0. Write all of the corrections down, then end the process
    for(z = 0; z < 512; z+=32){
        if(filename[0]==dir[z] && filename[1]==dir[z+1] && filename[2]==dir[z+2] && filename[3]==dir[z+3] && filename[4]==dir[z+4] && filename[5]==dir[z+5]){
            dir[z] = '\0';
            //Then check for sector numbers belonging to the file, and set map byte to 0
            //Then write dir and map back to sector using writeSector
            //Map Deletion: dir[z+6] of dir, find value and go to map[value] and 0
            for(y = z; y < 26+z; y++){
                map[dir[y+6]] = 0;
            }
            
            //Writes the changes back to map and dir to the sector
            writeSector(map,1);
            writeSector(dir,2);

            break;
        }
    }
}

void writeFile(char* buffer, char* filename, int numberOfSectors){
    char dir[512];
    char map[512];
    int x;
    int y;
    int z;

    readSector(map, 1);
    readSector(dir, 2);

    //Loop through sector for a blank directory entry (\0)
    for(z = 0; z < 512; z+=32){

        if(dir[z] == '\0'){
            //Loop through the 6-character file name and fill it in. If there are blanks or spaces, add \0
            for(y = 0; y < 6; y++){
                if(filename[y]=='\0'){
                    dir[z+y]='\0';                    
                }
                else{
                    dir[z+y]=filename[y];
                }
            }

            //SECTOR SECTION
            for(y = 0; y < numberOfSectors; y++){

                for(x = 3; x < 26+z; x++){
                    if(map[x]==0){
                        map[dir[x+6]] = 0xFF;
                        dir[z+6]=x;
                        writeSector(buffer, x);
                        //Need to fill in remaining with 0s
                    }
                }

                //Writes the changes back to map and dir to the sector
                writeSector(map,1);
                writeSector(dir,2);
            }

            //Stop it from continuing the search for open space after it found something
            break;
        }
    }
}

void handleTimerInterrupt(int segment, int sp){

    int dataseg,s,i;

    //'Tic' works just fine

    //printChar('T');
    //printChar('i');
    //printChar('c');
    returnFromTimer(segment, sp);
}

