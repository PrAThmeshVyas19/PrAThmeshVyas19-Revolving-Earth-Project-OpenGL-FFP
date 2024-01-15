// Windows header files
#include<windows.h>
#include"OGL.h"
#include<stdio.h>
#include<stdlib.h>

// OpenGL Header files
#include<gl/GL.h>
#include<gl/GLU.h>

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Link with OpenGL Library
#pragma comment(lib , "OpenGL32.lib")
#pragma comment(lib , "glu32.lib")

// Global Function decarations
LRESULT CALLBACK WndProc(HWND , UINT , WPARAM , LPARAM);

// Global Variable declaration
FILE *gpFILE = NULL; // global pointer Type

HWND ghwnd = NULL;  // global hwnd
BOOL gbActive = FALSE;
DWORD dwStyle = 0;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};
BOOL gbFullscreen = FALSE;

// OpenGL Related Global Variables
HDC ghdc = NULL;
HGLRC ghrc = NULL;

// Global Variable
GLfloat pAngle = 0.0f;

GLUquadric *quadric = NULL;

GLuint Texture_Earth = 0;
GLuint Texture_Stars = 0;

GLfloat YTranslate = 0.0f;
GLfloat XTranslate = 0.0f;
GLfloat ScaleX = 0.0f;
GLfloat ScaleY = 0.0f;
GLfloat ScaleZ = 0.0f;
GLfloat ZTranslate = 0.0f;
GLfloat lightAngle = 0.0f;

// for light variable
BOOL bLight = FALSE;

GLfloat lightAmbient[] = {1.0f , 1.0f , 1.0f , 1.0f};   
GLfloat lightDiffused[] = {1.0f , 1.0f , 1.0f , 1.0f};  // White light
GLfloat lightSpecular[] = {1.0f , 1.0f , 1.0f , 1.0f};  // White light
// GLfloat spot_direction[3];
GLfloat lightPosition[4];

// Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance , LPSTR lpszCmdLine , int iCmdShow)
{
    // Function Declarations
    int initialize(void);
    void uninitialize(void);
    void display(void);
    void update(void);

    // Local Variable Declarations
    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;
    TCHAR szAppName[] = TEXT("PPVWindow");
    int iResult = 0;
    BOOL bDone = FALSE;

    // code
    gpFILE = fopen("Log.txt" , "w");
    if(gpFILE == NULL)
    {
        MessageBox(NULL , TEXT("Log file cannot be opened") , TEXT("Error") , MB_OK | MB_ICONERROR);
        exit(0);
    }

    fprintf(gpFILE , "Jay Ganeshay Namaha \n Program Started Successfully \n");

    // Wndclassex Initialization
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.hIcon = LoadIcon(hInstance , MAKEINTRESOURCE(MYICON));
    wndclass.hCursor = LoadCursor(NULL , IDC_ARROW);
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = NULL;
    wndclass.hIconSm = LoadIcon(hInstance , MAKEINTRESOURCE(MYICON));

    // Register WNdclassex
    RegisterClassEx(&wndclass);

    // Create Window
    hwnd = CreateWindowEx(WS_EX_APPWINDOW ,
                        szAppName , 
                        TEXT("Prathmesh P. Vyas RTR5") ,   
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 
                        CW_USEDEFAULT ,
                        CW_USEDEFAULT , 
                        WIN_WIDTH , 
                        WIN_HEIGHT ,
                        NULL ,
                        NULL , 
                        hInstance , 
                        NULL);
    ghwnd = hwnd;

    // Initialization
    iResult = initialize();

    if(iResult != 0)
    {
        MessageBox(NULL , TEXT("Initialize() failed") , TEXT("Error") , MB_OK | MB_ICONERROR);
        DestroyWindow(hwnd);
    }

    // Show The WIndow
    ShowWindow(hwnd , iCmdShow);

    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // Game Loop
    while(bDone == FALSE)
    {
        if(PeekMessage(&msg , NULL , 0 , 0 , PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
            {
                bDone = TRUE;
            }

            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        else
        {
            if(gbActive == TRUE)
            {
                // Render
                display();

                // Update
                update();
            }
        }
        
    }

    // Uninitialization
    uninitialize();

    return((int)msg.wParam);
}

// Call Back Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg , WPARAM wParam, LPARAM lParam)
{
    // Function declarations
    void ToggleFullscreen(void);
    void resize(int , int);

    // code
    switch(iMsg)
    {
        case WM_SETFOCUS:
            gbActive = TRUE;
            break;

        case WM_KILLFOCUS:
            gbActive = FALSE;
            break;

        case WM_SIZE:
            resize(LOWORD(lParam) , HIWORD(lParam));    // width ani height sangto eg. Awaj ala
            break;

        case WM_ERASEBKGND:
            return 0;

        case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {
                case VK_ESCAPE:
                    DestroyWindow(hwnd);
                    break;
            }
            break;

        case WM_CHAR:
            switch(LOWORD(wParam))
            {
                case 'F':
                case 'f':
                    if(gbFullscreen == FALSE)
                    {
                        ToggleFullscreen();
                        gbFullscreen = TRUE;
                    }
                    else
                    {
                        ToggleFullscreen();
                        gbFullscreen = FALSE;
                    }
                    break;

                case 'L':
                case 'l':
                    if (bLight == FALSE)
                    {
                        glEnable(GL_LIGHTING);
                        bLight = TRUE;
                    }
                    else
                    {
                        glDisable(GL_LIGHTING);
                        bLight = FALSE;
                    }
                    break;

                case 'W':
                    YTranslate += 0.01f;
                    break;

                case 'w':
                    YTranslate -= 0.01f;
                    break;

                case 'Z':
                    ZTranslate += 0.5f;
                    break;

                case 'z':
                    ZTranslate -= 0.5f;
                    break;

                case 'D':
                case 'd':
                    XTranslate += 0.01f;
                    break;

                case 'A':
                case 'a':
                    XTranslate -= 0.01f;
                    break;

                case 'E':
                    ScaleX += 0.01f;
                    break;

                case 'e':
                    ScaleX -= 0.01f;
                    break;

                case 'Q':
                    ScaleY += 0.01f;
                    break;

                case 'R':
                    ScaleZ -= 0.1f;
                    break;

                case 'r':
                    ScaleZ += 0.1f;
                    break;

                case 'q':
                    ScaleY -= 0.01f;
                    break;

                case 'x':
                    ScaleY -= 1.0f;
                    break;

                case 'X':
                    ScaleY += 1.0f;
                    break;

            }
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            if(gpFILE)
            {
                fprintf(gpFILE , "YTranslate = %f\n",YTranslate);
                fprintf(gpFILE , "XTranslate = %f\n",XTranslate);
                fprintf(gpFILE , "XScale = %f\n",ScaleX);
                fprintf(gpFILE , "YScale = %f\n",ScaleY);
                fprintf(gpFILE , "ZTranslate = %f\n",ZTranslate);
                fprintf(gpFILE , "Program Ended Successfully \n");
            }
            PostQuitMessage(0);
            break;
        
        default:
            break;
    }

    return DefWindowProc(hwnd , iMsg , wParam , lParam);
}

void ToggleFullscreen(void)
{
    // local varaible declarations
    MONITORINFO mi = {sizeof(MONITORINFO)};

    // code
    if(gbFullscreen == FALSE)
    {
        dwStyle = GetWindowLong(ghwnd , GWL_STYLE);

        if(dwStyle & WS_OVERLAPPEDWINDOW)
        {
            if(GetWindowPlacement(ghwnd , &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd , MONITORINFOF_PRIMARY) , &mi))
            {
                SetWindowLong(ghwnd , GWL_STYLE , dwStyle  & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghwnd , HWND_TOP , mi.rcMonitor.left , mi.rcMonitor.top , mi.rcMonitor.right - mi.rcMonitor.left , mi.rcMonitor.bottom - mi.rcMonitor.top , SWP_NOZORDER | SWP_FRAMECHANGED);   // HWND_TOP == WS_OVERLAPPED , SWP_FRAMECHANGES => WM_NCCALSIZE(New Non client area calculate size)
            }
        }
        ShowCursor(FALSE);
    }

    else
    {
        SetWindowPlacement(ghwnd , &wpPrev);
        SetWindowLong(ghwnd , GWL_STYLE , dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPos(ghwnd , HWND_TOP , 0 , 0 , 0 , 0 , SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }
}


int initialize(void)
{
    // function declarations
    void resize(int , int);
    BOOL loadGLTexture(GLuint * , TCHAR[]);

    // variable declarations
    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormatIndex = 0;
    BOOL bResult;

    // code
    ZeroMemory(&pfd , sizeof(PIXELFORMATDESCRIPTOR));

    // Initialization of pixel format descriptor
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;

    // Step 2 Get the device context painter
    ghdc = GetDC(ghwnd);
    if(ghdc == NULL)
    {
        fprintf(gpFILE,"GetDC() Failed !!!\n");
        return -1;
    }

    // Step 3 
    iPixelFormatIndex = ChoosePixelFormat(ghdc , &pfd);
    if(iPixelFormatIndex == 0)
    {
        fprintf(gpFILE , "ChoosePixelFormat() Failed !!!\n");
        return -2;
    }

    // Step 4 Set optained pixel format
    if(SetPixelFormat(ghdc , iPixelFormatIndex,&pfd) == FALSE)
    {
        fprintf(gpFILE , "SetPixelFormat() Failed!!!\n");
        return -3;
    }

    // Step 5 Tell WindowGraphicLibrary to give me OpenGL compatible dc from this dc
    ghrc = wglCreateContext(ghdc);
    if(ghrc == NULL)
    {
        fprintf(gpFILE , "wglCreateContext() Failed !!!\n");
        return -4;
    }// now gldc will end its role and give controll to ghrc

    // Make Rendering Context Current
    if(wglMakeCurrent(ghdc , ghrc) == FALSE)
    {
        fprintf(gpFILE , "wglMakeCurrent() Failed !!!\n");
        return -5;
    }

    // Enabling Depth
    glShadeModel(GL_SMOOTH); // Lighting ani coloring la shading smooth kr
    glClearDepth(1.0f); // Depth buffer la clear karayla hii value vapar saglya bits la 1 kr
    glEnable(GL_DEPTH_TEST); // 8 test peiki enable kr depth test
    glDepthFunc(GL_LEQUAL); // Depth sathi konta func vaparu test sathi LEQUAL = Less than or Equal to => ClearDepth 1.0f z coordinate
    glHint(GL_PERSPECTIVE_CORRECTION_HINT , GL_NICEST); // Optional Beautification

    quadric = gluNewQuadric();

    // Set The ClearColor of Window to Blue
    // Here OpenGL Starts "Shree Gurudev Datta"
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    bResult = loadGLTexture(&Texture_Earth , MAKEINTRESOURCE(MY_EARTH));
    if (bResult == FALSE)
    {
        fprintf(gpFILE , "Loading of Kundali Texture Failed !!!\n");
        return -1;
    }

    bResult = loadGLTexture(&Texture_Stars , MAKEINTRESOURCE(MY_STARS));
    if (bResult == FALSE)
    {
        fprintf(gpFILE , "Loading of Star Texture Failed !!!\n");
        return -7;
    }

    glEnable(GL_TEXTURE_2D);

    // Enabling light zero
    glLightfv(GL_LIGHT0 , GL_AMBIENT , lightAmbient);
    glLightfv(GL_LIGHT0 , GL_DIFFUSE , lightDiffused);
    glLightfv(GL_LIGHT0 , GL_SPECULAR , lightSpecular);

    resize(WIN_WIDTH , WIN_HEIGHT);

    return 0;
}

BOOL loadGLTexture(GLuint * texture , TCHAR image_resource_ID[])
{
    // local variable declarations
    HBITMAP hBitmap = NULL;
    BITMAP bmp;

    // Load the Image
    hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL) , image_resource_ID , IMAGE_BITMAP , 0 , 0 , LR_CREATEDIBSECTION);
    if (hBitmap == NULL)
    {
        fprintf(gpFILE , "LoadImage() Failed\n");
        return FALSE;
    }
    
    // Get Image Data
    GetObject(hBitmap , sizeof(BITMAP) , &bmp);

    // Create OpenGL Texture
    glGenTextures(1 , texture);

    // BLind to the Generated Tecture
    glBindTexture(GL_TEXTURE_2D , *texture);

    // Unpack The loaded image
    glPixelStorei(GL_UNPACK_ALIGNMENT , 4);

    // Set Texture Parameters
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR);

    // Create Multiple Mipmap images
    gluBuild2DMipmaps(GL_TEXTURE_2D , GL_RGB , bmp.bmWidth , bmp.bmHeight , GL_BGR_EXT , GL_UNSIGNED_BYTE , (void *)bmp.bmBits);

    // Unbind Texture
    glBindTexture(GL_TEXTURE_2D , 0);   

    // Delete Image Resource Object
    DeleteObject(hBitmap);

    hBitmap = NULL;

    return TRUE;
}


