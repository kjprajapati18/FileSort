//TO DO LIST:
//1. ADD IF MALLOC IS NULL RETURN ERROR
//3. CHECK IF EVERYTHING WORKS
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

typedef struct LL{
  Node* first;
}LL;

int getInput(LL* list, int fd);
int intComparator(void* nodeOne, void* nodeTwo);
int stringComparator(void* nodeOne, void* nodeTwo);
int isNumber(char* string);
int isString(char* string);

int insertionSort(void* toSort, int (*comparator)(void*, void*));
int quickSort(void* toSort, int(*comparator)(void*, void*));

void freeList(LL* list);

int main(int argc, char* argv[]){

  //Error checking with inputs
  if(argc != 3){
    printf("Fatal Error: The amount of arguments is incorrect.\n");
    return -1;
  }

  int quicksort = -1;

  if(*(argv[1]) == '-' && *(argv[1]+1) == 'q'){
    //printf("Using the quicksort flag!\n");
    quicksort = 1;
  } else if( *(argv[1]) == '-' && *(argv[1]+1)== 'i'){
    //printf("Using the insertion sort flag!\n");
    quicksort = 0;
  } else {
    printf("Fatal Error: \"%s\" is not a valid sort flag\n", argv[1]);
    return -1;
  }

  int fd = open(argv[2], O_RDONLY);

  if(fd < 0){
    printf("Fatal Error: Could not open file %s\n", argv[2]);
    return -1;
  }

  //All inputs for calling the function are valid, try to read and store the words
  LL* linkedList = (LL*) malloc(sizeof(LL));
  if(linkedList == NULL){
    printf("Fatal Error: Cannot malloc enough space to start accepting tokens. \n");
    close(fd);
    return -1;
  }
  int valid = getInput(linkedList, fd);
  Node* ptr4 = linkedList->first;
  close(fd);
  //test print the list pre sort and post whie spce fix
  /* while(ptr4 != NULL){
    printf("%s,", ptr4->value);
    ptr4 = ptr4->next;
    }*/
  //FAKE RETURN TO TEST GET INPUT
  //return 0;

  if(valid == -1){
    printf("Fatal Error: Something went wrong reading. Errno: %d\n", errno);
    return -1;
  }
  else if(valid == 0){
    printf("Error: malloc returned a NULL pointer. Continuing with currently stored values.\n");
    if(quicksort){
      printf("Fatal Error: cannot allocate space for quicksort");
      return -1;
    }
  }
  
  else if(valid ==2){
    printf("Fatal Error: malloc returned a NULL pointer prior to accepting tokens. \n");
    return -1;
  }
  //Check if the inputs are integers or Strings. Call the appropriate sort
  int isNum = 0;
  /*if(linkedList->first == NULL){
    printf("Warning: File is empty\n");
    free(linkedList);
    return 0;
    }*/
  Node* ptr = linkedList->first;
  Node* prev= NULL;
  while(ptr->next!= NULL){
    prev = ptr;
    ptr = ptr->next;
  }
  if (*(ptr->value) == '\0'){
    if(prev == NULL){
      printf("Warning: File is empty\n");
      free(ptr->value);
      free(ptr);
      free(linkedList);
      close(fd);
      return 0;
    } else {
      prev->next = NULL;
    }
    free(ptr->value);
    free(ptr);
  }
  ptr = linkedList->first;
  while(ptr != NULL){
    if(*(ptr->value) == '\0')
      ptr = ptr->next;
    else
      break;
  }
  if (ptr ==NULL){
    printf("Warning: File is filled with empty tokens. Will treat as empty strings.\n");
  }else{
    isNum = isNumber(ptr->value);
  }

  ptr = linkedList->first;

  int (*comp) (void* p, void* q) = isNum? intComparator : stringComparator;
  quicksort? quickSort(linkedList, comp) : insertionSort(linkedList, comp);

  ptr = linkedList->first;
  //test counts
  //int count = 0;
  if (isNum){
    while(ptr !=NULL){
      printf("%d\n", atoi(ptr->value));
      ptr = ptr->next;
    }
  }
  else{
    while(ptr !=NULL){
      printf("%s\n", ptr->value);
      ptr = ptr->next;
    }
  }

  freeList(linkedList);
  return 0;
}

//Returns -1 on read error
//STILL IN PROGRESS: Returns 0 on malloc error (NULL malloc)
//Returns 1 on success
//Returns 2 on fatal malloc error
  
