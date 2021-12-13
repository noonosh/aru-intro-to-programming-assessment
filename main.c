#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISCOUNT_MIN_AGE 18
#define DISCOUNT_MAX_AGE 25
#define DISCOUNT_PERCENTAGE 0.14f
#define MAX_SALES 20
#define CSV_FILE "data.csv"

#define MENU_BUY 'b'
#define MENU_STATS 's'
#define MENU_VIEW 'v'
#define EXIT_STORE 'q'
#define TRUE 1
#define FALSE 0
#define bool unsigned char

unsigned short carsInStock = 20;

unsigned short numberOfSales = 0;
unsigned short soldCarsPerSale[MAX_SALES];
unsigned short modelPerSale[MAX_SALES];
bool discountsPerSale[MAX_SALES];
char customerNames[MAX_SALES][201];

float carPrices[] = { 45000.00, 32000.00, 60000.00, 55000.00 };
char modelNames[][10] = { "Tesla", "Hyundai", "Toyota", "BMW" };


#define FILE_OPENED 0
#define FILE_CLOSED 1
#define FILE_ERROR 2

FILE* file;
unsigned char fileStatus = FILE_CLOSED;

#ifdef _WIN32
    char os[] = "WIN";
#elif __APPLE__
    char os[] = "MAC";
#endif

// Clearing the screen will work both on MacOS and Windows
void clearScreen(char os_type[]) {
    char win[] = "WIN";
    char mac[] = "MAC";
    
    (os_type == win) ? system("cls") : system("clear");
}

void pauseProgram(char userChoice) {
	if (userChoice == EXIT_STORE) {
		printf("\n\nPress Enter to Exit...");
	}
	else {
		printf("\n\nPress Enter to return to the Menu...");
	}

	if (userChoice != '_') {
		getchar();
	}

	getchar();
}

char getCharFromConsole(char message[201]) {
	char userInput;
	printf(message);
	scanf("\n%c", &userInput);
	return userInput;
}

unsigned short getUnsignedShortFromConsole(char message[201]) {
	unsigned short userInput;

	printf(message);
	scanf("%hd", &userInput);

	return userInput;
}

void getStringFromConsole(char message[201], char *str) {
	printf(message);
	scanf("\n%[^\n]s", str);
}


FILE *createFile(char fileName[201]) {
	file = fopen(fileName, "w");
	if (file != NULL) {
		fclose(file);
	}
	return file;
}

void openFile(char fileName[201], char mode[4]) {
	file = fopen(fileName, mode);

	if (file == NULL) {

		if (createFile(fileName) == NULL) {
			fileStatus = FILE_ERROR;
		}
		else {
			openFile(fileName, mode);
		}

	}
	else {
		fileStatus = FILE_OPENED;
	}
}

void closeFile() {
	if (fileStatus == FILE_OPENED) {
		fclose(file);
		fileStatus = FILE_CLOSED;
	}
}

void readDataFromFile() {
	int lineCounter = 0;

	while (1) {

		unsigned short soldCarsPerSaleValue = 0, modelPerSaleValue = 0, discountsPerSaleValue = 0;
		char customerNameValue[201] = "";

		int scanResult = fscanf(
			file, 
			"%hd,%hd,%hd,%[^\n]s",
			&soldCarsPerSaleValue,
			&modelPerSaleValue,
			&discountsPerSaleValue,
			&customerNameValue
		);

		if (scanResult == EOF) {
			break;
		}

		soldCarsPerSale[lineCounter] = soldCarsPerSaleValue;
		modelPerSale[lineCounter] = modelPerSaleValue;
		discountsPerSale[lineCounter] = (bool)discountsPerSaleValue;
		strcpy(customerNames[lineCounter], customerNameValue);

		carsInStock -= soldCarsPerSaleValue;

		lineCounter++;
	}

	numberOfSales = lineCounter;
}

void getDataFromFile() {
	openFile(CSV_FILE, "r");

	if (fileStatus == FILE_OPENED) {
		readDataFromFile();
	}
	else if (fileStatus == FILE_ERROR) {
		printf("There was an error trying to read from the file %s.", CSV_FILE);
		pauseProgram('_');
	}

	closeFile();
}

void writeDataToFile() {
	for (int i = 0; i < numberOfSales; i++) {

		char line[201];
		char data[50];

		sprintf((int)soldCarsPerSale[i], data, 10);
		strcpy(line, data);
		strcat(line, ",");

		sprintf((int)modelPerSale[i], data, 10);
		strcat(line, data);
		strcat(line, ",");

		sprintf((int)discountsPerSale[i], data, 10);
		strcat(line, data);
		strcat(line, ",");

		strcat(line, customerNames[i]);

		fprintf(file, line);
		if (i < numberOfSales - 1) {
			fprintf(file, "\n");
		}
	}
}

void saveDataToFile() {
	openFile(CSV_FILE, "w");

	if (fileStatus == FILE_OPENED) {
		writeDataToFile();
	}
	else if (fileStatus == FILE_ERROR) {
		printf("There was an error trying to write to the file %s.", CSV_FILE);
		pauseProgram('_');
	}

	closeFile();
}


void greetUser() {
	printf("Welcome to the world of \"Epic Cars\"!\n\n");
}

