#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define OVERDRAFT_CHARGE 35
#define INTEREST_RATE .02

// Structs for easier data transfer/manipulation
typedef struct {
	int year;
	int month;
	int day;
} Date;

typedef struct {
	int id;
	char owner[20];
	double balance;
} Account;

typedef struct {
	Account array[20];
	int length;
	int count;
} AccountArray;

typedef struct {
	int id;
	int account;
	Date date;
	char type;
	double amount;
} Transaction;

typedef struct {
	Transaction array[20];
	int length;
	int count;
} TransactionArray;

// General functions
int display_menu();
void print_date(Date date, char s[]);
int compare_dates(Date a, Date b);
double round_dbl(double d, int i);

// Functions that operate on Accounts
void print_account(Account acct);

// Functions that operate on AccountArrays
void print_account_array(AccountArray acctArr);
void insert_account(AccountArray *acctArr, int id, char owner[], double balance);
void read_accounts(char fileName[], AccountArray *acctArr);
void swap_accounts(AccountArray *acctArr, int i, int j);
void sort_accounts(AccountArray *acctArr);
int search_account_id(AccountArray acctArr, int id);
int search_account_owner(AccountArray acctArr, char owner[]);
void save_accounts(AccountArray acctArr, char fileName[]);

// Functions that operate on Transactions
void print_transaction(Transaction trans);
int compare_transactions(Transaction a, Transaction b);

// Functions that operate on TransactionArrays
void print_transaction_array(TransactionArray transArr);
void insert_transaction(TransactionArray *transArr, int id, int account, char date[], char type, double amount);
void read_transactions(char fileName[], TransactionArray *transArr);
void swap_transactions(TransactionArray *transArr, int i, int j);
void sort_transactions(TransactionArray *transArr);
int find_first_transaction(TransactionArray transArr, int acctNo);
void sort_transactions_id(TransactionArray *transArr);
void save_transactions(TransactionArray transArr, char fileName[]);

// Functions that operate on AccountArrays and TransactionArrays
void reconcile(AccountArray *acctArr, TransactionArray *transArr);
void print_account_id(AccountArray acctArr, TransactionArray transArr, int acctNo);
void print_account_owner(AccountArray acctArr, TransactionArray transArr, char owner[]);
void print_account_transactions(AccountArray acctArr, TransactionArray transArr, int acctIdx);
void print_summary_report(AccountArray acctArr, TransactionArray transArr);
void print_detail_report(AccountArray acctArr, TransactionArray transArr);


int main() {
	AccountArray accounts;
	accounts.length = 20;
	accounts.count = 0;
	
	TransactionArray transactions;
	transactions.length = 20;
	transactions.count = 0;
	
	char acctFileName[50];
	char transFileName[50];
	int id = 0;
	char ownerName[50];
	
	int choice = 0;
	do {
		choice = display_menu();
		
		switch (choice) {
			case 1:
				printf("Enter account filename: ");
				scanf(" %s", acctFileName);
				printf("Enter transaction filename: ");
				scanf(" %s", transFileName);
				read_accounts(acctFileName, &accounts);
				read_transactions(transFileName, &transactions);
				sort_accounts(&accounts);
				sort_transactions(&transactions);
				printf("Files read and sorted.\n\n");
				break;
			case 2:
				printf("Reconciling accounts...\n");
				reconcile(&accounts, &transactions);
				printf("Reconciled accounts.\n\n");
				break;
			case 3:
				printf("Enter the ID: ");
				scanf("%d", &id);
				print_account_id(accounts, transactions, id);
				break;
			case 4:
				printf("Enter the owner's name: ");
				scanf("%s", ownerName);
				print_account_owner(accounts, transactions, ownerName);
				break;
			case 5:
				print_summary_report(accounts, transactions);
				break;
			case 6:
				print_detail_report(accounts, transactions);
				break;
			case 7:
				print_account_array(accounts);
				break;
			case 8:
				print_transaction_array(transactions);
				break;
			case 9:
				printf("Enter new account filename: ");
				scanf(" %s", acctFileName);
				printf("Enter new transaction filename: ");
				scanf(" %s", transFileName);
				save_accounts(accounts, acctFileName);
				sort_transactions_id(&transactions);
				save_transactions(transactions, transFileName);
				sort_transactions(&transactions);
				printf("Files saved.\n\n");
				break;
		}
	} while (choice > 0 && choice < 10);
	
	return 0;
}