int getInput(LL* list, int fd){

  list->first = (Node*) malloc(sizeof(Node));
  if(list->first == NULL){
    return 2;
  }
  Node* ptr = list->first;
  Node* prev = NULL;

  ptr->value = (char*) malloc(1);
  if(ptr->value == NULL){
    free(list->first);
    list->first = NULL;
    return 2;
  }
  *(ptr->value) = '\0';
  
  int bytesRead = 1;
  int size = 0;
  char buffer[201];
  int i = 0;
  int broke = 0;

  
  do{
    bytesRead = read(fd, buffer, 200);
    if(bytesRead == -1) return -1;
    else if (bytesRead == 0){
      ptr->next == NULL;
      break;
    }
    buffer[bytesRead] = '\0';
    int startIndex = 0;
    for(i = 0; i<bytesRead; i++){
      if (buffer[i] == '\0') break;
      if (buffer[i] == ','){
	size = strlen(ptr->value);
	char* temp = (char*) malloc(i-startIndex+ size+1);
	if(temp == NULL){
	  free(ptr->value);
	  free(ptr);
	  prev->next = NULL;
	  broke = 1;
	  break;
	}
	memcpy(temp, ptr->value, size+1);
	free(ptr->value);
	ptr->value = temp;
	buffer[i] = '\0';
	strcat(ptr->value, buffer+startIndex);
	startIndex = i+1;
	ptr->next = (Node *) malloc(sizeof(Node));
	if(ptr->next == NULL){
	  broke =1;
	  break;
	}
	prev = ptr;
	ptr = ptr->next;
	ptr->value = (char*) malloc(1);
	if(ptr->value == NULL){
	  free(ptr);
	  prev->next == NULL;
	  broke =1;
	  break;
	}
	*(ptr->value) = '\0';
      }
    }
    if(broke) break;

    if(bytesRead == startIndex) continue;
    buffer[200] = '\0';
    size = strlen(ptr->value);
    char* temp = (char*) malloc(bytesRead-startIndex+ size+1);
    if(temp == NULL){
      free(ptr->value);
      free(ptr);
      prev->next = NULL;
      broke = 1;
      break;
    }
    memcpy(temp, ptr->value, size+1);
    free(ptr->value);
    ptr->value = temp;
    strcat(ptr->value, buffer+startIndex);
    ptr->next = NULL;
  }while(bytesRead>0);
  
  ptr = list->first;
  while(ptr != NULL){
    for(i =0; i < strlen(ptr->value); i++){
      if(isspace((ptr->value)[i])){
	char buff[strlen(ptr->value) - i];
	(ptr->value)[i] = '\0';
	strcpy(buff, ptr->value + i + 1);
	strcat(ptr->value, buff);
	i--;
      }
    }
    ptr = ptr->next;
  }

  if(broke == 1) return 0;
  return 1;

}


//isNumber & isString determines whether a char* is entirely numerical or alphabetical respectively
//Return 0 for false
//Return 1 for true
int isNumber(char *string){
  
  int i = 0;
  //  if(string == '/0') return -1;
  if(string[0] == '-' || isdigit(string[0])){
    i++;
  } else {
    return 0;
  }
  
  while(string[i] != '\0'){
    if(isdigit(string[i])) i++;
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
  Node* head = ((LL*) toSort)->first;
  Node* ptr = head;
  Node* prev = NULL;
  Node* start = head->next;
  Node* startPrev = head;
  while (start != NULL){
    int changed = 0;
    ptr = head;
    prev = NULL;
    while(ptr != start){
      if(comparator(start->value, ptr->value) == -1){
	//adding to head front
	if(prev == NULL){
	  startPrev->next = start->next;
	  start->next = ptr;
	  head = start;
	  start = startPrev->next;
	  changed = 1;
	  break;
	}
	//otherwise
	else{
	  startPrev->next = start->next;
	  prev->next = start;
	  start->next = ptr;
	  start= startPrev->next;
	  changed = 1;
	  break;
	}
      }
      else{
	prev = ptr;
	ptr = ptr->next;
      }
    }
    if(!changed){ 
      startPrev = start;
      start = start->next;
    }
  }
  ((LL*) toSort)->first = head;
  return 0;
}

int quickSort(void* toSort, int (*comparator)(void*, void*)){
  Node* toSort2 = ((LL*) toSort)->first;
  Node* pivot = toSort2;
  Node* head = toSort2;
 
  if (head == NULL){
    return 0;
  }
  if (pivot->next == NULL){

    return 0;
  }
  Node* beforePivot = NULL;
  Node* prev = pivot;
  Node* ptr = pivot->next;
 
  while (ptr != NULL){
    if(comparator(ptr->value, pivot->value) == -1){
      prev->next = ptr->next;
      Node* temp = head;
      if(beforePivot == NULL) beforePivot =ptr;
      head = ptr;
      head->next = temp;
      ptr = prev->next;

    }
    else{
      prev = ptr;
      ptr = ptr->next;

    }
  }
  ((LL*) toSort)->first = head;
  LL* right = (LL*) malloc(sizeof(LL));
  right->first = pivot->next;
  if(beforePivot != NULL) beforePivot->next = NULL;

  quickSort(right, comparator);

  if(beforePivot !=NULL) quickSort(toSort, comparator);

  ptr = ((LL*) toSort)->first;
  if(beforePivot != NULL){
    while(ptr->next != NULL){
      ptr = ptr->next;
    }
    ptr->next = pivot;
  }
  pivot->next = right->first;
  free(right);
  return 0;
}

void freeList(LL* list){

  Node* ptr = list->first, *temp;

  while(ptr !=NULL){
    free(ptr->value);
    temp = ptr->next;
    free(ptr);
    ptr=temp;
  }

  free(list);
  return;
}
