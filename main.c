#include <stdio.h>
#include <string.h>
#include "src/GetData.h"

#define API_ENDPOINT "https://anglia.nuriddin.me/api/cars/"


#define MENU_BUY
#define MENU_STATS
#define MENU_SEE
#define EXIT_STORE


struct Car {
    char model_name[100];
    float price;
    char description[];
};

 

int main(void) {
    
    // struct Car Car1;
    
    // strcpy(Car1.model_name, "Tesla");
    
    // char response[] = (char) curl_download_json_to_buffer(API_ENDPOINT);
    
    // for (int i = 0; i < 100; i++) {
    //     printf("%s", response[i]);
    // }
    
    printf("%s", curl_download_json_to_buffer(API_ENDPOINT));
    
    
    return 0;
}