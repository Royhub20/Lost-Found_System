/*
CBSUA - CC104 FINAL PROJECT
SYSTEM: LOST AND FOUND SYSTEM
LANGUAGE: C (Dev-C++ Compatible)
DSA: ARRAY | SINGLY LINKED LIST | HASH TABLE
ALGORITHMS: MERGE SORT | BINARY SEARCH
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --------------------------
// DEFINITIONS
// --------------------------
#define MAX 100
#define HASH_SIZE 37

// --------------------------
// DATA STRUCTURES (PROPER DECLARATION)
// --------------------------

// 1. ITEM RECORD
typedef struct {
    char id[15];
    char desc[100];
    char loc[50];
    char date[15];
    char stat[15]; // LOST / FOUND / CLAIMED
} Item;

Item itemList[MAX];
int count = 0;

// 2. LINKED LIST FOR CLAIMS
typedef struct ClaimNode{
    char id[15];
    char name[50];
    char course[30];
    struct ClaimNode *next;
} ClaimNode;

ClaimNode *claimHead = NULL;

// 3. HASH TABLE STRUCTURE
typedef struct HashNode{
    char id[15];
    Item *data;
    struct HashNode *next;
} HashNode;

HashNode *hashTable[HASH_SIZE] = {NULL};

// --------------------------
// UTILITY FUNCTION
// --------------------------
int hashKey(const char *key){
    int sum = 0;
    int i=0;
    while(key[i] != '\0'){
        sum += key[i];
        i++;
    }
    return sum % HASH_SIZE;
}

// --------------------------
// ALGORITHMS
// --------------------------

// MERGE SORT
void merge(Item arr[], int l, int m, int r){
    int i,j,k;
    int n1 = m - l + 1;
    int n2 = r - m;

    Item L[50], R[50];

    for(i=0; i<n1; i++) L[i] = arr[l+i];
    for(j=0; j<n2; j++) R[j] = arr[m+1+j];

    i=0; j=0; k=l;
    while(i<n1 && j<n2){
        if(strcmp(L[i].date, R[j].date) <= 0){
            arr[k] = L[i];
            i++;
        }else{
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while(i<n1){ arr[k] = L[i]; i++; k++; }
    while(j<n2){ arr[k] = R[j]; j++; k++; }
}

void mergeSort(Item arr[], int l, int r){
    if(l < r){
        int mid = l + (r-l)/2;
        mergeSort(arr, l, mid);
        mergeSort(arr, mid+1, r);
        merge(arr, l, mid, r);
    }
}

// BINARY SEARCH
int binarySearch(Item arr[], int low, int high, const char *key){
    while(low <= high){
        int mid = low + (high - low)/2;
        if(strcmp(arr[mid].id, key) == 0)
            return mid;
        if(strcmp(arr[mid].id, key) < 0)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

// --------------------------
// SYSTEM FUNCTIONS
// --------------------------

// HASH TABLE OPERATIONS
void insertHash(Item *prod){
    int index = hashKey(prod->id);

    // ? FIXED MALLOC SYNTAX FOR DEV-C++
    HashNode *newNode = (HashNode*) malloc(sizeof(HashNode));

    strcpy(newNode->id, prod->id);
    newNode->data = prod;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

Item* searchHash(const char *id){
    int index = hashKey(id);
    HashNode *temp = hashTable[index];

    while(temp != NULL){
        if(strcmp(temp->id, id) == 0)
            return temp->data;
        temp = temp->next;
    }
    return NULL;
}

// LINKED LIST OPERATION
void addClaim(const char *id, const char *name, const char *course){
    ClaimNode *newClaim = (ClaimNode*) malloc(sizeof(ClaimNode));

    strcpy(newClaim->id, id);
    strcpy(newClaim->name, name);
    strcpy(newClaim->course, course);
    newClaim->next = claimHead;
    claimHead = newClaim;
}

// MAIN FEATURES
void addItem(const char *type){
    if(count >= MAX){
        printf("\n[ERROR] Database FULL!\n");
        return;
    }

    Item newItem;
    printf("\nEnter Item ID (eg. ITM001): ");
    scanf("%s", newItem.id);

    if(searchHash(newItem.id) != NULL){
        printf("[WARNING] ID Already Exists!\n");
        return;
    }

    printf("Description: ");
    scanf(" %[^\n]", newItem.desc);
    printf("Location: ");
    scanf(" %[^\n]", newItem.loc);
    printf("Date (DD/MM/YYYY): ");
    scanf("%s", newItem.date);
    strcpy(newItem.stat, type);

    itemList[count] = newItem;
    insertHash(&itemList[count]);
    count++;

    printf("[SUCCESS] %s Item Added!\n", type);
}

void viewAll(){
    if(count == 0){
        printf("\n[INFO] No Records Found!\n");
        return;
    }

    printf("\n=============================================================\n");
    printf("%-10s | %-20s | %-15s | %-10s\n", "ID", "DESCRIPTION", "LOCATION", "STATUS");
    printf("=============================================================\n");

    for(int i=0; i<count; i++){
        printf("%-10s | %-20s | %-15s | %-10s\n",
               itemList[i].id,
               itemList[i].desc,
               itemList[i].loc,
               itemList[i].stat);
    }
}

void claimItem(){
    char id[15], name[50], course[30];
    printf("\nEnter Item ID to Claim: ");
    scanf("%s", id);

    Item *found = searchHash(id);
    if(found == NULL){
        printf("[ERROR] Item NOT Found!\n");
        return;
    }

    if(strcmp(found->stat, "FOUND") != 0){
        printf("[WARNING] Item not available for Claim!\n");
        return;
    }

    printf("Enter Your Name: ");
    scanf(" %[^\n]", name);
    printf("Enter Course/Year: ");
    scanf(" %[^\n]", course);

    strcpy(found->stat, "CLAIMED");
    addClaim(id, name, course);
    printf("[SUCCESS] Claim Registered!\n");
}

void viewClaims(){
    if(claimHead == NULL){
        printf("\n[INFO] No Pending Claims!\n");
        return;
    }

    printf("\n===== LIST OF CLAIMS =====\n");
    ClaimNode *temp = claimHead;
    while(temp != NULL){
        printf("Item: %s | Claimant: %s (%s)\n",
               temp->id, temp->name, temp->course);
        temp = temp->next;
    }
}

// --------------------------
// MAIN MENU
// --------------------------
int main(){
    int choice;

    printf("=================================================\n");
    printf("   CBSUA - LOST & FOUND SYSTEM\n");
    printf("   CC104 - DATA STRUCTURES & ALGORITHMS\n");
    printf("=================================================\n");

    do{
        printf("\n[1] Report LOST Item");
        printf("\n[2] Report FOUND Item");
        printf("\n[3] View All Items");
        printf("\n[4] Sort by Date (Merge Sort)");
        printf("\n[5] Search by ID (Binary Search)");
        printf("\n[6] Claim Item");
        printf("\n[7] View Claim Requests");
        printf("\n[0] EXIT");
        printf("\nEnter Choice: ");
        scanf("%d", &choice);

        switch(choice){
            case 1: addItem("LOST"); break;
            case 2: addItem("FOUND"); break;
            case 3: viewAll(); break;
            case 4:
                mergeSort(itemList, 0, count-1);
                printf("\n[SORTED] Items arranged by Date!\n");
                break;
            case 5:{
                char key[15];
                printf("\nEnter ID to Search: ");
                scanf("%s", key);
                mergeSort(itemList, 0, count-1);
                int res = binarySearch(itemList, 0, count-1, key);

                if(res == -1) printf("[NOT FOUND] Item does not exist!\n");
                else printf("[FOUND] %s | Status: %s\n", itemList[res].desc, itemList[res].stat);
                break;
            }
            case 6: claimItem(); break;
            case 7: viewClaims(); break;
            case 0: printf("\nSystem Closed. Thank You!\n"); break;
            default: printf("[INVALID] Choice not available!\n");
        }

    }while(choice != 0);

    return 0;
}