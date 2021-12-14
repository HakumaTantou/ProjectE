//Jarrod LeComte

int main(){
	char prompt[4];
	char buffer[512];
    char fileSize[13312];
    char filename[7];
    char filename2[7];
	char badCommand[15];
    char nextline[2];
	char commandType[5];
	char commandExecute[5];
    char commandDir[4];
    char commandDel[4];
    char commandCopy[5];
    int pointer = 0;
    int wordnum;
    int i;
    int x;
    int z;
    
	prompt[0] = 'A';
	prompt[1] = ':';
	prompt[2] = '>';
	prompt[3] = '\0';

    nextline[0] = '\r';
    nextline[1] = '\n'; 

	commandType[0] = 't';
	commandType[1] = 'y';
	commandType[2] = 'p';
	commandType[3] = 'e';
	commandType[4] = '\0';

	commandExecute[0] = 'e';
	commandExecute[1] = 'x';
	commandExecute[2] = 'e';
	commandExecute[3] = 'c';
	commandExecute[4] = '\0';

    commandDir[0] = 'd';
    commandDir[1] = 'i';
    commandDir[2] = 'r';
    commandDir[3] = '\0';

    commandDel[0] = 'd';
    commandDel[1] = 'e';
    commandDel[2] = 'l';
    commandDel[3] = '\0';

    commandCopy[0] = 'c';
    commandCopy[1] = 'o';
    commandCopy[2] = 'p';
    commandCopy[3] = 'y';
    commandCopy[4] = '\0';


    

	while (1){
        //Prints out the A:> prompt
		syscall(0, prompt, 0, 0);
        //Accepts user input and displays it
		syscall(1, buffer, 30, 0);


        //Sets up file name
        for(i = 0; i < 18; i++){
            if(buffer[i]==' ' && i > 2){

                wordnum++;
                
                //After the first space
                if(wordnum==1){
                    for(z = 0; z < 6; z++){
                        filename[z]=buffer[i+z+1];
                    }
                }
                //If there is a second space
                if(wordnum==2){
                    for(z = 0; z < 6; z++){
                        filename2[z]=buffer[i+z+1];
                    }
                }
            }
        }

        
        /*
        //Sets up file name
        for(i = 0; i < 18; i++){
            if(buffer[i]==' '){
                for(z = 0; z < 13; z++){
                    if(buffer[i+z+1] == ' ' || buffer[i+z+1] == '\0'){
                        filename[z] = '\0';
                    }
                    else{
                        filename[z]=buffer[i+z+1];
                    }
                }
            }
        }
        */
        
        
        //type
        if(buffer[0]==commandType[0] && buffer[1]==commandType[1] && buffer[2]==commandType[2] && buffer[3]==commandType[3]){
            syscall(3, filename, buffer, 2);
            syscall(0, buffer, 0, 0);
        }
        
        //exec
        else if(buffer[0]==commandExecute[0] && buffer[1]==commandExecute[1] && buffer[2]==commandExecute[2] && buffer[3]==commandExecute[3]){
            syscall(3, filename, buffer, 2);
            syscall(5, filename, 0, 0);
        }

        //del
        else if(buffer[0]==commandDel[0] && buffer[1]==commandDel[1] && buffer[2]==commandDel[2]){
            syscall(7, filename, 0, 0);
        }
        
        //dir
        else if(buffer[0]==commandDir[0] && buffer[1]==commandDir[1] && buffer[2]==commandDir[2]){
            char directory[512];      
            syscall(2, directory, 2);
            
            for(z = 0; z < 512; z+=32){
                for(i = 0; i < 6; i++){
                    filename[i]=directory[i+z];
                }
                if(filename[0]!='\0'){
                    syscall(0, filename, 0, 0);
		            syscall(0, nextline, 0, 0);
                }
                
            }

        }
        /*
        //copy
        else if(buffer[0]==commandCopy[0] && buffer[1]==commandCopy[1] && buffer[2]==commandCopy[2] && buffer[3]==commandCopy[3]){
            syscall(3, filename, buffer, 2);
            syscall(6, )
        }
        */

        else{
            badCommand[0] = 'B';
		    badCommand[1] = 'a';
		    badCommand[2] = 'd';
		    badCommand[3] = ' ';
		    badCommand[4] = 'C';
		    badCommand[5] = 'o';
		    badCommand[6] = 'm';
		    badCommand[7] = 'm';
		    badCommand[8] = 'a';
		    badCommand[9] = 'n';
		    badCommand[10] = 'd';
		    badCommand[11] = '!';
            badCommand[12] = '\r';
            badCommand[13] = '\n';
            badCommand[14] = '\0';
		    syscall(0, badCommand, 0, 0);
            break;
        }

        //Resets the interface
        wordnum = 0;
        for(i = 0; i < 512; i++){
            buffer[i] = '\0';
            filename[i] = '\0';
            filename2[i] = '\0';
        }

        /*
        // Error message
		else{
			badCommand[0] = 'B';
			badCommand[1] = 'a';
			badCommand[2] = 'd';
			badCommand[3] = ' ';
			badCommand[4] = 'C';
			badCommand[5] = 'o';
			badCommand[6] = 'm';
			badCommand[7] = 'm';
			badCommand[8] = 'a';
			badCommand[9] = 'n';
			badCommand[10] = 'd';
			badCommand[11] = '!';
            badCommand[12] = '\r';
            badCommand[13] = '\n';
            badCommand[14] = '\0';
			syscall(0, badCommand, 0, 0);
		}
        */
	}
}
