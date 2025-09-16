#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <sstream>
#include <shobjidl.h> 
#include <objbase.h>
#include<Windows.h>
#include <fstream>
#include<iostream>
#include<mmsystem.h>
#include<playsoundapi.h>
#include "CC212SGL.h"
#include <conio.h>
#pragma comment(lib, "winmm.lib")

using namespace std;
#pragma comment(lib, "winmm.lib")
//Link to the Graphics .lib file
#pragma comment(lib, "CC212SGL.lib")
//Declare instance from the Graphics 
CC212SGL gr;

//Screen coordinates
long long Screen_X_Middle;
long long Screen_Y_Middle;

// it stops the interactions between the CMD and the mouse so graphics is not disturbed
void DisableQuickEdit() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    DWORD prev_mode;
    GetConsoleMode(hStdin, &prev_mode);

    // Remove QuickEdit Mode and Insert Mode
    DWORD new_mode = prev_mode & ~ENABLE_QUICK_EDIT_MODE;
    new_mode &= ~ENABLE_INSERT_MODE;

    // Enable Extended Flags (required to make changes stick)
    new_mode |= ENABLE_EXTENDED_FLAGS;

    SetConsoleMode(hStdin, new_mode);
}

//hide the blinking text cursor (caret) in CMD to prevent graphics disturption
void HideCursor(bool ShowCursor = false)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(hConsole, &cursorInfo); // get current cursor info
    cursorInfo.bVisible = ShowCursor;                 // hide the cursor
    SetConsoleCursorInfo(hConsole, &cursorInfo); // apply changes
}

void waitFor(int start, int threshold)
{
    while (true)
    {
        float _t = (clock() / (float)CLOCKS_PER_SEC * 1000.0f - start / (float)CLOCKS_PER_SEC * 1000.0f);
        if (_t >= threshold)
            break;
    }
}

class GraphicalObject
{
    int* Sprites;
    int TotalFramesCount = -1;
    int CurrentFrame = 0;

    int x = 0, y = 0;

public:
    GraphicalObject(int sprites = 1, int totalFramesCount = 1)
    {
        Sprites = new int[sprites];
        TotalFramesCount = totalFramesCount;
    }