/* General functions */

// Displays menu with options and returns selection as int 1-10
int display_menu() {
	int choice = 0;
	int res = 0;
	do {
		printf("1-Read and sort account and transaction files\n");
		printf("2-Reconcile accounts\n");
		printf("3-Find account by ID-Binary Search-List Transactions\n");
		printf("4-Find account by owner-Selection Search-List Transactions\n");
		printf("5-Print account summary report\n");
		printf("6-Print account detail report\n");
		printf("7-Print all accounts\n");
		printf("8-Print all transactions\n");
		printf("9-Save account and transactions to file\n");
		printf("10-Exit\n");
		printf("Enter a choice: ");
		// Ensures we got a numeric response
		res = scanf("%d", &choice);
	} while ((choice <= 0 || choice > 10) && res);
	
	return choice;
}

// Puts date in string form, saves in s
void print_date(Date date, char s[]) {
	sprintf(s, "%d/%d/%d", date.month, date.day, date.year);
}

// Returns -1 if a before b, 0 if equal, 1 if a after b
int compare_dates(Date a, Date b) {
	// I figured since we made our own Date struct, we shouldn't use
	// the C time library.
	if (a.year < b.year) return -1;
	if (a.year > a.year) return 1;
	if (a.month < b.month) return -1;
	if (a.month > b.month) return 1;
	if (a.day < b.day) return -1;
	if (a.day > b.day) return 1;
	return 0;
}

// Round a double d to i digits
double round_dbl(double d, int i){
	// Multiply d by 10^(i+1) and cast as int
	int n = (int)(d * pow(10, i+1));
	// Add 5 - will round up if last digit > 5
	n += 5;
	// Remove least significant digit
	n /= 10;
	// Cast n as double into d
	d = (double)n;
	// Divide d by 10^i
	d /= pow(10, i);
	// return d rounded to i digits
	return d;
	
}

/* Functions that operate on Accounts */

// Prints account information given struct. Blank line at end
void print_account(Account acct) {
	printf("ID: %d\n", acct.id);
	printf("Owner: %s\n", acct.owner);
	printf("Balance: %.2lf\n\n", acct.balance);
}

/* Functions that operate on AccountArrays */

// Prints all accounts using print_account()
void print_account_array(AccountArray acctArr) {
	int i=0;
	for (i=0; i<acctArr.count; i++) {
		print_account(acctArr.array[i]);
	}
}

// Inserts account into passed AccountArray struct given data
void insert_account(AccountArray *acctArr, int id, char owner[], double balance) {
	// We can't put more than the max amount of entries in the array
	if (acctArr->count >= acctArr->length) return;
	
	Account newAccount;
	newAccount.id = id;
	strcpy(newAccount.owner, owner);
	newAccount.balance = balance;
	
	// Stores/increments count
	acctArr->array[acctArr->count++] = newAccount;
}

// Reads a file and inserts accounts into passed AccountArray using insert_account
void read_accounts(char fileName[], AccountArray *acctArr) {
	char s[200];
	int id;
	char owner[50];
	double balance;
	printf("Reading file %s...\n", fileName);
	
	FILE *in_file = fopen(fileName, "r");
	
	// We got a file?
	if(in_file != NULL){
		// Read each line
		while(fgets(s, 200, in_file) != NULL){
			// Remove newline char from read line
			s[strlen(s)-1] = '\0';
			
			id = atoi(strtok(s, ","));
			// Subsequent tokenizations need not reference s
			strcpy(owner, strtok(NULL, ","));
			balance = atof(strtok(NULL, ","));
			
			insert_account(acctArr, id, owner, balance);
		}
		
		// Clean up
		fclose(in_file);
	}
}

// Swaps accounts at indexes--order doesn't matter
void swap_accounts(AccountArray *acctArr, int i, int j) {
	Account temp = (*acctArr).array[i];
	(*acctArr).array[i] = (*acctArr).array[j];
	(*acctArr).array[j] = temp;
}

