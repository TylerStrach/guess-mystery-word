//  wordleGuessSecretWord.c
//  Have the program do the guessing to discover the secret wordle word.
//
//  Author: Tyler Strach
//  System: CLion and XCode
//
//  Links to wordle dictionary words at:
//    https://www.reddit.com/r/wordle/comments/s4tcw8/a_note_on_wordles_word_list/


/*  Running the program looks like:
        Using file wordsLarge.txt with 12947 words. 
        -----------------------------------------------------------
        
        enter a secret word or just r to choose one at random: dream
        Trying to find secret word: 
               d r e a m 
        
            1. s o r e s 
                   * *   
            2. s e r e r 
                 * *     
            3. r a r e r 
               * *   *   
            4. r a r e e 
               * *   *   
            5. s a r e e 
                 * * *   
            6. p a r e r 
                 * * *   
            7. c a r e r 
                 * * *   
            8. b a r e r 
                 * * *   
            9. f a r e r 
                 * * *   
           10. D a r e r 
                 * * *   
           11. D e a r e 
                 * * *   
           12. D e b A r 
                 *     * 
           13. D R E A r 
                       * 
           14. D R E A d 
                         
           15. D R E A M 
                         
        Got it!

     ... and then it runs two more times ...
 */
#include <stdio.h>    // for printf(), scanf()
#include <stdlib.h>   // for exit( -1)
#include <string.h>   // for strcpy
#include <assert.h>   // for assert() sanity checks
#include <ctype.h>    // for toupper()
#include <time.h>     // for time()
#include <stdbool.h>  // for boolean variables

// Declare globals
#define WORD_LENGTH 5     // All words have 5 letters, + 1 NULL at the end when stored
//#define WORDS_FILE_NAME "wordsLarge.txt"
#define WORDS_FILE_NAME  "wordsLarge.txt"
#define MAX_NUMBER_OF_WORDS 12947   // Number of words in the full set of words file
#define true 1   // Make boolean logic easier to understand
#define false 0  // Make boolean logic easier to understand

typedef struct wordCount wordCountStruct;
struct wordCount{
    char word[ WORD_LENGTH + 1];   // The word length plus NULL
    int score;                     // Score for the word
};

//-----------------------------------------------------------------------------------------
// Read in words from file into array.  We've previously read the data file once to
// find out how many words are in the file.
void readWordsFromFile(
        char fileName[],        // Filename we'll read from
        wordCountStruct *words, // Array of words where we'll store words we read from file
        int *wordCount)          // How many words.  Gets updated here and returned
{
    FILE *inFilePtr  = fopen(fileName, "r");  // Connect logical name to filename
    assert( inFilePtr != NULL);               // Ensure file open worked correctly

    // Read each word from file and store into array, initializing the score for that word to 0.
    char inputString[ 6];
    *wordCount = 0;
    while( fscanf( inFilePtr, "%s", inputString) != EOF) {
        strcpy( words[ *wordCount].word, inputString);
        words[ *wordCount].score = 0;
        (*wordCount)++;
    }

    // Close the file
    fclose( inFilePtr);
} // end readWordsFromFile(..)


//-----------------------------------------------------------------------------------------
// Comparator for use in built-in qsort(..) function.  Parameters are declared to be a
// generic type, so they will match with anything.
// This is a two-part comparison.  First the scores are compared.  If they are the same,
// then the words themselves are also compared, so that the results are in descending
// order by score, and within score they are in alphabetic order.
int compareFunction( const void * a, const void * b) {
    // Before using parameters we have cast them into the actual type they are in our program
    // and then extract the numerical value used in comparison
    int firstScore = ((wordCountStruct *) a)->score;
    int secondScore = ((wordCountStruct *) b)->score;

    // If scores are different, then that's all we need for our comparison.
    if (firstScore != secondScore) {
        // We reverse the values, so the result is in descending vs. the otherwise ascending order
        // return firstScore - secondScore;   // ascending order
        return secondScore - firstScore;      // descending order
    }
    else {
        // Scores are equal, so check words themselves, to put them in alphabetical order
        return strcmp( ((wordCountStruct *)a)->word,  ((wordCountStruct *)b)->word );
    }
} //end compareFunction(..)

