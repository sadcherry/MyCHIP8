#include <stdio.h>
#include <GL/glut.h>
#include "mychip8.h"

// Display size
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

int modifier = 10;

// Window size
// int display_width = SCREEN_WIDTH * modifier;
// int display_height = SCREEN_HEIGHT * modifier;

 int display_width = 640;
 int display_height = 320;
 
void display();
void reshape_window(GLsizei w, GLsizei h);
void keyboardUp(unsigned char key, int x, int y);
void keyboardDown(unsigned char key, int x, int y);

// Use new drawing method
#define DRAWWITHTEXTURE
//typedef unsigned __int8 u8;

unsigned char screenData[SCREEN_HEIGHT][SCREEN_WIDTH][3]; 
void setupTexture();

int main(int argc, char **argv) 
{		
	if(argc < 2)
	{
		printf("Usage: myChip8.exe chip8application\n\n");
		return 1;
	}

	// Load game
	if(!LoadApp(argv[1]))		
		return 1;
		
    // Setup OpenGLabclixu123
	
    glutInit(&argc, argv);          
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(display_width, display_height);
    glutInitWindowPosition(320, 320);
	glutCreateWindow("myChip8 by Laurence Muller");
	
	glutDisplayFunc(display);
	glutIdleFunc(display);
    glutReshapeFunc(reshape_window);        
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp); 

#ifdef DRAWWITHTEXTURE
	setupTexture();			
#endif	

	glutMainLoop(); 

	return 0;
}

// Setup Texture
void setupTexture()
{
	int x, y;
	// Clear screen
	for(y = 0; y < SCREEN_HEIGHT; ++y)		
		for(x = 0; x < SCREEN_WIDTH; ++x)
			screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0;

	// Create a texture 
	glTexImage2D(GL_TEXTURE_2D, 0, 3, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screenData);

	// Set up the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); 

	// Enable textures
	glEnable(GL_TEXTURE_2D);
}

void updateTexture()
{	
	int x, y;
	// Update pixels
	for( y = 0; y < 32; ++y)		
		for( x = 0; x < 64; ++x)
			if(Gfx[(y * 64) + x] == 0)
				screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0;	// Disabled
			else 
				screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 255;  // Enabled

	// Update Texture
	glTexSubImage2D(GL_TEXTURE_2D, 0 ,0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screenData);

	glBegin( GL_QUADS );
		glTexCoord2d(0.0, 0.0);		glVertex2d(0.0,			  0.0);
		glTexCoord2d(1.0, 0.0); 	glVertex2d(display_width, 0.0);
		glTexCoord2d(1.0, 1.0); 	glVertex2d(display_width, display_height);
		glTexCoord2d(0.0, 1.0); 	glVertex2d(0.0,			  display_height);
	glEnd();
}

// Old gfx code
void drawPixel(int x, int y)
{
	glBegin(GL_QUADS);
		glVertex3f((x * modifier) + 0.0f,     (y * modifier) + 0.0f,	 0.0f);
		glVertex3f((x * modifier) + 0.0f,     (y * modifier) + modifier, 0.0f);
		glVertex3f((x * modifier) + modifier, (y * modifier) + modifier, 0.0f);
		glVertex3f((x * modifier) + modifier, (y * modifier) + 0.0f,	 0.0f);
	glEnd();
}

void updateQuads()
{
	int x, y;
	// Draw
	for( y = 0; y < 32; ++y)		
		for( x = 0; x < 64; ++x)
		{
			if(Gfx[(y*64) + x] == 0) 
				glColor3f(0.0f,0.0f,0.0f);			
			else 
				glColor3f(1.0f,1.0f,1.0f);

			drawPixel(x, y);
		}
}

void display()
{
	HandleOpcode();
		
	if(DrawFlag)
	{
		// Clear framebuffer
		glClear(GL_COLOR_BUFFER_BIT);
        
#ifdef DRAWWITHTEXTURE
		updateTexture();
#else
		updateQuads();		
#endif			

		// Swap buffers!
		glutSwapBuffers();    

		// Processed frame
		DrawFlag = 0;
	}
}

void reshape_window(GLsizei w, GLsizei h)
{
	glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);        
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);

	// Resize quad
	display_width = w;
	display_height = h;
}

void keyboardDown(unsigned char key, int x, int y)
{
	if(key == 27)    // esc
		;//exit(0);

	if(key == '1')		Keyboard[0x1] = 1;
	else if(key == '2')	Keyboard[0x2] = 1;
	else if(key == '3')	Keyboard[0x3] = 1;
	else if(key == '4')	Keyboard[0xC] = 1;

	else if(key == 'q')	Keyboard[0x4] = 1;
	else if(key == 'w')	Keyboard[0x5] = 1;
	else if(key == 'e')	Keyboard[0x6] = 1;
	else if(key == 'r')	Keyboard[0xD] = 1;

	else if(key == 'a')	Keyboard[0x7] = 1;
	else if(key == 's')	Keyboard[0x8] = 1;
	else if(key == 'd')	Keyboard[0x9] = 1;
	else if(key == 'f')	Keyboard[0xE] = 1;

	else if(key == 'z')	Keyboard[0xA] = 1;
	else if(key == 'x')	Keyboard[0x0] = 1;
	else if(key == 'c')	Keyboard[0xB] = 1;
	else if(key == 'v')	Keyboard[0xF] = 1;

	//printf("Press key %c\n", key);
}

void keyboardUp(unsigned char key, int x, int y)
{
	if(key == '1')		Keyboard[0x1] = 0;
	else if(key == '2')	Keyboard[0x2] = 0;
	else if(key == '3')	Keyboard[0x3] = 0;
	else if(key == '4')	Keyboard[0xC] = 0;

	else if(key == 'q')	Keyboard[0x4] = 0;
	else if(key == 'w')	Keyboard[0x5] = 0;
	else if(key == 'e')	Keyboard[0x6] = 0;
	else if(key == 'r')	Keyboard[0xD] = 0;

	else if(key == 'a')	Keyboard[0x7] = 0;
	else if(key == 's')	Keyboard[0x8] = 0;
	else if(key == 'd')	Keyboard[0x9] = 0;
	else if(key == 'f')	Keyboard[0xE] = 0;

	else if(key == 'z')	Keyboard[0xA] = 0;
	else if(key == 'x')	Keyboard[0x0] = 0;
	else if(key == 'c')	Keyboard[0xB] = 0;
	else if(key == 'v')	Keyboard[0xF] = 0;
}