// Bubble sort on account IDs
void sort_accounts(AccountArray *acctArr) {
	int i, j = 0;
	int count = acctArr->count;
	
	// Outer loop, loops through all values
	for (i=0;i<count;i++) {
		// Inner loop, loops from right starting at last index, going to i+1
		for (j=(count-1);j>i;j--) {
			// If rightmost value is less than the value to the left of it, swap them
			if (acctArr->array[j].id < acctArr->array[j-1].id) {
				// Swap them
				swap_accounts(acctArr, j, j-1);
			}
		}
	}
}

// Binary search for accounts by ID. acctArr should be sorted first
int search_account_id(AccountArray acctArr, int id) {
	int begin = 0;
	int count = acctArr.count;
	int end = count - 1;
	int mid = 0;	
	
	// Keep going until begin becomes equal to or greater than end
	while (begin < end) {
		// Middle is average of end and beginning
		mid = (end+begin)/2;
	
		// If the needle is greater than mid value, cut it in half to top half
		if (id > acctArr.array[mid].id)
			begin = mid + 1;
		// If it's less, cut in half to bottom half
		else if (id < acctArr.array[mid].id)
			end = mid - 1;
		// If it's equal, we've got our value
		else
			return mid;
	}
	
	// If we're here and haven't returned, needle isn't in array. Return -1
	return -1;
}

// Selection search for accounts by owner, acctArr need not be sorted
// This is case sensitive.
int search_account_owner(AccountArray acctArr, char owner[]) {
	int i;
	for(i=0; i<acctArr.count; i++)
		if(strcmp(acctArr.array[i].owner, owner) == 0) // Strings are equal
			return i;
	return -1;
}

// Saves accounts to file in CSV format
void save_accounts(AccountArray acctArr, char fileName[]) {
	printf("Writing file %s...\n", fileName);
	
	FILE *out_file = fopen(fileName, "w");
	int i;
	
	// File created?
	if(out_file != NULL){
		for(i=0; i<acctArr.count; i++) {
			fprintf(out_file, "%d,%s,%.2lf\n",
				acctArr.array[i].id, acctArr.array[i].owner,
				acctArr.array[i].balance);
		}
			
		// Clean up
		fclose(out_file);
	}
}

/* Functions that operate on Transactions */

// Pretty prints transaction and its info
void print_transaction(Transaction trans) {
	char date[11];
	print_date(trans.date, date);
	
	printf("ID: %d\n", trans.id);
	printf("Account: %d\n", trans.account);
	printf("Date: %s\n", date);
	printf("Type: %c\n", trans.type);
	printf("Amount: %.2lf\n\n", trans.amount);
}

// Sorts by accounts, then by dates if accounts are equal
int compare_transactions(Transaction a, Transaction b) {
	if (a.account > b.account) return 1;
	if (a.account < b.account) return -1;
	return compare_dates(a.date, b.date);
}

/* Functions that operate on TransactionArrays */

// Prints all transactions ordered by array index using print_transaction()
void print_transaction_array(TransactionArray transArr) {
	int i=0;
	for (i=0; i<transArr.count; i++) {
		print_transaction(transArr.array[i]);
	}
}

// Inserts transactions into transArr given additional info
void insert_transaction(TransactionArray *transArr, int id, int account, char date[], char type, double amount) {
	// Let's not overflow the array
	if (transArr->count >= transArr->length) return;
	
	// Neet to turn date string into real Date
	Date newDate;
	newDate.month = atoi(strtok(date, "/"));
	// Subsequent tokenizations need not reference original string
	newDate.day = atoi(strtok(NULL, "/"));
	newDate.year = atoi(strtok(NULL, "/"));
	
	Transaction newTransaction;
	newTransaction.id = id;
	newTransaction.account = account;
	newTransaction.date = newDate;
	newTransaction.type = type;
	newTransaction.amount = amount;
	
	// Adds and increments
	transArr->array[transArr->count++] = newTransaction;
}

