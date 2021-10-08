#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

int tempArray[3];
FILE* total, *core1, *core2;
//底下這程式碼會印出所有CPU的溫度
int printTemp() {
    int eatReturn;
    
    
    //根據我以前的測試，這個檔案必須重新開啟作業系統才會更新裡面的數值
    total = fopen("/sys/class/hwmon/hwmon6/temp1_input", "r");
    assert(total != NULL);
    core1 = fopen("/sys/class/hwmon/hwmon6/temp2_input", "r");
    assert(core1 != NULL);
    core2 = fopen("/sys/class/hwmon/hwmon6/temp3_input", "r");
    assert(core2 != NULL);

    for (int i = 0; i < 3; i++)
        tempArray[i] = -1;
    fseek(total, 0, SEEK_SET);
    //eatReturn是明確的把「回傳值」吃掉，讓編譯器不要出現警告訊息
    eatReturn = fscanf(total, "%d", &tempArray[0]);
    fseek(core1, 0, SEEK_SET);
    eatReturn = fscanf(core1, "%d", &tempArray[1]);
    fseek(core2, 0, SEEK_SET);
    eatReturn = fscanf(core2, "%d", &tempArray[2]);
    
    //根據我以前的測試，這個檔案必須重新開啟作業系統才會更新裡面的數值
    //先關掉，等一下才可以重新開啟，或許可以用reopen
    fclose(total); fclose(core1); fclose(core2);

    int avgTemp = 0;
    //計算平均溫度
    for (int i = 0; i < 3; i++)
        avgTemp += tempArray[i];
    avgTemp /= 3;
    //回傳平均溫度
    return avgTemp;
}

int main(int argc, char **argv) {
    //預設值等到CPU的溫度降到35度，temperature是目標溫度
    int temperature = 35; 
    //使用者可以在參數列輸入溫度
    if (argc == 2) {
        sscanf(argv[1], "%d", &temperature);
    }
    fprintf(stderr, "target = %d\n", temperature);
    //授課老師自己稍微看一下，這個溫度要乘上1000才是真正的溫度
    temperature = temperature * 1000;

    //一個迴圈，每隔一秒讀取CPU的溫度，直到溫度比設定的還要低
    int temp;
    int try = 1;
    while((temp = printTemp()) > temperature) {
        fprintf(stderr, "#%02d sec 💥CPU temperature\t=\t%.2f ℃\n", try, (float)tempArray[0]/1000);
        fprintf(stderr, "        🔥Core1 temperature\t=\t%.2f ℃\n", (float)tempArray[1]/1000);
        fprintf(stderr, "        💥Core2 temperature\t=\t%.2f ℃\n", (float)tempArray[2]/1000);
        fprintf(stderr, "        🔥Avg temperature\t=\t%.2f ℃\n\n", ((float)temp)/1000);

        try++;
        //不要跑一個空的迴圈，這樣溫度會不斷的往上升，因此呼叫sleep暫停這個應用程式一秒鐘
        sleep(1);
    }
    printf("temperature %.2f℃\n", ((float)temp)/1000);
    exit(0);
}