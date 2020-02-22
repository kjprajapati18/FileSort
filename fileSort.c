#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

typedef struct _Node{
  char* value;
  struct _Node* next;
} Node;

int getInput(Node* head, int fd);
int intComparator(void* nodeOne, void* nodeTwo);
int stringComparator(void* nodeOne, void* nodeTwo);
int isNumber(char* string);
int isString(char* string);

int insertionSort(void* toSort, int (*comparator)(void*, void*));
int quickSort(void* toSort, int(*comparator)(void*, void*));

int main(int argc, char* argv[]){

  //Error checking with inputs
  if(argc != 3){
    printf("Fatal Error: The amount of arguments is incorrect\n");
    return -1;
  }

  int quicksort = -1;

  if(*(argv[1]) == '-' && *(argv[1]+1) == 'q'){
    printf("Using the quicksort flag!\n");
    quicksort = 1;
  } else if( *(argv[1]) == '-' && *(argv[1]+1)== 'i'){
    printf("Using the insertion sort flag!\n");
    quicksort = 0;
  } else {
    printf("Fatal Error: You must use either -q or -i as a flag\n");
    return -1;
  }

  int fd = open(argv[2], O_RDONLY);

  if(fd < 0){
    printf("Fatal Error: Could not open file\n");
    return -1;
  }

  //All inputs for calling the function are valid, try to read and store the words

  Node* head = (Node*) malloc(sizeof(Node));
  Node* ptr = head;
  int valid = getInput(head, fd);
  if(valid == -1){
    printf("Fatal Error: Something went wrong reading. Errno: %d\n", errno);
    return -1;
  }
  else if(valid == 0){
    printf("Error: malloc returned a NULL pointer. Continuing with currently stored values.\n");
  }
  while(head != NULL){
    printf("%s\n", head->value);
    head = head->next;
  }
  /*printf("Hello workld");
  int check1 = isString(ptr->value);
  printf("%d\n", check1);
  int check = stringComparator(ptr->value, ptr->next->value);
  printf("%d\n", check);
  int (*strComp) (void* p, void* q) = &stringComparator;
  quickSort(ptr, (*strComp));
  while(ptr != NULL){
    printf("%s\n", ptr->value);
    ptr= ptr->next;
    }*/
  

  return 0;
}

//Returns -1 on read error
//Returns 0 on malloc error (NULL malloc)
//Returns 1 on success
int getInput(Node* head, int fd){


  char buf;                //store reads
  Node* prev = NULL;       //Prev pointer in case of malloc returning null
  int bytesRead = 1;       //Find out when read doesn't read any bytes
  int numOfWordBytes = 0;  //Find out how many bytes in the current word
  int sizeMult = 1;        //Multiplier for when we increase word malloc size
  int initialSize = 32;    //Initial malloc size per word

  while(bytesRead > 0){

    //malloc initial word size and check if error
    head->value = (char*) malloc(initialSize*sizeof(char));
    if(head->value == NULL){
      free(head);
      prev->next = NULL;
      printf("\nLast full word stored was: %s\n", prev->value);
      return 0;
    }

    //Read one byte. If space, continue. If comma, get out of this loop so we can finalize the Node
    do{
      bytesRead = read(fd,&buf,1);
      if(bytesRead == -1) return -1;
      if(buf == ',') break;
      if(isspace(buf) != 0) continue;

      if(bytesRead != 0) *(head->value + numOfWordBytes) = buf;
      numOfWordBytes += bytesRead;

      //If we run out of space, we need to malloc more space and copy everything
      if(numOfWordBytes == initialSize*sizeMult*sizeof(char)){
        sizeMult*=2;
        char* newTemp = (char*) malloc(initialSize*sizeMult*sizeof(char));
        if(newTemp == NULL){
          free(head->value);
          free(head);
          prev->next = NULL;
          printf("\nLast full word stored was: %s\n", prev->value);
          return 0;
        }
        memcpy(newTemp, head->value, numOfWordBytes+1);
        free(head->value);
        head->value = (char*) malloc(initialSize*sizeMult*sizeof(char));
        memcpy(head->value, newTemp, numOfWordBytes+1);
        free(newTemp);
      }

    }while(bytesRead >0);

    //Set the null terminating character in case we hit EOF (EOF will print out question mark box)
    *(head->value + numOfWordBytes) = '\0';

    //Word is now complete, so we reset everything and move onto the next word/Node
    if(numOfWordBytes == 0 && bytesRead == 0){
      //Only will come here if the last character is a comma (don't create the empty token)
      return 1;
    }

    //If we exited the loop by hitting a comma, then go into this and create the next node.
    //If EOF, no need to reset or create anything
    if(bytesRead > 0){
      numOfWordBytes = 0;
      sizeMult = 1;

      head->next = (Node*) malloc(sizeof(Node));
      prev = head;
      head = head->next;
      if(head == NULL){
        prev->next = NULL;
        printf("\nLast full word stored was: %s\n", prev->value);
        return 0;
      }
    }
  }

  return 1;
}

//isNumber & isString determines whether a char* is entirely numerical or alphabetical respectively
//Return 0 for false
//Return 1 for true
int isNumber(char *string){
  
  int i = 0;
  if(string[0] == '-' || isdigit(string[0])){
    i++;
  } else {
    return 0;
  }
  
  while(*(string + i) != '\0'){
    if(isdigit(*(string+i))) continue;
    else return 0;
  }
  
  return 1;
}

int isString(char *string){
  int i = 0;
  while(*(string+i) != '\0'){
    if(isalpha(*(string+i))) continue;
    else return 0;
  }
  return 1;
}


int intComparator(void* inp1, void* inp2){
  int num1 = atoi((char*) inp1);
  int num2 = atoi((char*) inp2);
  if (num1 < num2){
    return -1;
  }
  else if (num1 > num2){
    return 1;
  }
  else{
    return 0;
  }
}

int stringComparator(void* nodeOne, void* nodeTwo){
  int size;
  int i;
  int same = 0;
  int which;
  //find which string is shorter
  if (strlen((char*) nodeOne) < strlen((char*) nodeTwo)){
    size = strlen((char*) nodeOne);
    which = 0;
  }
  else{
    size = strlen((char*) nodeTwo);
    which = 1;
  }
  if (strlen((char*) nodeOne) == strlen((char*) nodeTwo)){
    same = 1;
  }
  
  //check char by char for differences
  for(i = 0; i < size; i++){
    if(((char*) nodeOne)[i] < ((char*) nodeTwo)[i]){
      return -1;
    }
    else if( ((char*) nodeOne)[i] > ((char*) nodeTwo)[i]){
      return 1;
    }
  }
  
  if(same){
    return 0;
  }
  if (which){
    return 1;
  }
  else{
    return -1;
  }
}

int insertionSort(void* toSort, int (*comparator)(void*, void*)){
  Node* head = (Node*) toSort;
  return 0;
}

int quickSort(void* toSort, int (*comparator)(void*, void*)){
  if (toSort == NULL){
    return 0;
  }
  Node* pivot = (Node*) toSort;
  Node* head = (Node*) toSort;
  Node* prev = pivot;
  Node* ptr = pivot->next;
  while (ptr != NULL){
    if(comparator(ptr->value, pivot->value) == -1){
      prev->next = ptr->next;
      Node* temp = head;
      head = ptr;
      head->next = temp;
      ptr = prev->next;
    }
    else{
      prev = ptr;
      ptr = ptr->next;
    }
  }
  quickSort(head, comparator);
  quickSort(pivot->next, comparator);
  return 0;
}