void resize(int width , int height)
{
    // code
    if(height <= 0)
    {
        height = 1;
    }

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    gluPerspective(45.0f , (GLfloat)width / (GLfloat)height , 0.1f , 100.0f);
}

void display(void)
{
    // code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glTranslatef(0.0f , 0.0f , -3.0f);

    glPushMatrix();
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);

        glTranslatef(0.0f , 0.0f , 0.0f);

        glBindTexture(GL_TEXTURE_2D , Texture_Stars);

        glBegin(GL_QUADS);

        glTexCoord2f(0.0 , 0.0);
        glVertex3f(-3.0f , 3.0f , 0.0f);

        glTexCoord2f(1.0 , 0.0);
        glVertex3f(3.0f , 3.0f , 0.0f);

        glTexCoord2f(1.0 , 1.0);
        glVertex3f(3.0f , -3.0f , 0.0f);

        glTexCoord2f(0.0 , 1.0);
        glVertex3f(-3.0f , -3.0f , 0.0f);

        glEnd();
    }
    glPopMatrix();

    // Rendering light0
    glPushMatrix();
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        // glRotatef(pAngle , 1.0f , 0.0f, 0.0f);
        lightPosition[0] = 0.060000; 
        lightPosition[1] = -0.010000; // By Rule
        lightPosition[2] = 4.500000;
        lightPosition[3] = 1.0f;
        glLightfv(GL_LIGHT0 , GL_POSITION , lightPosition);
        
        // spot_direction[0] = ScaleX;
        // spot_direction[1] = ScaleY;
        // spot_direction[2] = ScaleZ;
        // glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);

    }
    glPopMatrix();

    glPushMatrix();
    {
        // glEnable(GL_LIGHTING);
        // glEnable(GL_LIGHT0);
        
        glTranslatef(0.530000 , 0.0f , 0.0f);

        glRotatef(90.0F , -1.0F , 0.0F , 0.0F);

        glRotatef(pAngle , 0.0f , 0.0f , -1.0f);

        glBindTexture(GL_TEXTURE_2D , Texture_Earth);

        gluQuadricTexture(quadric , GLU_TRUE);

        gluSphere(quadric , 1.0f , 50 , 50);
    }
    glPopMatrix();

    glPushMatrix();
    {
        
    }
    glPopMatrix();

    SwapBuffers(ghdc);  // Win32 API

    glBindTexture(GL_TEXTURE_2D , 0);
}