void displayMenu(void) {
    printf("Menu:\n");
    printf("%c. Buy a car\n", MENU_BUY);
    printf("%c. See statistics for sales\n", MENU_STATS);
    printf("%c. Exit the store\n\n", EXIT_STORE);
    
}

void menu_showModelNames() {
	int numOfCars = sizeof(carPrices) / sizeof(float);

	printf("\nCar models:\n");

	for (int i = 0; i < numOfCars; i++) {
		printf("%d - %s\n", i, modelNames[i]);
	}
}

float menu_applyDiscount(float currentPrice) {
	return currentPrice * (1 - DISCOUNT_PERCENTAGE);
}

unsigned short menu_checkIfDiscountIsNeeded(unsigned short age) {
	if (age >= DISCOUNT_MIN_AGE && age <= DISCOUNT_MAX_AGE) {
		return TRUE;
	}
	else {
		char nusCardResponse = getCharFromConsole("\nDo you have an Epic Cars' 20%% gift wowcher? Answer 'y' or 'n' : ");

		if (nusCardResponse == 'y') {
			return TRUE;
		}
	}

	return FALSE;
}

void menu_printDiscountOutcome(bool giveDiscount) {
	switch (giveDiscount) {
		case TRUE:
			printf("\nYou get a discount!");
			break;
		case FALSE:
			printf("\nNo discount given.");
			break;
	}
}

void menu_buyCars() {
	printf("Buy a car:\n\n");

	if (carsInStock > 0) {
        if (carsInStock == 1)
        printf(printf("There is 1 car available.\n\n"));
		printf("There are %hd cars available.\n\n", carsInStock);
	}
	else {
		printf("Sorry, there are no more cars left in stock.");
		return;
	}

	unsigned short carsToBuy = 0, age = 0, carModel;
	bool giveDiscount = FALSE;
	float totalPrice = 0;

	getStringFromConsole("What is your name? Name: ", &customerNames[numberOfSales]);

	carsToBuy = getUnsignedShortFromConsole("How many cars do you want to purchase? Enter a number: ");

	if (carsInStock < carsToBuy) {
		printf("Sorry, there are fewer cars left than you need.");
		return;
	}

	soldCarsPerSale[numberOfSales] = carsToBuy;

	menu_showModelNames();

	carModel = getUnsignedShortFromConsole("\nWhich car do you want? Choose from above: ");

	modelPerSale[numberOfSales] = carModel;

	totalPrice = carsToBuy * carPrices[carModel];

	carsInStock -= carsToBuy;

	age = getUnsignedShortFromConsole("How old are you? Age: ");

	giveDiscount = menu_checkIfDiscountIsNeeded(age);

	if (giveDiscount == TRUE) {
		totalPrice = menu_applyDiscount(totalPrice);
	}

	discountsPerSale[numberOfSales] = giveDiscount;

	menu_printDiscountOutcome(giveDiscount);

	printf("\n\nThank you.\n");
	printf("You have bought %hd cars.\n", carsToBuy);
	printf("Total cost is %.2f GBP.\n", totalPrice);
	printf("\nThere are %hd cars left in stock.", carsInStock);

	numberOfSales++;
}

void menu_viewSales() {
	float totalSalesValue = 0;
	unsigned int soldCars = 0;

	printf("All Sales Data:\n\n");

	for (int i = 0; i < numberOfSales; i++) {

		int carModel = modelPerSale[i];
		float price = soldCarsPerSale[i] * carPrices[carModel];

		char discountGivenText[4];
		if (discountsPerSale[i] == TRUE) {
			strcpy(discountGivenText, "Yes");
			price *= (1 - DISCOUNT_PERCENTAGE);
		}
		else {
			strcpy(discountGivenText, "No");
		}

		printf("Sale Index: %d | Sale Amount: %f | Car model: %s | "
			   "Car Price: %.2f | Number of Cars: %hd | "
			   "Discount Given: %s | Customer Name: %s\n",
			   i, price, modelNames[carModel],
			   carPrices[carModel], soldCarsPerSale[i],
			   discountGivenText, customerNames[i]);

		totalSalesValue += price;
		soldCars += soldCarsPerSale[i];

	}

	printf("\n%hd cars have been sold with a total value of %.2f GBP. There are %hd cars unsold.\n",
		   soldCars, totalSalesValue, carsInStock);
}


void exitStore() {
   printf("Thank you for visiting Epic Cars! See you around in a soon time, and Merry Christmas! ;)");
   saveDataToFile();
}

int main(void) {

	getDataFromFile();

	char userInput;

	do {
		clearScreen(os);

		greetUser();
		displayMenu();

		userInput = getCharFromConsole("Please choose one: ");

		clearScreen(os);

		switch (userInput) {
			case MENU_BUY:
				menu_buyCars();
				break;

			case MENU_STATS:
				menu_viewSales();
				break;

			case EXIT_STORE:
				exitStore();
				break;
            default:
                printf("\nWarning!\nPlease, choose a valid option.");
		}

		pauseProgram(userInput);

	} while (userInput != EXIT_STORE);

	clearScreen(os);

	printf("Bye! Have a nice day.\n\n");
    
    return 0;
}