    void loadFrames(const char path[], int imageWidth, int imageHight)
    {
        x = imageWidth / 2, y = imageHight / 2;
        if (TotalFramesCount > 1)
        {
            for (int i = 1;i <= TotalFramesCount;i++)
            {
                char Final_Loc[200];//maximum path size
                char num[10];

                itoa(i, num, 10);// conversion from int to string ascii

                strcpy(Final_Loc, path);// Final_Loc = path
                strcat(Final_Loc, num); // Final_Loc += num
                strcat(Final_Loc, ".png");// Final_Loc += ".png"

                Sprites[i - 1] = gr.loadImage(Final_Loc);
            }
        }
        else
        {
            char Final_Loc[200];//maximum path size
            strcpy(Final_Loc, path);// Final_Loc = path
            strcat(Final_Loc, ".png");// Final_Loc += ".png"
            Sprites[0] = gr.loadImage(Final_Loc);
        }
    }
    void AnimateVid(int HorizontalShift = 0, int VirticalShift = 0, bool Reversed = false)
    {
        if (Reversed == false)
        {
            for (int i = 0;i < TotalFramesCount;i++)
            {
                int frame_start = clock();

                gr.beginDraw();

                gr.drawImage(Sprites[i], Screen_X_Middle - x + HorizontalShift, Screen_Y_Middle - y + VirticalShift, gr.generateFromRGB(0, 0, 0));

                gr.endDraw();

                waitFor(frame_start, 1.0 / 30 * 1000);
            }
        }
        else if (Reversed == true)
        {
            for (int i = TotalFramesCount;i > 0;i--)
            {
                int frame_start = clock();

                gr.beginDraw();

                gr.drawImage(Sprites[i], Screen_X_Middle - x + HorizontalShift, Screen_Y_Middle - y + VirticalShift, gr.generateFromRGB(0, 0, 0));

                gr.endDraw();

                waitFor(frame_start, 1.0 / 30 * 1000);
            }
        }
    }
    void AnimateObject(int HorizontalShift = 0, int VirticalShift = 0, int R = 0, int G = 0, int B = 0)
    {
        if (TotalFramesCount > 1 && CurrentFrame != TotalFramesCount)
        {
            gr.beginDraw();
            gr.drawImage(Sprites[CurrentFrame++], Screen_X_Middle - x + HorizontalShift, Screen_Y_Middle - y + VirticalShift, gr.generateFromRGB(R, G, B));
            gr.endDraw();
        }

        else if (TotalFramesCount > 1 && CurrentFrame == TotalFramesCount)
        {
            CurrentFrame = 0;
            gr.beginDraw();
            gr.drawImage(Sprites[CurrentFrame++], Screen_X_Middle - x + HorizontalShift, Screen_Y_Middle - y + VirticalShift, gr.generateFromRGB(R, G, B));
            gr.endDraw();
        }

        else
        {
            gr.drawImage(Sprites[0], Screen_X_Middle - x + HorizontalShift, Screen_Y_Middle - y + VirticalShift, gr.generateFromRGB(R, G, B));
        }
    }

};
void MainMenu(GraphicalObject* planeShape,bool* call,GraphicalObject* Logo)
{
    int R1 = 110, B1 = 110;
    int R2 = 0, B2 = 0;

    short choice = 0;
    while (true)
    {
        int frame_start = clock();

        gr.beginDraw();
        planeShape->AnimateObject(400);
        Logo->AnimateObject(-Screen_X_Middle + 430, -Screen_Y_Middle + 170);

        if (choice == 1)
            R1 = 110, R2 = 0, B1 = 110, B2 = 0;
        else if (choice == 2)
            R1 = 0, R2 = 110, B1 = 0, B2 = 110;
        

        if (_kbhit())
        {
            int c = _getch();

            if (c == 224) { // special key (arrows, F keys, etc.)
                c = _getch(); // get the actual key code
                switch (c) {
                case 72: // Up arrow
                    if (choice > 1)
                        choice--;
                    break;
                case 80: // Down arrow
                    if (choice < 2)
                        choice++;
                    break;
                }
            }
            else if (c == 13) { // Enter key
                if (choice == 2) {
                    HWND hwnd = GetConsoleWindow();
                    PostMessage(hwnd, WM_CLOSE, 0, 0); // closes window
                }
                else 
                {
                    *call = false;
                    return;
                }
            }
        }


        //button back 1
        gr.setDrawingColor(gr.generateFromRGB(110, 0, 110));
        gr.drawSolidCircle(Screen_X_Middle - 750, Screen_Y_Middle -100, 140);
        gr.drawSolidRectangle(Screen_X_Middle - 690, Screen_Y_Middle - 100, 250, 140);
        gr.drawSolidCircle(Screen_X_Middle - 500, Screen_Y_Middle - 100, 140);
        //button 1
        gr.setDrawingColor(gr.generateFromRGB(R1, 0, B1));
        gr.drawSolidCircle(Screen_X_Middle - 740, Screen_Y_Middle -90, 120);
        gr.drawSolidRectangle(Screen_X_Middle - 680, Screen_Y_Middle - 90, 250, 120);
        gr.drawSolidCircle(Screen_X_Middle - 490, Screen_Y_Middle - 90, 120);
        
        //button back 2
        gr.setDrawingColor(gr.generateFromRGB(110, 0, 110));
        gr.drawSolidCircle(Screen_X_Middle - 750, Screen_Y_Middle + 110, 140);
        gr.drawSolidRectangle(Screen_X_Middle - 690, Screen_Y_Middle + 110, 250, 140);
        gr.drawSolidCircle(Screen_X_Middle - 500, Screen_Y_Middle + 110, 140);
        //button 2
        gr.setDrawingColor(gr.generateFromRGB(R2, 0, B2));
        gr.drawSolidCircle(Screen_X_Middle -740, Screen_Y_Middle + 120, 120);
        gr.drawSolidRectangle(Screen_X_Middle - 680, Screen_Y_Middle + 120, 250, 120);
        gr.drawSolidCircle(Screen_X_Middle - 490, Screen_Y_Middle + 120, 120);
        
        //button back 3
        gr.setDrawingColor(gr.generateFromRGB(110,0, 110));
        gr.drawSolidCircle(Screen_X_Middle - 850, Screen_Y_Middle + 360, 140);
        gr.drawSolidRectangle(Screen_X_Middle - 790, Screen_Y_Middle + 360, 440, 140);
        gr.drawSolidCircle(Screen_X_Middle - 420, Screen_Y_Middle + 360, 140);
        //button 3
        gr.setDrawingColor(gr.generateFromRGB(0, 0, 0));
        gr.drawSolidCircle(Screen_X_Middle -840, Screen_Y_Middle + 370, 120);
        gr.drawSolidRectangle(Screen_X_Middle - 780, Screen_Y_Middle + 370, 430, 120);
        gr.drawSolidCircle(Screen_X_Middle - 410, Screen_Y_Middle + 370, 120);

        //nameing
        gr.setDrawingColor(gr.generateFromRGB(255, 255, 255));
        gr.setFontSizeAndBoldness(100,100);
        gr.drawText(Screen_X_Middle - 640, Screen_Y_Middle - 80, "Play");
        gr.drawText(Screen_X_Middle - 630, Screen_Y_Middle + 130, "Exit");
        gr.setFontSizeAndBoldness(50,100);
        gr.drawText(Screen_X_Middle - 800, Screen_Y_Middle + 380, "            Created by\nMohamed Hazem Tawfik");

        gr.endDraw();

        //Time Control End 
        waitFor(frame_start, 1.0 / 30 * 1000);
    }
}
void showDraw() 
{
    
    gr.setFontSizeAndBoldness(400, 100);
    gr.setDrawingColor(gr.generateFromRGB(200, 200, 200));
    gr.drawText(Screen_X_Middle, Screen_Y_Middle - 200, "Draw");
    gr.endDraw();
}