/**
 * calculates the points and stores the point totals in each word struct
 * borrowed from my project 3 submission
 * @param totalWords
 * @param allWords
 */
void calculatePoints(int totalWords, wordCountStruct *allWords) {
    int points;
    for (int i = 0; i < totalWords; i++) { // for each word in allWords
        if(allWords[i].score == -1)
            continue;
        points = 0;
        for (int j = 0; j < totalWords; j++) { // compares each word to every other word in array
            if(allWords[j].score == -1)
                continue;
            // reset flags for each word being compared
            bool threePointFlags[5] = {0, 0, 0, 0, 0}; // set to 1 if index counted as 3pts
            bool onePointFlags[5] = {0, 0, 0, 0, 0}; // set to 1 if index counted as 1pt
            for (int k = 0; k < 5; k++) { // first: check if correct character in the same index for each letter
                if (allWords[i].word[k] == allWords[j].word[k]) {
                    threePointFlags[k] = 1;
                    points += 3;
                }
            }
            for (int l = 0; l < 5; l++) { // second: check for each letter in "i" word
                if (threePointFlags[l] != 1) { // if already counted as 3pts, skip to next letter
                    for (int m = 0; m < 5; m++) { // for each letter in the "j" word
                        if ((allWords[i].word[l] == allWords[j].word[m]) && (threePointFlags[m] != 1) &&
                            (onePointFlags[m] !=
                             1)) { // if characters match and the 1pt flags for both were not flagged
                            onePointFlags[m] = 1;
                            points += 1;
                            break; // move onto next index
                        }
                    }
                }
            }
        }
        allWords[i].score = points; // save the points
    }
}


// -----------------------------------------------------------------------------------------
// Find a secret word
void findSecretWord(
        wordCountStruct allWords[],    // Array of all the words
        int wordCount,                  // How many words there are in allWords
        char secretWord[])              // The word to be guessed
{
    char computerGuess[ 6];  // Allocate space for the computer guess

    printf("Trying to find secret word: \n");
    // Display secret word with a space between letters, to match the guess words below.
    printf("       ");
    for( int i=0; i<WORD_LENGTH; i++) {
        printf("%c ", secretWord[ i]);
    }
    printf("\n");
    printf("\n");

    // Loop until the word is found
    int guessNumber = 1;
    bool wordIsFound = false;
    char prevGuess[6];
    while( !wordIsFound ) {
        // Lots of code to go here ...
        calculatePoints(wordCount, allWords);
        qsort(allWords, wordCount, sizeof(wordCountStruct), compareFunction);

        if(guessNumber == 1)
            strcpy(computerGuess, "amaze");
        else
            strcpy(computerGuess, allWords[0].word);
        allWords[0].score = -1;

        char foundLetters[] = {' ', ' ', ' ', ' ', ' '};
        char guessResults[] = {' ', ' ', ' ', ' ', ' '};
        char cantHave[] = {' ', ' ', ' ', ' ', ' '};

        // check if letters match in correct order
        for(int i = 0; i < 5; i++){
            if(secretWord[i] == computerGuess[i]){
                foundLetters[i] = toupper(secretWord[i]);
                computerGuess[i] = toupper(secretWord[i]);
            }
        }

        printf("%5d. %c %c %c %c %c\n", guessNumber, computerGuess[0], computerGuess[1], computerGuess[2],
               computerGuess[3], computerGuess[4]);

        // checks if the word has been found
        bool allMatch = true;
        for(int i = 0; i < 5; i++){ // checks if all letters match
            if(computerGuess[i] != toupper(secretWord[i]))
                allMatch = false;
        }
        if(allMatch)
            wordIsFound = true;


        // check if letter is in wrong order
        bool dupFlag = false;
        for(int i = 0; i < 5; i++){

            if(guessResults[i] != ' ') // checks if its not ' '
                continue;

            // checks if character was already found
            dupFlag = false;
            for(int a = 0; a < 5; a++){
                if(computerGuess[i] == foundLetters[a])
                    dupFlag = true;
            }
            if(dupFlag)
                continue;

            // checks for letter in wrong position
            for(int j = 0; j < 5; j++){
                if(computerGuess[i] == secretWord[j]){
                    guessResults[i] = '*';
                    foundLetters[i] = tolower(computerGuess[i]);
                }
            }
        }

        // marks letters that cannot be present in the word
        for(int i = 0; i < 5; i++){
            if(guessResults[i] == ' ')
                cantHave[i] = computerGuess[i];
        }

        printf("       %c %c %c %c %c\n", guessResults[0], guessResults[1], guessResults[2],
               guessResults[3], guessResults[4]);

        // if the word has letters that were already guessed, score = -1
        for(int i = 0; i < wordCount; i++){
            for(int j = 0; j < 5; j++){
                if(cantHave[j] != ' ' && cantHave[j] == allWords[i].word[j] )
                    allWords[i].score = -1;
            }
        }

        // updates the scores to remove for next guess
        // if the uppercase letter is not in that position, score = -1
        for(int i = 0; i < wordCount; i++){
            for(int j = 0; j < 5; j++){
                if(foundLetters[j] != ' ' && foundLetters[j] == toupper(foundLetters[j]) &&
                foundLetters[j] != toupper(allWords[i].word[j]))
                    allWords[i].score = -1;
            }
        }

        // if the lowercase letter does not exist somewhere in the word, score = -1
        for(int i = 0; i < wordCount; i++){
            bool foundLetter = false;
            for(int j = 0; j < 5; j++){
                if(foundLetters[j] == ' ' || foundLetters[j] == toupper(foundLetters[j])){
                    continue;
                }
                for(int k = 0; k < 5; k++){
                    if(foundLetters[j] == allWords[i].word[k]){
                        foundLetter = true;
                    }
                }
                if(foundLetter == false)
                    allWords[i].score = -1;
            }
        }

        // Update guess number and store guess word to prevent repeats

        guessNumber++;
        strcpy(prevGuess, computerGuess);
    } //end for( int i...)

    for(int i = 0; i < wordCount; i++){
        allWords[i].score = 0;
    }

    printf("Got it!\n");
} //end findSecretWord