// Reads transactions from CSV file, adds them to transArr using insert_transaction()
void read_transactions(char fileName[], TransactionArray *transArr) {
	char s[200];
	int id;
	int account;
	char date[11];
	char type;
	double amount;
	printf("Reading file %s...\n", fileName);
	
	FILE *in_file = fopen(fileName, "r");
	
	// Got a file?
	if(in_file != NULL){
		// Read each line
		while(fgets(s, 200, in_file) != NULL){
			// Remove newline char from read line
			s[strlen(s)-1] = '\0';
			
			id = atoi(strtok(s, ","));
			// Subsequent tokenizations need not reference s
			account = atoi(strtok(NULL, ","));
			strcpy(date, strtok(NULL, ","));
			type = strtok(NULL, ",")[0];
			amount = atof(strtok(NULL, ","));
			
			insert_transaction(transArr, id, account, date, type, amount);
		}
		
		// Clean up
		fclose(in_file);
	}
}

// Swaps transactions in transArr based on index
void swap_transactions(TransactionArray *transArr, int i, int j) {
	Transaction temp = (*transArr).array[i];
	(*transArr).array[i] = (*transArr).array[j];
	(*transArr).array[j] = temp;
}

// Bubble sort transactions based on compare algorithm used in compare_transactions()
void sort_transactions(TransactionArray *transArr) {
	int i, j = 0;
	int count = transArr->count;
	
	// Outer loop, loops through all values
	for (i=0;i<count;i++) {
		// Inner loop, loops from right starting at last index, going to i+1
		for (j=(count-1);j>i;j--) {
			// If rightmost value is less than the value to the left of it, swap them
			if (compare_transactions(transArr->array[j], transArr->array[j-1]) < 0) {
				// Swap them
				swap_transactions(transArr, j, j-1);
			}
		}
	}
}


// Finds first transaction for account ID in sorted transArr.
// After using this, may loop until account ID no longer matches if sorted
// using sort_transactions
int find_first_transaction(TransactionArray transArr, int acctNo) {
	int i;
	for(i=0; i<transArr.count; i++)
		if(acctNo == transArr.array[i].account)
			return i;
	return -1;
}

// Bubble sorts transactions by transaction IDs
void sort_transactions_id(TransactionArray *transArr) {
	int i, j = 0;
	int count = transArr->count;
	
	// Outer loop, loops through all values
	for (i=0;i<count;i++) {
		// Inner loop, loops from right starting at last index, going to i+1
		for (j=(count-1);j>i;j--) {
			// If rightmost value is less than the value to the left of it, swap them
			if (transArr->array[j].id < transArr->array[j-1].id) {
				// Swap them
				swap_transactions(transArr, j, j-1);
			}
		}
	}
}

// Saves transactions to file
void save_transactions(TransactionArray transArr, char fileName[]) {
	printf("Writing file %s...\n", fileName);
	
	FILE *out_file = fopen(fileName, "w");
	int i;
	
	// Made a file?
	if(out_file != NULL){
		for(i=0; i<transArr.count; i++) {
			fprintf(out_file, "%d,%d,%d/%d/%d,%c,%.2lf\n",
				transArr.array[i].id, transArr.array[i].account,
				transArr.array[i].date.month, transArr.array[i].date.day,
				transArr.array[i].date.year, transArr.array[i].type,
				transArr.array[i].amount);
		}
			
		// Clean up
		fclose(out_file);
	}
}

/* Functions that operate on AccountArrays and TransactionArrays */

