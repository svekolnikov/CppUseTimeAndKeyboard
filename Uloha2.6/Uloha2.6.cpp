#include <iostream>
#include <time.h>
#include <windows.h>
#include <stdio.h>
using namespace std;

#define STATE_X         11
#define STATE_Y         3
#define RANDOM_KEY_X    20
#define RANDOM_KEY_Y    6
#define TIME_X          RANDOM_KEY_X
#define TIME_Y          7
#define PRESSED_KEY_X   RANDOM_KEY_X
#define PRESSED_KEY_Y   8


struct measure_time_t {
    clock_t start = 0;
    clock_t end = 0;
    float delta = 0;
};

void PrintScuffold() 
{
    cout << "******************************\n";
    cout << "*  For start press F2        *\n";
    cout << "*  For stop press F10 or Esc *\n";
    cout << "*  State:                    *\n";
    cout << "******************************\n";
    cout << "*                            *\n";
    cout << "*  Random key:               *\n";
    cout << "*  Elapsed time:             *\n";
    cout << "*  Pressed key:              *\n";
    cout << "*                            *\n";
    cout << "******************************\n";
}                                        
void SetStatusToTerminal(HANDLE hConsole, int state)
{
    COORD p = { STATE_X, STATE_Y };
    SetConsoleCursorPosition(hConsole, p);

    if (state == 0)
    {
        printf("Stopped");
    }
    else if (state == 1)
    {
        printf("Run    ");
    }     
}
void SetKeyToTerminal(HANDLE hConsole, int vkey, int x, int y)
{
    COORD p = { x, y };
    SetConsoleCursorPosition(hConsole, p);

    if (vkey == 0)
    {
        printf(" ");
    }
    else
    {
        printf("%c", (char)vkey);
    }    
}
void SetTimeToTerminal(HANDLE hConsole, float time)
{
    //Clear
    COORD p = { TIME_X - 2, TIME_Y };
    SetConsoleCursorPosition(hConsole, p);
    printf("       ");

    SHORT posX = 0;
    if (time < 10)
        posX = TIME_X;
    else if (time < 100)
        posX = TIME_X - 1;
    else if (time < 1000)
        posX = TIME_X - 2;
        
    p = { posX, TIME_Y };
    SetConsoleCursorPosition(hConsole, p);
    printf("%.3f s", time);
}

int GetAllKeyAsync()
{
    int key = 0;
    for (key = 0x01; key <= 0xFE; key++)
    {
        if (GetAsyncKeyState(key) & 0x8000)
            return key;
    }
    return 0;
}
int GetState(int key)
{
    if (key == VK_F2)
        return 1;
    else if (key == VK_F10 || key == VK_ESCAPE)
        return 0;

    return -1;
}
int GetKey(int key)
{
    if (key >= 0x41 && key <= 0x5A)
    {
        return key;
    }
    return 0;
}
int GetRandomKey() 
{
    return 0x41 + (rand() % static_cast<int>(0x5A - 0x41 + 1));
}

int main(int argc, char* argv[])
{
    //Decorations
    PrintScuffold();
    //Console handle
    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    //Programm state
    int curState = 0;
    int prevState = 0;
    SetStatusToTerminal(hConsole, curState);
       
    measure_time_t time;
    int key = 0;
    int pressedKey = 0;
    int rndKey = 0;
    while (true) 
    { 
        key = GetAllKeyAsync();
        if (GetState(key) == 1) // change to RUN
        {
            curState = 1;
            if (curState != prevState) // new state
            {
                prevState = curState;
                time.delta = 0;
                time.end = 0;
                time.start = clock(); // start time
                SetStatusToTerminal(hConsole, 1);
                rndKey = GetRandomKey();
                SetKeyToTerminal(hConsole, rndKey, RANDOM_KEY_X, RANDOM_KEY_Y);
                pressedKey = 0;
                SetKeyToTerminal(hConsole, pressedKey, PRESSED_KEY_X, PRESSED_KEY_Y);
            }            
        }
        else if (GetState(key) == 0) // change to STOP
        {
            curState = 0;
            if (curState != prevState) // new state
            {
                prevState = curState;                
                SetStatusToTerminal(hConsole, 0);
                rndKey = 0;
                SetKeyToTerminal(hConsole, rndKey, RANDOM_KEY_X, RANDOM_KEY_Y);
            }
        }
        else
        {
            if (curState == 1)
            {
                pressedKey = GetKey(key);
                time.end = clock();
                time.delta = (float)(time.end - time.start) / CLOCKS_PER_SEC;
                SetTimeToTerminal(hConsole, time.delta);
                SetKeyToTerminal(hConsole, pressedKey, PRESSED_KEY_X, PRESSED_KEY_Y);

                if (pressedKey == rndKey)
                {
                    curState = prevState =0;
                    SetStatusToTerminal(hConsole, 0);
                }   
            }
        }
        Sleep(1); 
    }

    return 0;
}