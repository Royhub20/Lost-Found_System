#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For malloc, free
#include <ctype.h>  // For isspace

#define FILENAME "transactions.txt"

// Node structure for a singly linked list
typedef struct Transaction {
    int id;
    char payer[20];
    float amount_paid;
    struct Transaction *next; // Pointer to the next node
} Transaction;

Transaction *head = NULL; // Head of the linked list
int count = 0;           // Number of transactions
float balance = 0.0;
float creditLimit = 0.0;
int next_id = 1;         // To ensure unique, sequential IDs for new transactions

// Helper function to create a new transaction node
Transaction* createTransactionNode(int id, const char* payer, float amount_paid) {
    Transaction *newNode = (Transaction*)malloc(sizeof(Transaction));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    newNode->id = id;
    strncpy(newNode->payer, payer, sizeof(newNode->payer) - 1);
    newNode->payer[sizeof(newNode->payer) - 1] = '\0'; // Ensure null-termination
    newNode->amount_paid = amount_paid;
    newNode->next = NULL;
    return newNode;
}

void load() {
    FILE *f = fopen(FILENAME, "r");
    if (f == NULL) {
        printf("File \"%s\" not found. Starting with empty records.\n", FILENAME);
        balance = 0.0; // Ensure balance is 0 if file not found
        creditLimit = 0.0; // Ensure creditLimit is 0 if file not found
        next_id = 1;
        return;
    }

    char line[100];
    int balance_loaded = 0; // Flag to check if balance was explicitly loaded
    int cl_loaded = 0;      // Flag to check if creditLimit was explicitly loaded

    // Read Balance
    if (fgets(line, sizeof(line), f) != NULL) {
        if (strncmp(line, "Balance:",8) == 0) {
            if (sscanf(line + 8, "%f", &balance) == 1) {
                balance_loaded = 1;
            } else {
                printf("Error loading balance from file. Setting balance to 0.0\n");
                balance = 0.0;
            }
        }
    }
    // Read CreditLimit
    if (fgets(line, sizeof(line), f) != NULL) {
        if (strncmp(line, "CreditLimit:", 12) == 0) {
            if (sscanf(line + 12, "%f", &creditLimit) == 1) {
                cl_loaded = 1;
            } else {
                printf("Error loading credit limit from file. Setting credit limit to 0.0\n");
                creditLimit = 0.0;
            }
        }
    }

    int loaded_id;
    char loaded_payer[20];
    float loaded_amount_paid;
    
    // Clear existing list before loading (if for some reason called multiple times)
    Transaction *current = head;
    while(current != NULL) {
        Transaction *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    count = 0;
    next_id = 1; // Reset next_id for new transactions

    int transactions_found = 0; // Flag to check if any transactions were found
    while (fscanf(f, "%d,%[^,],%f\n", &loaded_id, loaded_payer, &loaded_amount_paid) == 3) {
        transactions_found = 1;
        Transaction *newNode = createTransactionNode(loaded_id, loaded_payer, loaded_amount_paid);
        if (head == NULL) {
            head = newNode;
        } else {
            current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newNode;
        }
        count++;
        if (loaded_id >= next_id) { // Keep track of the highest ID for new transactions
            next_id = loaded_id + 1;
        }
    }
    
    // FIX: If no transactions were loaded, ensure balance and creditLimit are reset to 0
    // unless they were explicitly read from the file.
    // This helps if the file has "Balance:X" but no transactions after it.
    if (!transactions_found) {
        if (!balance_loaded) {
            balance = 0.0;
        }
        if (!cl_loaded) {
            creditLimit = 0.0;
        }
        next_id = 1; // If no transactions, start ID from 1
    }
    
    fclose(f);
}

void save() {
    FILE *f = fopen(FILENAME, "w");
    if (f == NULL) {
        printf("Error opening file \"%s\" for saving.\n", FILENAME);
        return;
    }
    fprintf(f, "Balance:%.2f\n", balance);
    fprintf(f, "CreditLimit:%.2f\n", creditLimit);
    
    Transaction *current = head;
    while (current != NULL) {
        fprintf(f, "%d,%s,%.2f\n", current->id, current->payer, current->amount_paid);
        current = current->next;
    }
    fclose(f);
}

// Clears the entire linked list and frees memory
void clearTransactions() {
    Transaction *current = head;
    while (current != NULL) {
        Transaction *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    count = 0;
    next_id = 1; // Reset next_id
}

float getValidFloatInput(const char* prompt) {
    char input[50];
    float value;
    while (1) {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) != NULL) {
            char *endptr;
            value = strtof(input, &endptr);
            if (endptr == input || (*endptr != '\0' && *endptr != '\n' && !isspace(*endptr))) {
                printf("Invalid input. Please enter a number.\n");
            } else {
                return value;
            }
        } else {
            printf("Error reading input.\n");
            return 0.0;
        }
    }
}