// Reconcile accounts
void reconcile(AccountArray *acctArr, TransactionArray *transArr){
	// Indices for acctArr and transArr, and counter
	int acctIdx=0, transIdx=0, count=0;
	double balance = 0.0;	// Running balance for an account
	double interest = 0.0;	// Interest calculated for an account
	char date[20];			// To get date from user
	char date2[20];			// To copy date and insert into transArr
							// because strtok damages strings
	
	// Get today's date from user
	printf("Enter date (mm/dd/yyyy): ");
	scanf("%s", date);

	// acctIdx starts at 0 and ends at count-1
	while(acctIdx < (*acctArr).count){
		// Get balance from account
		balance = (*acctArr).array[acctIdx].balance;
		// Process transactions for this account
		while((*acctArr).array[acctIdx].id == (*transArr).array[transIdx].account){
			// Process a deposit
			if((*transArr).array[transIdx].type == 'd')
				balance += (*transArr).array[transIdx].amount;
			// Process a withdrawl
			else if((*transArr).array[transIdx].type == 'w')
				balance -= (*transArr).array[transIdx].amount;
			// If account has an overdraft - charge fee
			if(balance < 0.0){
				// Make a copy of today's date
				strcpy(date2, date);
				// Insert overdraft fee into transactions
				insert_transaction(transArr, (*transArr).count+1, (*acctArr).array[acctIdx].id,
									date2, 'o', OVERDRAFT_CHARGE);
				// Deduct overdraft from balance
				balance -= OVERDRAFT_CHARGE;
			}
			// Go to next transaction
			transIdx++;
			// Record that an account update has occurred
			count++;
			// If last transaction in array has been processed - break loop
			if(transIdx >= (*transArr).count)
				break;
		}
		// If an update occurred for this account
		if(count>0){
			// If balance is > zero - pay interest to account
			if(balance > 0.0){
				// Calc interest
				interest = round_dbl(balance*(INTEREST_RATE/12.0), 2);
				// Make a copy of today's date
				strcpy(date2, date);
				// Insert interest payment into transactions
				insert_transaction(transArr, (*transArr).count+1, 
						(*acctArr).array[acctIdx].id, date2, 'i', interest);
				// Update balance in account
				(*acctArr).array[acctIdx].balance = round_dbl(balance + interest, 2);
			}
			// No updates
			else
				// Update balance in account
				(*acctArr).array[acctIdx].balance = round_dbl(balance, 2);
		}
		// Go to next account
		acctIdx++;
		// Reset count of updates for account to zero
		count = 0;
	}
	// Sort transactions by account and date
	sort_transactions(transArr);
}

// Prints account transactions by account ID
void print_account_id(AccountArray acctArr, TransactionArray transArr, int acctNo) {
	int index = search_account_id(acctArr, acctNo);
	// index == -1 if not found
	if (index < 0) {
		printf("ID not found.\n\n");
		return;
	}
	print_account_transactions(acctArr, transArr, index);
}

// Prints account transactions by account Owner
void print_account_owner(AccountArray acctArr, TransactionArray transArr, char owner[]) {
	int index = search_account_owner(acctArr, owner);
	// index == -1 if not found
	if (index < 0) {
		printf("Owner not found. Note, this is case-sensitive.\n\n");
		return;
	}
	print_account_transactions(acctArr, transArr, index);
}

// Prints account transactions for account at acctIdx in acctArr
void print_account_transactions(AccountArray acctArr, TransactionArray transArr, int acctIdx) {
	print_account(acctArr.array[acctIdx]);
	
	int transIds[20];
	int transCount = 0;
	int i = 0;
	char date[11];
	
	// Let's find the first transaction and loop until the account ID no longer matches
	int transIdx = find_first_transaction(transArr, acctArr.array[acctIdx].id);
	while (transArr.array[transIdx].account == acctArr.array[acctIdx].id) {
		transIds[transCount++] = transIdx;
		transIdx++;
	}
	
	// We have a count now, and a list of transaction IDs
	
	printf("Account index: %d, # transactions: %d\n", acctIdx, transCount);
	// We need two tabs because of dates overflowing a single tab
	printf("ID\tDate\t\tType\tAmount\n");
	
	// Print all transactions
	for(i=0;i<transCount;i++) {
		print_date(transArr.array[transIds[i]].date, date);
		printf("%d\t%s\t%c\t%.2lf\n", transArr.array[transIds[i]].id,
			date, transArr.array[transIds[i]].type,
			transArr.array[transIds[i]].amount);
	}
	
	printf("\n");
}

// Prints basic report with account IDs, owner names and current balances
void print_summary_report(AccountArray acctArr, TransactionArray transArr) {
	printf("Account Summary Report\n");
	printf("ID\tOwner\tBalance\n");
	
	int i = 0;
	for(i=0;i<acctArr.count;i++) {
		printf("%d\t%s\t%.2lf\n", acctArr.array[i].id,
			acctArr.array[i].owner, acctArr.array[i].balance);
	}
	
	printf("\n");
}

// Prints detailed report with every transaction for every account
void print_detail_report(AccountArray acctArr, TransactionArray transArr) {
	printf("Account Detail Report\n");
	
	int i = 0;
	for(i=0;i<acctArr.count;i++) {
		print_account_transactions(acctArr, transArr, i);
	}
	
	printf("\n");
}