int main()
{
    gr.setup();
    gr.setFullScreenMode();
    system("CLS"); // Clear the cmd
    DisableQuickEdit();
    HideCursor();
    CoInitialize(NULL);

    mciSendString(L"open \"music.mp3\" type mpegvideo alias bgmusic", NULL, 0, NULL);
    mciSendString(L"play bgmusic repeat", NULL, 0, NULL);

    //Screen center coordinates
    Screen_X_Middle = gr.getWindowWidth() / 2;
    Screen_Y_Middle = gr.getWindowHeight() / 2;

    //The matrix
    short Plane[3][3] = {0};

    //graphical objects 
    GraphicalObject* planeShape = new GraphicalObject(1,1);
    GraphicalObject* Table = new GraphicalObject(1,1);
    GraphicalObject* Logo = new GraphicalObject(1,1);
    GraphicalObject* THEX = new GraphicalObject(1,1);
    GraphicalObject* THEY = new GraphicalObject(1,1);

    //Loading frames
    planeShape->loadFrames("Images//Plane2",1125,1125);
    Table->loadFrames("Images//Table", 700, 651);
    Logo->loadFrames("Images//Logo", 548, 263);
    THEX->loadFrames("Images//THEX", 548, 263);
    THEY->loadFrames("Images//THEY", 548, 263);

    bool Callmainmenu = true;
    bool Playerturn = false;
    int Player1Score = 0;
    int Player2Score = 0;
    char P1S[10];
    char P2S[10];

    POINT cursorPos;
    while (true)
    {
        int frame_start = clock();

        //Mouse Position
        GetCursorPos(&cursorPos);

        gr.beginDraw();

        if (Callmainmenu)
            MainMenu(planeShape, &Callmainmenu, Logo);

        planeShape->AnimateObject(400);
        Table->AnimateObject(-Screen_X_Middle + 390);
        Logo->AnimateObject(-Screen_X_Middle + 430, -Screen_Y_Middle + 170);
        //button back 3
        gr.drawSolidCircle(Screen_X_Middle - 850, Screen_Y_Middle + 360, 140);
        gr.drawSolidRectangle(Screen_X_Middle - 790, Screen_Y_Middle + 360, 440, 140);
        gr.drawSolidCircle(Screen_X_Middle - 420, Screen_Y_Middle + 360, 140);

        //calling main menu
        if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && (cursorPos.x > Screen_X_Middle - 840 && cursorPos.x < Screen_X_Middle - 290) && ((cursorPos.y > Screen_Y_Middle + 360 && cursorPos.y < Screen_Y_Middle + 480)))
        {
            Callmainmenu = true;
            gr.setDrawingColor(gr.generateFromRGB(110, 0, 110));
            for (int i = 0;i < 3;i++)
            {
                for (int j = 0;j < 3;j++)
                    Plane[i][j] = 0;
            }
            Player1Score = 0;
            Player2Score = 0;
        }
        else if ((cursorPos.x > Screen_X_Middle - 840 && cursorPos.x < Screen_X_Middle - 290) && ((cursorPos.y > Screen_Y_Middle + 360 && cursorPos.y < Screen_Y_Middle + 480)))
            gr.setDrawingColor(gr.generateFromRGB(110, 0, 110));
        else
            gr.setDrawingColor(gr.generateFromRGB(0, 0, 0));
        gr.drawSolidCircle(Screen_X_Middle - 840, Screen_Y_Middle + 370, 120);
        gr.drawSolidRectangle(Screen_X_Middle - 780, Screen_Y_Middle + 370, 430, 120);
        gr.drawSolidCircle(Screen_X_Middle - 410, Screen_Y_Middle + 370, 120);

        //nameing
        gr.setDrawingColor(gr.generateFromRGB(255, 255, 255));
        gr.setFontSizeAndBoldness(70, 100);
        gr.drawText(Screen_X_Middle - 800, Screen_Y_Middle + 390, " Return To Menu");
        
        //draw score
        itoa(Player1Score,P1S,10);
        itoa(Player2Score,P2S,10);
        gr.setDrawingColor(gr.generateFromRGB(110, 0, 0));
        gr.setFontSizeAndBoldness(200, 100);
        gr.drawText(Screen_X_Middle - 800, Screen_Y_Middle+50 , P1S);
        gr.setDrawingColor(gr.generateFromRGB(0, 0, 110));
        gr.setFontSizeAndBoldness(200, 100);
        gr.drawText(Screen_X_Middle - 430, Screen_Y_Middle+50 , P2S);

        gr.setDrawingColor(gr.generateFromRGB(255, 255, 255));

        //Matrix draw
        if (true)
        {
            // Check rows
            if (Plane[0][0] != 0 && Plane[0][0] == Plane[0][1] && Plane[0][1] == Plane[0][2])
            {
                if (Plane[0][0] == 1) Player1Score++; else Player2Score++;

                for (int i = 0;i < 40;i++)
                {
                    int cooldown = clock();
                    waitFor(cooldown, 1.0 / 30 * 1000);
                }
                for (int i = 0;i < 3;i++)
                {
                    for (int j = 0;j < 3;j++)
                        Plane[i][j] = 0;
                }

            }
            if (Plane[1][0] != 0 && Plane[1][0] == Plane[1][1] && Plane[1][1] == Plane[1][2])
            {
                if (Plane[1][0] == 1) Player1Score++; else Player2Score++;
                
                for (int i = 0;i < 40;i++)
                {
                    int cooldown = clock();
                    waitFor(cooldown, 1.0 / 30 * 1000);
                }
                for (int i = 0;i < 3;i++)
                {
                    for (int j = 0;j < 3;j++)
                        Plane[i][j] = 0;
                }
            }
            if (Plane[2][0] != 0 && Plane[2][0] == Plane[2][1] && Plane[2][1] == Plane[2][2])
            {
                if (Plane[2][0] == 1) Player1Score++; else Player2Score++;
                for (int i = 0;i < 40;i++)
                {
                    int cooldown = clock();
                    waitFor(cooldown, 1.0 / 30 * 1000);
                }
                for (int i = 0;i < 3;i++)
                {
                    for (int j = 0;j < 3;j++)
                        Plane[i][j] = 0;
                }
            }

            // Check columns
            if (Plane[0][0] != 0 && Plane[0][0] == Plane[1][0] && Plane[1][0] == Plane[2][0])
            {
                if (Plane[0][0] == 1) Player1Score++; else Player2Score++;
                for (int i = 0;i < 40;i++)
                {
                    int cooldown = clock();
                    waitFor(cooldown, 1.0 / 30 * 1000);
                }
                for (int i = 0;i < 3;i++)
                {
                    for (int j = 0;j < 3;j++)
                        Plane[i][j] = 0;
                }
            }
            if (Plane[0][1] != 0 && Plane[0][1] == Plane[1][1] && Plane[1][1] == Plane[2][1])
            {
                if (Plane[0][1] == 1) Player1Score++; else Player2Score++;
                for (int i = 0;i < 40;i++)
                {
                    int cooldown = clock();
                    waitFor(cooldown, 1.0 / 30 * 1000);
                }
                for (int i = 0;i < 3;i++)
                {
                    for (int j = 0;j < 3;j++)
                        Plane[i][j] = 0;
                }
            }
            if (Plane[0][2] != 0 && Plane[0][2] == Plane[1][2] && Plane[1][2] == Plane[2][2])
            {
                if (Plane[0][2] == 1) Player1Score++; else Player2Score++;
                for (int i = 0;i < 40;i++)
                {
                    int cooldown = clock();
                    waitFor(cooldown, 1.0 / 30 * 1000);
                }
                for (int i = 0;i < 3;i++)
                {
                    for (int j = 0;j < 3;j++)
                        Plane[i][j] = 0;
                }
            }

            // Check diagonals
            if (Plane[0][0] != 0 && Plane[0][0] == Plane[1][1] && Plane[1][1] == Plane[2][2])
            {
                if (Plane[0][0] == 1) Player1Score++; else Player2Score++;
                for (int i = 0;i < 40;i++)
                {
                    int cooldown = clock();
                    waitFor(cooldown, 1.0 / 30 * 1000);
                }
                for (int i = 0;i < 3;i++)
                {
                    for (int j = 0;j < 3;j++)
                        Plane[i][j] = 0;
                }
            }
            if (Plane[0][2] != 0 && Plane[0][2] == Plane[1][1] && Plane[1][1] == Plane[2][0])
            {
                if (Plane[0][2] == 1) Player1Score++; else Player2Score++;
                for (int i = 0;i < 40;i++)
                {
                    int cooldown = clock();
                    waitFor(cooldown, 1.0 / 30 * 1000);
                }
                for (int i = 0;i < 3;i++)
                {
                    for (int j = 0;j < 3;j++)
                        Plane[i][j] = 0;
                }
            }
            bool Isdraw = true;
            for (int i = 0;i < 3;i++)
            {
                for (int j = 0;j < 3;j++)
                {
                    if (Plane[i][j] == 0)
                        Isdraw = false;
                }
            }
            

            //[0][0]
            if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && (cursorPos.x > Screen_X_Middle - 155 && cursorPos.x < Screen_X_Middle + 210) && ((cursorPos.y > Screen_Y_Middle - 535 && cursorPos.y < Screen_Y_Middle - 168)))
            {
                if (Plane[0][0] == 0)
                {
                    if (Playerturn)
                    {
                        Plane[0][0] = 1;
                        Playerturn = !Playerturn;
                    }
                    else
                    {
                        Plane[0][0] = 2;
                        Playerturn = !Playerturn;
                    }
                }
            }
            //[0][1]
            else if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && (cursorPos.x > Screen_X_Middle + 215 && cursorPos.x < Screen_X_Middle + 580) && ((cursorPos.y > Screen_Y_Middle - 535 && cursorPos.y < Screen_Y_Middle - 168)))
            {
                if (Plane[0][1] == 0)
                {
                    if (Playerturn)
                    {
                        Plane[0][1] = 1;
                        Playerturn = !Playerturn;
                    }
                    else
                    {
                        Plane[0][1] = 2;
                        Playerturn = !Playerturn;
                    }
                }
            }
            //[0][2]
            else if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && (cursorPos.x > Screen_X_Middle + 585 && cursorPos.x < Screen_X_Middle + 900) && ((cursorPos.y > Screen_Y_Middle - 535 && cursorPos.y < Screen_Y_Middle - 168)))
            {
                if (Plane[0][2] == 0)
                {
                    if (Playerturn)
                    {
                        Plane[0][2] = 1;
                        Playerturn = !Playerturn;
                    }
                    else
                    {
                        Plane[0][2] = 2;
                        Playerturn = !Playerturn;
                    }
                }
            }
            //[1][0]
            else if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && (cursorPos.x > Screen_X_Middle - 155 && cursorPos.x < Screen_X_Middle + 210) && ((cursorPos.y > Screen_Y_Middle - 172 && cursorPos.y < Screen_Y_Middle + 178)))
            {
                if (Plane[1][0] == 0)
                {
                    if (Playerturn)
                    {
                        Plane[1][0] = 1;
                        Playerturn = !Playerturn;
                    }
                    else
                    {
                        Plane[1][0] = 2;
                        Playerturn = !Playerturn;
                    }
                }
            }
            //[1][1]
            else if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && (cursorPos.x > Screen_X_Middle + 215 && cursorPos.x < Screen_X_Middle + 580) && ((cursorPos.y > Screen_Y_Middle - 172 && cursorPos.y < Screen_Y_Middle + 178)))
            {
                if (Plane[1][1] == 0)
                {
                    if (Playerturn)
                    {
                        Plane[1][1] = 1;
                        Playerturn = !Playerturn;
                    }
                    else
                    {
                        Plane[1][1] = 2;
                        Playerturn = !Playerturn;
                    }
                }
            }
            //[1][2]
            else if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && (cursorPos.x > Screen_X_Middle + 585 && cursorPos.x < Screen_X_Middle + 900) && ((cursorPos.y > Screen_Y_Middle - 172 && cursorPos.y < Screen_Y_Middle + 178)))
            {
                if (Plane[1][2] == 0)
                {
                    if (Playerturn)
                    {
                        Plane[1][2] = 1;
                        Playerturn = !Playerturn;
                    }
                    else
                    {
                        Plane[1][2] = 2;
                        Playerturn = !Playerturn;
                    }
                }
            }
            //[2][0]
            else if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && (cursorPos.x > Screen_X_Middle - 155 && cursorPos.x < Screen_X_Middle + 210) && ((cursorPos.y > Screen_Y_Middle + 205 && cursorPos.y < Screen_Y_Middle + 555)))
            {
                if (Plane[2][0] == 0)
                {
                    if (Playerturn)
                    {
                        Plane[2][0] = 1;
                        Playerturn = !Playerturn;
                    }
                    else
                    {
                        Plane[2][0] = 2;
                        Playerturn = !Playerturn;
                    }
                }
            }
            //[2][1]
            else if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && (cursorPos.x > Screen_X_Middle + 215 && cursorPos.x < Screen_X_Middle + 580) && ((cursorPos.y > Screen_Y_Middle + 205 && cursorPos.y < Screen_Y_Middle + 555)))
            {
                if (Plane[2][1] == 0)
                {
                    if (Playerturn)
                    {
                        Plane[2][1] = 1;
                        Playerturn = !Playerturn;
                    }
                    else
                    {
                        Plane[2][1] = 2;
                        Playerturn = !Playerturn;
                    }
                }
            }
            //[2][2]
            else if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && (cursorPos.x > Screen_X_Middle + 585 && cursorPos.x < Screen_X_Middle + 900) && ((cursorPos.y > Screen_Y_Middle + 205 && cursorPos.y < Screen_Y_Middle + 555)))
            {
                if (Plane[2][2] == 0)
                {
                    if (Playerturn)
                    {
                        Plane[2][2] = 1;
                        Playerturn = !Playerturn;
                    }
                    else
                    {
                        Plane[2][2] = 2;
                        Playerturn = !Playerturn;
                    }
                }
            }
            
            //Matrix draw
            //[0][0]
            if (Plane[0][0] == 1)
            {
                THEX->AnimateObject(-155 + 320, -335);
            }
            else if (Plane[0][0] == 2)
            {
                THEY->AnimateObject(-155 + 320, -335);
            }

            //[0][1]
            if (Plane[0][1] == 1)
            {
                THEX->AnimateObject(215 + 320, -335);
            }
            else if (Plane[0][1] == 2)
            {
                THEY->AnimateObject(215 + 320, -335);
            }

            //[0][2]
            if (Plane[0][2] == 1)
            {
                THEX->AnimateObject(585 + 320, -335);
            }
            else if (Plane[0][2] == 2)
            {
                THEY->AnimateObject(585 + 320, -335);
            }
            //[1][0]
            if (Plane[1][0] == 1)
            {
                THEX->AnimateObject(-155 + 320, -335 + 350);
            }
            else if (Plane[1][0] == 2)
            {
                THEY->AnimateObject(-155 + 320, -335 + 350);
            }

            //[1][1]
            if (Plane[1][1] == 1)
            {
                THEX->AnimateObject(215 + 320, -335 + 350);
            }
            else if (Plane[1][1] == 2)
            {
                THEY->AnimateObject(215 + 320, -335 + 350);
            }

            //[1][2]
            if (Plane[1][2] == 1)
            {
                THEX->AnimateObject(585 + 320, -335 + 350);
            }
            else if (Plane[1][2] == 2)
            {
                THEY->AnimateObject(585 + 320, -335 + 350);
            }
            //[2][0]
            if (Plane[2][0] == 1)
            {
                THEX->AnimateObject(-155 + 320, -335 + 700);
            }
            else if (Plane[2][0] == 2)
            {
                THEY->AnimateObject(-155 + 320, -335 + 700);
            }

            //[2][1]
            if (Plane[2][1] == 1)
            {
                THEX->AnimateObject(215 + 320, -335 + 700);
            }
            else if (Plane[2][1] == 2)
            {
                THEY->AnimateObject(215 + 320, -335 + 700);
            }

            //[2][2]
            if (Plane[2][2] == 1)
            {
                THEX->AnimateObject(585 + 320, -335 + 700);
            }
            else if (Plane[2][2] == 2)
            {
                THEY->AnimateObject(585 + 320, -335 + 700);
            }
            // Check rows
            gr.setDrawingColor(gr.generateFromRGB(110,0,110));
            if (Plane[0][0] != 0 && Plane[0][0] == Plane[0][1] && Plane[0][1] == Plane[0][2])
                gr.drawSolidRectangle(Screen_X_Middle - 155, Screen_Y_Middle - 535 + 165, 585 + 350 + 155, 20);
            if (Plane[1][0] != 0 && Plane[1][0] == Plane[1][1] && Plane[1][1] == Plane[1][2])
                gr.drawSolidRectangle(Screen_X_Middle - 155, Screen_Y_Middle - 535 + 165+350, 585 + 350 + 155, 20);
            if (Plane[2][0] != 0 && Plane[2][0] == Plane[2][1] && Plane[2][1] == Plane[2][2])
                gr.drawSolidRectangle(Screen_X_Middle - 155, Screen_Y_Middle - 535 + 165+700, 585 + 350 + 155, 20);

            // Check columns
            if (Plane[0][0] != 0 && Plane[0][0] == Plane[1][0] && Plane[1][0] == Plane[2][0])
                gr.drawSolidRectangle(Screen_X_Middle -155 +350/2, Screen_Y_Middle - 535+60 , 20, 585 + 350+30);
            if (Plane[0][1] != 0 && Plane[0][1] == Plane[1][1] && Plane[1][1] == Plane[2][1])
                gr.drawSolidRectangle(Screen_X_Middle -155 +350/2+360, Screen_Y_Middle - 535+60 , 20, 585 + 350+30);
            if (Plane[0][2] != 0 && Plane[0][2] == Plane[1][2] && Plane[1][2] == Plane[2][2])
                gr.drawSolidRectangle(Screen_X_Middle -155 +350/2+730, Screen_Y_Middle - 535+60 , 20, 585 + 350+30);
            // Check diagonals
            gr.setDrawingThickness(20);
            if (Plane[0][2] != 0 && Plane[0][2] == Plane[1][1] && Plane[1][1] == Plane[2][0])
                gr.drawLine(Screen_X_Middle-155+50,Screen_Y_Middle+205+300, Screen_X_Middle +585+300, Screen_Y_Middle -535+50);
            if (Plane[0][0] != 0 && Plane[0][0] == Plane[1][1] && Plane[1][1] == Plane[2][2])
                gr.drawLine(Screen_X_Middle - 155 + 50, Screen_Y_Middle - 535 + 50, Screen_X_Middle + 585 + 300, Screen_Y_Middle + 205 + 300);
            gr.setDrawingThickness(0);
            
            if (Isdraw)
            {
                for (int i = 0;i < 40;i++)
                {
                    int cooldown = clock();
                    showDraw();
                    waitFor(cooldown, 1.0 / 30 * 1000);
                }
                for (int i = 0;i < 40;i++)
                {
                    int cooldown = clock();
                    waitFor(cooldown, 1.0 / 30 * 1000);
                }

                for (int i = 0;i < 3;i++)
                {
                    for (int j = 0;j < 3;j++)
                        Plane[i][j] = 0;
                }
                
            }

            
        }
       
        gr.endDraw();

        //Time Control End 
        waitFor(frame_start, 1.0 / 30 * 1000);
    }
    CoUninitialize();
    return 0;
}