void newTransaction() {
    float availableCredit = creditLimit - balance;
    printf("\n----------------------------------------------------\n");
    printf("|                  New Transaction                   |\n");
    printf("----------------------------------------------------\n");
    
    char payer_name[20];
    printf("Payer: ");
    if (fgets(payer_name, sizeof(payer_name), stdin) == NULL) {
        printf("Error reading payer name.\n");
        return;
    }
    payer_name[strcspn(payer_name, "\n")] = 0;

    printf("Current balance: %.2f\n", balance);
    printf("Credit Limit: %.2f\n", creditLimit);
    printf("Available Credit: %.2f\n", availableCredit);

    float amountPaid;
    printf("Amount you want to pay: ");
    if (scanf("%f", &amountPaid) != 1) {
        printf("Invalid input for amount.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (amountPaid > availableCredit) {
        printf("Error: Amount exceeds available credit.\n");
        return;
    }

    balance += amountPaid;

    float adjustedBalance;
    printf("Calculated remaining balance: %.2f\n", balance);
    printf("Enter the actual remaining balance : ");

    if (scanf("%f", &adjustedBalance) == 1) {
        balance = adjustedBalance;
    } else {
        printf("Invalid input. Using calculated balance.\n");
        while (getchar() != '\n' && getchar() != EOF);
    }
    while (getchar() != '\n');

    // Create and add new node to linked list (Insertion Algorithm)
    Transaction *newNode = createTransactionNode(next_id++, payer_name, amountPaid);
    if (head == NULL) {
        head = newNode;
    } else {
        Transaction *current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
    count++;
    printf("Remaining balance: %.2f\n", balance);
}

void view() {
    printf("\n----------------------------------------------------\n");
    printf("|                Transaction History                 |\n");
    printf("----------------------------------------------------\n");
    printf("Remaining balance: %.2f\n", balance);
    printf("Credit Limit: %.2f\n", creditLimit);
    printf("Available Credit: %.2f\n", creditLimit - balance);

    if (head == NULL) { // Check if list is empty
        printf("No transactions recorded.\n");
        return;
    }

    printf("----------------------------------------------------\n");
    printf("| ID  | Payer               | Amount Paid |\n");
    printf("----------------------------------------------------\n");

    Transaction *current = head; // Start traversal from head (Traversal Algorithm)
    while (current != NULL) {
        printf("| %-3d | %-19s | %-11.2f |\n",
               current->id, current->payer, current->amount_paid);
        current = current->next;
    }

    printf("----------------------------------------------------\n");
}

void removeTransaction() {
    printf("\n----------------------------------------------------\n");
    printf("|                Remove Transaction                  |\n");
    printf("----------------------------------------------------\n");
    int id;
    printf("ID to delete: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input for ID.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    Transaction *current = head;
    Transaction *prev = NULL;
    int found = 0;

    // Deletion Algorithm for Linked List
    while (current != NULL) {
        if (current->id == id) {
            balance -= current->amount_paid; // Adjust balance
            if (prev == NULL) { // Node to be deleted is the head
                head = current->next;
            } else { // Node to be deleted is not the head
                prev->next = current->next;
            }
            free(current); // Free memory of the deleted node
            count--;
            found = 1;
            printf("Deleted. Remaining balance: %.2f\n", balance);
            
            // Re-ID all transactions to maintain sequential order (Linear Search and Update)
            Transaction *re_id_current = head;
            int temp_id = 1;
            while(re_id_current != NULL) {
                re_id_current->id = temp_id++;
                re_id_current = re_id_current->next;
            }
            next_id = temp_id; // Update next_id for new transactions
            
            // FIX: If the list is now empty after deletion, reset balance to 0
            if (head == NULL) {
                balance = 0.0;
                printf("All transactions removed. Balance reset to 0.00\n");
            }

            break; // Exit loop once transaction is found and removed
        }
        prev = current;
        current = current->next;
    }

    if (!found) {
        printf("ID not found.\n");
    }
    printf("Remaining balance: %.2f\n", balance);
}

// Bubble Sort for Linked List (by ID)
void sortTransactionsByID() {
    if (head == NULL || head->next == NULL) {
        if (head == NULL) {
            printf("No transactions to sort.\n");
        } else {
            printf("Only one transaction, no need to sort.\n");
        }
        return;
    }

    int swapped;
    Transaction *ptr1;
    Transaction *lptr = NULL; // Pointer to the last element of the sorted list

    // Implement Bubble Sort for linked list by swapping data within nodes
    do {
        swapped = 0;
        ptr1 = head;

        while (ptr1->next != lptr) {
            if (ptr1->id > ptr1->next->id) { // Compare by ID
                // Swap data (id, payer, amount_paid)
                int temp_id = ptr1->id;
                char temp_payer[sizeof(ptr1->payer)];
                float temp_amount = ptr1->amount_paid;
                
                strncpy(temp_payer, ptr1->payer, sizeof(ptr1->payer));

                ptr1->id = ptr1->next->id;
                strncpy(ptr1->payer, ptr1->next->payer, sizeof(ptr1->payer));
                ptr1->amount_paid = ptr1->next->amount_paid;

                ptr1->next->id = temp_id;
                strncpy(ptr1->next->payer, temp_payer, sizeof(ptr1->next->payer));
                ptr1->next->amount_paid = temp_amount;

                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1; // After each pass, the last element is in its correct place
    } while (swapped);
    printf("Transactions sorted by ID.\n");
}

int main() {
    load();
    printf("Welcome Baby  ??");

    printf("\n----------------------------------------------------\n");
    printf("|       Credit Card Payment Tracker                 |\n");
    printf("----------------------------------------------------\n");
    
    // Only prompt for creditLimit and balance if they were NOT loaded or are 0
    if (creditLimit == 0.0 && balance == 0.0 && head == NULL) { // Check if nothing was loaded or if balance/CL are still 0
        creditLimit = getValidFloatInput("Enter your credit limit: ");
        balance = getValidFloatInput("Enter your initial outstanding balance: ");
    } else {
        printf("Loaded Credit Limit: %.2f\n", creditLimit);
        printf("Loaded Balance: %.2f\n", balance);
    }
    
    int choice;
    do {
        printf("\n----------------------------------------------------\n");
        printf("|                     Main Menu                    |\n");
        printf("----------------------------------------------------\n");
        printf("1. New transaction\n");
        printf("2. View transactions\n");
        printf("3. Delete record\n");
        printf("4. Sort transactions by ID\n");
        printf("5. Exit\n");
        printf("Choice: ");
 
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            choice = -1;
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1: newTransaction(); break;
            case 2: view(); break;
            case 3: removeTransaction(); break;
            case 4: sortTransactionsByID(); break;
            case 5:
                save(); 
                clearTransactions(); // Free all allocated memory before exiting
               
                exit(0);
            default: printf("Invalid option! Please enter a number between 1 and 5.\n");
        }
    } while (1);

    return 0;
}