// -----------------------------------------------------------------------------------------
int main() {
    char wordsFileName[81];                   // Stores the answers file name
    strcpy(wordsFileName, WORDS_FILE_NAME);   // Set the filename, defined at top of program.
    srand( (unsigned) time( NULL));           // Seed the random number generator to be current time
    // Declare space for all the words, of a maximum known size.
    wordCountStruct allWords[ MAX_NUMBER_OF_WORDS];
    // Start out the wordCount to be the full number of words.  This will decrease as
    //    play progresses each time through the game.
    int wordCount = 0;
    // The secret word that the computer will try to find, plus the return character from fgets.
    char secretWord[ WORD_LENGTH + 1];
    char userInput[ 81];                // Used for menu input of secret word

    // Read in words from file, update wordCount and display information
    readWordsFromFile( wordsFileName, allWords, &wordCount);
    printf("Using file %s with %d words. \n", wordsFileName, wordCount);

    // Run the word-guessing game three times
    for( int i=0; i<3; i++) {
        // Reset secret Word
        strcpy( secretWord, "");
        // Display prompt
        printf("-----------------------------------------------------------\n");
        printf("\n");
        printf("Enter a secret word or just r to choose one at random: ");
        scanf(" %s", userInput);
        // Eliminate the return character at end or userInput if it is there
        int length = (int) strlen( userInput);
        if( userInput[ length] == '\n') {
            userInput[ length] = '\0';
        }
        strcpy( secretWord, userInput);   // Store the secret word from user input

        // If input was 'r' then choose a word at random.
        if( strlen( secretWord) <= 1) {
            // Randomly select a secret word to be guessed.
            int randomIndex = rand() % wordCount;
            strcpy( secretWord, allWords[ randomIndex].word);
        }

        // Run the game once with the current secret word
        findSecretWord( allWords, wordCount, secretWord);
    }

    printf("Done\n");
    printf("\n");
    return 0;
} // end main()