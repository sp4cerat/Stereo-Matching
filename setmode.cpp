///////////////////////////////////////////
// includes
#include <windows.h>
#include <mmsystem.h>
#include <malloc.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
///////////////////////////////////////////
//float fabs(float a){if (a>=0)return a; else return (-a);}
float fmin(float a,float b){if(a<b)return a;return b;}
///////////////////////////////////////////

void GetDepth(unsigned char* left,unsigned char* right,unsigned char* depth,int width,int height)
{			
	int i,j,y;  

	float* matrix=(float*)malloc((width+1)*(width+1)*4);

	// Initialize DP Matrix
	for(i=0;i<width+1;i++)
	{ 		   
	 matrix [ 0 + i * width ] = float(i);
	 matrix [ i + 0 * width ] = float(i);
	}		 

	// Start -> loop over all y-lines
	for( y = 0; y < height ; y++ )
	{
		// Evaluate DP Matrix
		for(j=1;j<width;j++)
		{
			for(i=1;i<width;i++)
			{
			 // read right image [ i-1 , y ]
			 int r_r = right[( i-1 + y * width )*3+0];//R
			 int r_g = right[( i-1 + y * width )*3+1];//G
			 int r_b = right[( i-1 + y * width )*3+2];//B

			 // read left image [ j-1 , y ]
			 int l_r = left [( j-1 + y * width )*3+0];//R
			 int l_g = left [( j-1 + y * width )*3+1];//G
			 int l_b = left [( j-1 + y * width )*3+2];//B

			 // absolute difference in color space
			 float dif_rgb = float(abs(l_r-r_r) + abs(l_g-r_g) + abs(l_b-r_b));

			 float add;
			 float m00 = matrix[( i-1 )+( j-1 )*width];
			 float m01 = matrix[( i-1 )+( j   )*width];
			 float m10 = matrix[( i   )+( j-1 )*width];

			 add = fmin(m00,m01);
			 add = fmin(add,m10);		 

			 add = add + dif_rgb;

			 matrix[i+j*width] = add;
			}//x
		}//y

		// init path position

		int pathX=width-1;
		int pathY=width-1;

		// find Path

		while(( pathX > 0 )||( pathY > 0 ))
		{
		 // initialize with a big number
		 float d00 = 10000;
		 float d01 = 10000;
		 float d10 = 10000;
		 
		 // read Matrix 
		 if(pathX>0)if(pathY>0) d00=matrix[(pathX-1)+(pathY-1)*width];
		 if(pathX>0)            d01=matrix[(pathX-1)+(pathY  )*width];
		 if(pathY>0)            d10=matrix[(pathX  )+(pathY-1)*width];
		 
		 // choose new position
		 if((d00<=d01)&&(d00<=d10)){pathX--;pathY--;}else // d00 is smallest
		 if((d01< d00)&&(d01<=d10)){pathX--;        }else // d01 is smallest
								   {        pathY--;}     // d10 is smallest

		 int disparity = ( pathY-pathX )*16+128;

		 // RGB Image
		 depth[( pathX + y*width )*3 + 0 ]=disparity;
		 depth[( pathX + y*width )*3 + 1 ]=disparity;
		 depth[( pathX + y*width )*3 + 2 ]=disparity;
		}
	}
	free (matrix);
}
///////////////////////////////////////////
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)											// This checks what the message is.  Below is some of the message that windows might return.  There of course is HUNDREDS.
    {														// The only message that you NEED to deal with, is the WM_DESTROY.  The rest you don't have to check for.  I am just showing some examples on how to check for a message.
		case WM_CREATE:										// This message is sent when the window is created.
			// We would want to put our initialization code here...
			break;											// We break from the switch statement.

		case WM_PAINT:										// This message is sent to the WndProc when the window needs to be repainted.  This might be if we moved the window, resized it, or maximized it, or another window was over it.
			// Put our code here to paint what should be in the window.	
		    break;											// Break from the switch statement

		case WM_CLOSE:									// This message is sent when the user closes the window.
		case WM_DESTROY:									// This message is sent when the user closes the window.
		//	 self->running=false;
		//	PostQuitMessage(0);								// You must call this function or else you will need to do control-alt-delete and manually close the program from the programs queue.  0 = WM_QUIT.
															// This function actually puts a message on the message queue.  SendMessage() sends a direct message, which is acted upon immediately.  just for your information.
			break;											// Break from the switch statement
	}	
	
	return DefWindowProc (hwnd, iMsg, wParam, lParam);		// The DefWindowProc function calls the default window procedure to provide default processing for any window messages that an application does not process. 
															// This function ensures that every message is processed. DefWindowProc is called with the same parameters received by the WndProc. 
}															// End of the WndProc
///////////////////////////////////////////
int main(int a,char**args){ HINSTANCE hInstance=NULL;

    HWND        hwnd;										// HWND - a HWND is a handle to a window.  This is used to keep track of a certain window.  Programs can have multiple windows.  We only have one though.
    WNDCLASSEX  wndclass;									// WNDCLASSEX - This variable will hold all the information about the window (The name, icon, cursor, color, menu bar...)
    wndclass.cbSize        = sizeof (wndclass);				// Here we set the size of the wndclass. You will see this a lot in windows, it's kinda odd.  We use the "sizeof()" function to tell windows the size of our class.
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;		// The style we want is Verticle-Redraw and Horizontal-Redraw
    wndclass.lpfnWndProc   = &WndProc;						// Here is where we assing our CALLBACK function.  Remember up above our function "WndProc"?  This just tells windows which function it needs to call to check for window messages.
    wndclass.cbClsExtra    = 0;								// We don't want to allocate any extra bytes for a class (useless for us)
    wndclass.cbWndExtra    = 0;								// Another useless thing for us.  I believe these last 2 are initialized to 0 anyway.
    wndclass.hInstance     = hInstance;						// We assign our hInstance to our window.  Once again, You can have several instances of a program, so this keeps track of the current one.
	wndclass.hIcon         = 0;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszMenuName  = NULL;							// We don't have a menu, so let's set this to NULL.
    wndclass.lpszClassName = "DepthTest";				// Here we set a name for our class, to distinguish it against others.  We need to use this name for later when we create the window.					
    wndclass.hIconSm       = LoadIcon (NULL, IDI_WINLOGO);	// We want the icon to be the windows logo.  This is the icon in the top left corner of the window.
	RegisterClassEx (&wndclass);							// We need to register our windows class with the windows OS.  We also need to pass the memory address of wndclass, so we use the "&".
															// CreateWindow() returns a handle to the window, which we store in our HWND "hwnd".
    hwnd = CreateWindow ("DepthTest",							// window class name - This tells CreateWindow() to use our class up above.
						 "DepthTest",		  					// window's Title    - This will be the name on the title bar of the window.
						 WS_OVERLAPPEDWINDOW,				// window style		 - This flag tells windows to create a typical window, (options like resize, minimize, close, etc).
						 CW_USEDEFAULT,						// initial x position- The top left corner X value of the window in screen coordinates.  This flag let's windows choose for us.
						 CW_USEDEFAULT,						// initial y position- The top left corner Y value of the window in screen coordinates.  This flag let's windows choose for us.
						 640,									// initial x size	 - The bottom right corner X value of the window in screen coordinates.  This flag let's windows choose for us.
						 480,								    // initial y size	 - The bottom right corner Y value of the window in screen coordinates.  This flag let's windows choose for us.
						 NULL,								// This is the parent window handle.  Since we don't have a parent window, we set this to NULL
						 NULL,								// This is the window menu handle, but since we don't have a menu, we set this to NULL.
						 hInstance,						    // This is the programs instance handle.  We just pass it our hInstance from our WinMain().  By the way, Windows OS passes the info to WinMain(). It's all taken care of for us.
						 NULL);								// If we wanted to pass in a variable or structure to the "WndProc", we would do it here.  (I just prefer using global variables though).  I just pass in NULL.

    ShowWindow (hwnd, 1/*iCmdShow*/);						// This shows our window.  We give it our handle to our window, which now has all our windows info, and our WinMain() variable iCmdShow.
    UpdateWindow (hwnd);									// This pretty much paints our window to the screen.
//////////////////////////////////////////

	unsigned char header[54];

	char file_right[]="martian_left.bmp";
	char file_left []="martian_right.bmp";
//	char file_right[]="corridor_right.bmp";
//	char file_left []="corridor_left.bmp";

	// read right file
	int 
    handle=open(file_right, O_RDONLY | O_BINARY ) ;
	if (handle == -1){ close(handle); MessageBox(NULL, "no right" , "Info" , MB_OK); return -1;}
	read(handle,header,54);
	int x=header[18]+(int)(header[19]<<8);
	int y=header[22]+(int)(header[23]<<8);
    unsigned char *left   =(unsigned char*)malloc(x*y*4);
  	read(handle,left,x*y*3);
	close(handle);

	// read left file

	handle=open(file_left, O_RDONLY | O_BINARY) ;
	if (handle == -1)	{ close(handle); MessageBox(NULL, "no left" , "Info" , MB_OK); return -1;	}
	read(handle,header,54);
    x=header[18]+(int)(header[19]<<8);
    y=header[22]+(int)(header[23]<<8);
    unsigned char *right  =(unsigned char*)malloc(x*y*4);
  	read(handle,right,x*y*3);
	close(handle);	

	char Info[200];sprintf(Info,"Info %dx%dx%d",x,y,header[28]);

	if (right == NULL)	{ close(handle); MessageBox(NULL, Info, "no mem" , MB_OK); return -1;	}
	if (left  == NULL)	{ close(handle); MessageBox(NULL, Info, "no mem" , MB_OK); return -1;	}

	MoveWindow(hwnd,1024/2-x/2,50,x+20,y+30,true);
	//////////////////////////////////////////
	unsigned char *depth;
	BITMAPINFO BitmapInfo={0};
	ZeroMemory(&BitmapInfo, sizeof(BitmapInfo));
	BitmapInfo.bmiHeader.biHeight = y;
	BitmapInfo.bmiHeader.biWidth  = x;
	BitmapInfo.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 24;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	HDC hdcDst = GetDC(hwnd);					
	HBITMAP hbm=CreateDIBSection(0, &BitmapInfo, DIB_RGB_COLORS, (void**)&depth, NULL, NULL);
   	//////////////////////////////////////////
	HDC hdcSrc = CreateCompatibleDC(NULL);
	SelectObject(hdcSrc, hbm);
	//////////////////////////////////////////
	memset ( depth , 0, x*y*3);

	// Call Stero matching method
   	GetDepth(left,right,depth,x,y);

	// Show Result
	BitBlt(hdcDst, 0, 0, x, y, hdcSrc, 0, 0, SRCCOPY);
	UpdateWindow (hwnd);
														  
	// Save result image													  
	FILE* fn;
	if ((fn = fopen ("result.bmp","wb")) != NULL)
	{
 	 header[22]=y;
	 header[23]=y/256;

	 fwrite(header ,1,54   ,fn);
	 fwrite(depth  ,1,x*y*3,fn);
 	 fclose(fn);
	}
	else
	MessageBox(NULL, "no write" , "Info" , MB_OK); 
	//////////////////////////////////////////
	MessageBox(NULL, "Ready" , "OK" , MB_OK); 
	return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR szCmdLine, int sw){ return main( 0,NULL );}