void update(void)
{
    // code
    pAngle = pAngle + 0.05f;

    if (pAngle >= 360.0f)
    {
        pAngle = pAngle - 360.0f;
    }

    // Animating light 0
    lightAngle = lightAngle + 1.0f;

    if(lightAngle > 360.0f)
    {
        lightAngle = lightAngle - 360.0f;
    }
    
}

void uninitialize(void)
{
    // function declarations
    void ToggleFullscreen(void);

    // Make The hdc as current dc
    if(wglGetCurrentContext() == ghrc)
    {
        wglMakeCurrent(NULL , NULL); // Os la sangna sagla context kadun maza context de
    }// Dc la controll de

    // Delete Rendering context
    if(ghrc)
    {
        wglDeleteContext(ghrc);
        ghrc = NULL;
    }

    // Release the hdc
    if(ghdc)
    {
        ReleaseDC(ghwnd , ghdc);
        ghdc = NULL;
    }

    // code

    // if application is exiting in full screen 
    if(gbFullscreen == TRUE)
    {
        ToggleFullscreen();
        gbFullscreen = FALSE;
    }

    // DestroyWIndow
    if(ghwnd)
    {
        DestroyWindow(ghwnd);
        ghwnd = NULL;
    }

    // close the log file
    if(gpFILE)
    {
        fclose(gpFILE);
        gpFILE = NULL;
    }
}
