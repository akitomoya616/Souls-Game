//using library instead of putting all the definition into this main.c
#include "CSCIx229.h"

int th=90;          //  Azimuth of view angle
int ph=0;          //  Elevation of view angle
int axes=0;        //  Display axes
int mode=2;        //  What to display
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio

double screen_height=1; //  height of the screen
double screen_width=1; //  width of the screen

double dim=5.0;   //  Size of world
const char* text[] = {"Orthogonal","Perspective","First Person"};
//coordinates for the eye and the unit direction while using first person view
double direction_x=0;
double direction_y=0;
double direction_z=-1;
double Eye_x=0;
double Eye_y=0.4;
double Eye_z=0;
//movement unit to help the camera move in 2D
double move_x=0;
double move_z=0;

// Light values
int move=1;           // Move/Stop the lighting ball
int light     =   1;  // Lighting
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  15;  // Ambient intensity (%)
int diffuse   =  20;  // Diffuse intensity (%)
int specular  =  45;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   2;  // Elevation of light
typedef struct {float x,y,z;} vtx;
typedef struct {int A,B,C;} tri;

//texture values
int ntex=0;    //  Texture
double rep=1;  //  Repetition
unsigned int texture[50];  //  Texture names

//interactable settings
int wall_break=0;  //  Detect if illusory wall should be break
int collision_detection=1;  //  Collision detection on/off
int developer_mode=0;  //  Developer mode on/off

//transparency
int alpha=0.75;

//timer for re-assigining the texture coordinates of the fog gate in boss room
float timer=0;

//shader
int shader  = 0; //  Shader programs
#ifdef GL_VERSION_4_0
#define MODE 11
int shader4[] = {0,0,0}; //  OpenGL 4 shaders
#else
#define MODE 10
#endif
float X=0,Y=0,Z=1; //  Mandelbrot X,Y,Z
int roll=1;       //  Rolling brick texture
const float Emission[]  = {0.0,0.0,0.0,1.0};
const float Ambient[]   = {0.3,0.3,0.3,1.0};
const float Diffuse[]   = {1.0,1.0,1.0,1.0};
const float Specular[]  = {1.0,1.0,1.0,1.0};
const float Shinyness[] = {16};

/*
 * Set color
 */
//call this before glBegin!
void SetColor(float R,float G,float B)
{
   float color[] = {R,G,B,1.0};
   glColor3f(R,G,B);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,color);
}

//defining objects
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   glColor3f(1,1,1);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

//transparent golem as the boss
static void golem(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y+dy,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Set transparency
   glEnable(GL_BLEND);
   glColor4f(1,1,1,alpha);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   glDepthMask(0);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,texture[19]);

   //  Left foot
   glBegin(GL_QUADS);
   //  Front
   glNormal3f(0,0,+1);
   glTexCoord2f(0,0); glVertex3f(-3,-1,+1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,-0,+1);
   glTexCoord2f(0,1); glVertex3f(-3,-0,+1);
   //  Back
   glNormal3f(0,0,-1);
   glTexCoord2f(0,0); glVertex3f(-3,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,-0,-1);
   glTexCoord2f(0,1); glVertex3f(-3,-0,-1);
   //  Right
   glNormal3f(+1,0,0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,-0,-1);
   glTexCoord2f(0,1); glVertex3f(-1,-0,+1);
   //  Left
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(-3,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-3,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-3,-0,+1);
   glTexCoord2f(0,1); glVertex3f(-3,-0,-1);
   //  Top
   glNormal3f(0,+1,0);
   glTexCoord2f(0,0); glVertex3f(-3,-0,+1);
   glTexCoord2f(1,0); glVertex3f(-1,-0,+1);
   glTexCoord2f(1,1); glVertex3f(-1,-0,-1);
   glTexCoord2f(0,1); glVertex3f(-3,-0,-1);
   //  Bottom
   glNormal3f(0,-1,0);
   glTexCoord2f(0,0); glVertex3f(-3,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-3,-1,+1);
   //  End
   glEnd();


   //  Left knee

   glBegin(GL_QUADS);
   //  Front bottom
   glNormal3f(0,-1,3);
   glTexCoord2f(0,0); glVertex3f(-3,-0,+0.5);
   glTexCoord2f(1,0); glVertex3f(-1,-0,+0.5);
   glTexCoord2f(1,1); glVertex3f(-1,+1.5,+1);
   glTexCoord2f(0,1); glVertex3f(-3,+1.5,+1);
   //  Front up
   glNormal3f(0,1,3);
   glTexCoord2f(0,0); glVertex3f(-3,+3,+0.5);
   glTexCoord2f(1,0); glVertex3f(-1,+3,+0.5);
   glTexCoord2f(1,1); glVertex3f(-1,+1.5,+1);
   glTexCoord2f(0,1); glVertex3f(-3,+1.5,+1);

   //  Back bottom
   glNormal3f(0,-1,-3);
   glTexCoord2f(0,0); glVertex3f(-3,-0,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-0,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1.5,-1.5);
   glTexCoord2f(0,1); glVertex3f(-3,+1.5,-1.5);

   //  Back up
   glNormal3f(0,1,-3);
   glTexCoord2f(0,0); glVertex3f(-3,+3,-1);
   glTexCoord2f(1,0); glVertex3f(-1,+3,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1.5,-1.5);
   glTexCoord2f(0,1); glVertex3f(-3,+1.5,-1.5);

   glEnd();

   //  left part
   glBegin(GL_TRIANGLES);
   glNormal3f(-2.25,0.75,0);
   glTexCoord2f(1,0); glVertex3f(-3,+3,-1);//back left top
   glTexCoord2f(0,0); glVertex3f(-3,+3,+0.5);//front left top
   glTexCoord2f(0.5,1); glVertex3f(-3.5,+1.5,-0.25);//left middle

   glNormal3f(-1.875,0.25,-0.75);
   glTexCoord2f(1,0); glVertex3f(-3,+3,-1);//back left top
   glTexCoord2f(0,0); glVertex3f(-3,+1.5,-1.5);//back left middle
   glTexCoord2f(0.5,1); glVertex3f(-3.5,+1.5,-0.25);//left middle

   glNormal3f(-1.875,-0.25,-0.75);
   glTexCoord2f(0,0); glVertex3f(-3,+1.5,-1.5);//back left middle
   glTexCoord2f(1,0); glVertex3f(-3,-0,-1);//back left bottom
   glTexCoord2f(0.5,1); glVertex3f(-3.5,+1.5,-0.25);//left middle

   glNormal3f(-2.25,-0.75,0);
   glTexCoord2f(1,0); glVertex3f(-3,-0,-1);//back left bottom
   glTexCoord2f(0,0); glVertex3f(-3,-0,+0.5);//front left bottom
   glTexCoord2f(0.5,1); glVertex3f(-3.5,+1.5,-0.25);//left middle

   glNormal3f(-1.875,-0.25,0.75);
   glTexCoord2f(0,0); glVertex3f(-3,-0,+0.5);//front left bottom
   glTexCoord2f(1,0); glVertex3f(-3,+1.5,+1);//front left middle
   glTexCoord2f(0.5,1); glVertex3f(-3.5,+1.5,-0.25);//left middle

   glNormal3f(-1.875,0.25,0.75);
   glTexCoord2f(1,0); glVertex3f(-3,+1.5,+1);//front left middle
   glTexCoord2f(0,0); glVertex3f(-3,+3,+0.5);//front left top
   glTexCoord2f(0.5,1); glVertex3f(-3.5,+1.5,-0.25);//left middle

   glEnd();

   //  right part
   glBegin(GL_TRIANGLES);
   glNormal3f(+2.25,0.75,0);
   glTexCoord2f(1,0); glVertex3f(-1,+3,-1);//back right top
   glTexCoord2f(0,0); glVertex3f(-1,+3,+0.5);//front right top
   glTexCoord2f(0.5,1); glVertex3f(-0.5,+1.5,-0.25);//right middle

   glNormal3f(+1.875,0.25,-0.75);
   glTexCoord2f(1,0); glVertex3f(-1,+3,-1);//back right top
   glTexCoord2f(0,0); glVertex3f(-1,+1.5,-1.5);//back right middle
   glTexCoord2f(0.5,1); glVertex3f(-0.5,+1.5,-0.25);//right middle

   glNormal3f(+1.875,-0.25,-0.75);
   glTexCoord2f(0,0); glVertex3f(-1,+1.5,-1.5);//back right middle
   glTexCoord2f(1,0); glVertex3f(-1,-0,-1);//back right bottom
   glTexCoord2f(0.5,1); glVertex3f(-0.5,+1.5,-0.25);//right middle

   glNormal3f(+2.25,-0.75,0);
   glTexCoord2f(1,0); glVertex3f(-1,-0,-1);//back right bottom
   glTexCoord2f(0,0); glVertex3f(-1,-0,+0.5);//front right bottom
   glTexCoord2f(0.5,1); glVertex3f(-0.5,+1.5,-0.25);//right middle

   glNormal3f(+1.875,-0.25,0.75);
   glTexCoord2f(0,0); glVertex3f(-1,-0,+0.5);//front right bottom
   glTexCoord2f(1,0); glVertex3f(-1,+1.5,+1);//front right middle
   glTexCoord2f(0.5,1); glVertex3f(-0.5,+1.5,-0.25);//right middle

   glNormal3f(+1.875,0.25,0.75);
   glTexCoord2f(1,0); glVertex3f(-1,+1.5,+1);//front right middle
   glTexCoord2f(0,0); glVertex3f(-1,+3,+0.5);//front right top
   glTexCoord2f(0.5,1); glVertex3f(-0.5,+1.5,-0.25);//right middle

   glEnd();



   //  Right foot
   glBegin(GL_QUADS);
   //  Front
   glNormal3f(0,0,+1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+3,-1,+1);
   glTexCoord2f(1,1); glVertex3f(+3,-0,+1);
   glTexCoord2f(0,1); glVertex3f(+1,-0,+1);
   //  Back
   glNormal3f(0,0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+3,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+3,-0,-1);
   glTexCoord2f(0,1); glVertex3f(+1,-0,-1);
   //  Right
   glNormal3f(+1,0,0);
   glTexCoord2f(0,0); glVertex3f(+3,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+3,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+3,-0,-1);
   glTexCoord2f(0,1); glVertex3f(+3,-0,+1);
   //  Left
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,-0,+1);
   glTexCoord2f(0,1); glVertex3f(+1,-0,-1);
   //  Top
   glNormal3f(0,+1,0);
   glTexCoord2f(0,0); glVertex3f(+1,-0,+1);
   glTexCoord2f(1,0); glVertex3f(+3,-0,+1);
   glTexCoord2f(1,1); glVertex3f(+3,-0,-1);
   glTexCoord2f(0,1); glVertex3f(+1,-0,-1);
   //  Bottom
   glNormal3f(0,-1,0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+3,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+3,-1,+1);
   glTexCoord2f(0,1); glVertex3f(+1,-1,+1);
   //  End
   glEnd();

   //  Right knee

   glBegin(GL_QUADS);
   //  Front bottom
   glNormal3f(0,-1,3);
   glTexCoord2f(0,0); glVertex3f(+1,-0,+0.5);
   glTexCoord2f(1,0); glVertex3f(+3,-0,+0.5);
   glTexCoord2f(1,1); glVertex3f(+3,+1.5,+1);
   glTexCoord2f(0,1); glVertex3f(+1,+1.5,+1);
   //  Front up
   glNormal3f(0,1,3);
   glTexCoord2f(0,0); glVertex3f(+1,+3,+0.5);
   glTexCoord2f(1,0); glVertex3f(+3,+3,+0.5);
   glTexCoord2f(1,1); glVertex3f(+3,+1.5,+1);
   glTexCoord2f(0,1); glVertex3f(+1,+1.5,+1);

   //  Back bottom
   glNormal3f(0,-1,-3);
   glTexCoord2f(0,0); glVertex3f(+1,-0,-1);
   glTexCoord2f(1,0); glVertex3f(+3,-0,-1);
   glTexCoord2f(1,1); glVertex3f(+3,+1.5,-1.5);
   glTexCoord2f(0,1); glVertex3f(+1,+1.5,-1.5);

   //  Back up
   glNormal3f(0,1,-3);
   glTexCoord2f(0,0); glVertex3f(+1,+3,-1);
   glTexCoord2f(1,0); glVertex3f(+3,+3,-1);
   glTexCoord2f(1,1); glVertex3f(+3,+1.5,-1.5);
   glTexCoord2f(0,1); glVertex3f(+1,+1.5,-1.5);

   glEnd();

   //  left part
   glBegin(GL_TRIANGLES);
   glNormal3f(-2.25,0.75,0);
   glTexCoord2f(1,0); glVertex3f(+1,+3,-1);//back left top
   glTexCoord2f(0,0); glVertex3f(+1,+3,+0.5);//front left top
   glTexCoord2f(0.5,1); glVertex3f(+0.5,+1.5,-0.25);//left middle

   glNormal3f(-1.875,0.25,-0.75);
   glTexCoord2f(1,0); glVertex3f(+1,+3,-1);//back left top
   glTexCoord2f(0,0); glVertex3f(+1,+1.5,-1.5);//back left middle
   glTexCoord2f(0.5,1); glVertex3f(+0.5,+1.5,-0.25);//left middle

   glNormal3f(-1.875,-0.25,-0.75);
   glTexCoord2f(0,0); glVertex3f(+1,+1.5,-1.5);//back left middle
   glTexCoord2f(1,0); glVertex3f(+1,-0,-1);//back left bottom
   glTexCoord2f(0.5,1); glVertex3f(+0.5,+1.5,-0.25);//left middle

   glNormal3f(-2.25,-0.75,0);
   glTexCoord2f(1,0); glVertex3f(+1,-0,-1);//back left bottom
   glTexCoord2f(0,0); glVertex3f(+1,-0,+0.5);//front left bottom
   glTexCoord2f(0.5,1); glVertex3f(+0.5,+1.5,-0.25);//left middle

   glNormal3f(-1.875,-0.25,0.75);
   glTexCoord2f(0,0); glVertex3f(+1,-0,+0.5);//front left bottom
   glTexCoord2f(1,0); glVertex3f(+1,+1.5,+1);//front left middle
   glTexCoord2f(0.5,1); glVertex3f(+0.5,+1.5,-0.25);//left middle

   glNormal3f(-1.875,0.25,0.75);
   glTexCoord2f(1,0); glVertex3f(+1,+1.5,+1);//front left middle
   glTexCoord2f(0,0); glVertex3f(+1,+3,+0.5);//front left top
   glTexCoord2f(0.5,1); glVertex3f(+0.5,+1.5,-0.25);//left middle

   glEnd();

   //  right part
   glBegin(GL_TRIANGLES);
   glNormal3f(+2.25,0.75,0);
   glTexCoord2f(1,0); glVertex3f(+3,+3,-1);//back right top
   glTexCoord2f(0,0); glVertex3f(+3,+3,+0.5);//front right top
   glTexCoord2f(0.5,1); glVertex3f(+3.5,+1.5,-0.25);//right middle

   glNormal3f(+1.875,0.25,-0.75);
   glTexCoord2f(1,0); glVertex3f(+3,+3,-1);//back right top
   glTexCoord2f(0,0); glVertex3f(+3,+1.5,-1.5);//back right middle
   glTexCoord2f(0.5,1); glVertex3f(+3.5,+1.5,-0.25);//right middle

   glNormal3f(+1.875,-0.25,-0.75);
   glTexCoord2f(0,0); glVertex3f(+3,+1.5,-1.5);//back right middle
   glTexCoord2f(1,0); glVertex3f(+3,-0,-1);//back right bottom
   glTexCoord2f(0.5,1); glVertex3f(+3.5,+1.5,-0.25);//right middle

   glNormal3f(+2.25,-0.75,0);
   glTexCoord2f(1,0); glVertex3f(+3,-0,-1);//back right bottom
   glTexCoord2f(0,0); glVertex3f(+3,-0,+0.5);//front right bottom
   glTexCoord2f(0.5,1); glVertex3f(+3.5,+1.5,-0.25);//right middle

   glNormal3f(+1.875,-0.25,0.75);
   glTexCoord2f(0,0); glVertex3f(+3,-0,+0.5);//front left bottom
   glTexCoord2f(1,0); glVertex3f(+3,+1.5,+1);//front left middle
   glTexCoord2f(0.5,1); glVertex3f(+3.5,+1.5,-0.25);//left middle

   glNormal3f(+1.875,0.25,0.75);
   glTexCoord2f(1,0); glVertex3f(+3,+1.5,+1);//front right middle
   glTexCoord2f(0,0); glVertex3f(+3,+3,+0.5);//front right top
   glTexCoord2f(0.5,1); glVertex3f(+3.5,+1.5,-0.25);//right middle

   glEnd();


   //  Body
   //bottom that connects two legs
   glBegin(GL_QUADS);
   glNormal3f(0,-1,0);
   glTexCoord2f(0,0); glVertex3f(-3,+3,+0.5);//front left bottom
   glTexCoord2f(1,0); glVertex3f(+3,+3,+0.5);//front right bottom
   glTexCoord2f(1,1); glVertex3f(+3,+3,-1);//back right bottom
   glTexCoord2f(0,1); glVertex3f(-3,+3,-1);//back left bottom
   //top
   glNormal3f(0,1,0);
   glTexCoord2f(0,0); glVertex3f(-3,+7,+0.5);//front left top
   glTexCoord2f(1,0); glVertex3f(+3,+7,+0.5);//front right top
   glTexCoord2f(1,1); glVertex3f(+3,+7,-1);//back right top
   glTexCoord2f(0,1); glVertex3f(-3,+7,-1);//back left top
   glEnd();

   //  Front face
   glBegin(GL_TRIANGLES);
   //bottom middle
   glNormal3f(0,-9,12);
   glTexCoord2f(1,0); glVertex3f(-3,+3,+0.5);//front left bottom
   glTexCoord2f(0,0); glVertex3f(+3,+3,+0.5);//front right bottom
   glTexCoord2f(0.5,1); glVertex3f(+0,+5,+2);//front middle

   //bottom left
   glNormal3f(-3,-1.5,8);
   glTexCoord2f(0,0); glVertex3f(-4,+5,+0.5);//front left middle
   glTexCoord2f(1,0); glVertex3f(-3,+3,+0.5);//front left bottom
   glTexCoord2f(0.5,1); glVertex3f(+0,+5,+2);//front middle

   //top left
   glNormal3f(-3,1.5,8);
   glTexCoord2f(1,0); glVertex3f(-3,+7,+0.5);//front left top
   glTexCoord2f(0,0); glVertex3f(-4,+5,+0.5);//front left middle
   glTexCoord2f(0.5,1); glVertex3f(+0,+5,+2);//front middle

   //top middle
   glNormal3f(0,9,12);
   glTexCoord2f(1,0); glVertex3f(-3,+7,+0.5);//front left top
   glTexCoord2f(0,0); glVertex3f(+3,+7,+0.5);//front right top
   glTexCoord2f(0.5,1); glVertex3f(+0,+5,+2);//front middle

   //top right
   glNormal3f(3,1.5,8);
   glTexCoord2f(0,0); glVertex3f(+4,+5,+0.5);//front right middle
   glTexCoord2f(1,0); glVertex3f(+3,+7,+0.5);//front right top
   glTexCoord2f(0.5,1); glVertex3f(+0,+5,+2);//front middle

   //bottom right
   glNormal3f(3,-1.5,8);
   glTexCoord2f(1,0); glVertex3f(+4,+5,+0.5);//front right middle
   glTexCoord2f(0,0); glVertex3f(+3,+3,+0.5);//front right bottom
   glTexCoord2f(0.5,1); glVertex3f(+0,+5,+2);//front middle

   glEnd();

   //  Back face
   glBegin(GL_TRIANGLES);
   //bottom middle
   glNormal3f(0,-9,-12);
   glTexCoord2f(1,0); glVertex3f(-3,+3,-1);//back left bottom
   glTexCoord2f(0,0); glVertex3f(+3,+3,-1);//back right bottom
   glTexCoord2f(0.5,1); glVertex3f(+0,+5,-2);//back middle

   //bottom left
   glNormal3f(-3,-1.5,-8);
   glTexCoord2f(0,0); glVertex3f(-4,+5,-1);//back left middle
   glTexCoord2f(1,0); glVertex3f(-3,+3,-1);//back left bottom
   glTexCoord2f(0.5,1); glVertex3f(+0,+5,-2);//back middle

   //top left
   glNormal3f(-3,1.5,-8);
   glTexCoord2f(1,0); glVertex3f(-3,+7,-1);//back left top
   glTexCoord2f(0,0); glVertex3f(-4,+5,-1);//back left middle
   glTexCoord2f(0.5,1); glVertex3f(+0,+5,-2);//back middle

   //top middle
   glNormal3f(0,9,-12);
   glTexCoord2f(1,0); glVertex3f(-3,+7,-1);//back left top
   glTexCoord2f(0,0); glVertex3f(+3,+7,-1);//back right top
   glTexCoord2f(0.5,1); glVertex3f(+0,+5,-2);//back middle

   //top right
   glNormal3f(3,1.5,-8);
   glTexCoord2f(0,0); glVertex3f(+4,+5,-1);//back right middle
   glTexCoord2f(1,0); glVertex3f(+3,+7,-1);//back right top
   glTexCoord2f(0.5,1); glVertex3f(+0,+5,-2);//back middle

   //bottom right
   glNormal3f(3,-1.5,-8);
   glTexCoord2f(1,0); glVertex3f(+4,+5,-1);//back right middle
   glTexCoord2f(0,0); glVertex3f(+3,+3,-1);//back right bottom
   glTexCoord2f(0.5,1); glVertex3f(+0,+5,-2);//back middle

   glEnd();

   //  right body
   glBegin(GL_QUADS);
   glNormal3f(3,1.5,0);
   glTexCoord2f(0,0); glVertex3f(+3,+3,-1);//back right bottom
   glTexCoord2f(1,0); glVertex3f(+3,+3,+0.5);//front right bottom
   glTexCoord2f(1,1); glVertex3f(+4,+5,+0.5);//front right middle
   glTexCoord2f(0,1); glVertex3f(+4,+5,-1);//back right middle
   glEnd();

   //  left body
   glBegin(GL_QUADS);
   glNormal3f(-3,1.5,0);
   glTexCoord2f(0,0); glVertex3f(-3,+3,-1);//back left bottom
   glTexCoord2f(1,0); glVertex3f(-3,+3,+0.5);//front left bottom
   glTexCoord2f(1,1); glVertex3f(-4,+5,+0.5);//front left middle
   glTexCoord2f(0,1); glVertex3f(-4,+5,-1);//back left middle
   glEnd();



   //  Right arm
   //bottom
   glBegin(GL_POLYGON);
   glNormal3f(-0.75,-1.5,0); 
   glTexCoord2f(0.2,0); glVertex3f(+4,+5,-1);//back right middle - body
   glTexCoord2f(0.8,0); glVertex3f(+4,+5,+0.5);//front right middle - body

   glTexCoord2f(1,0.8); glVertex3f(+5,+4.5,+1);//front right middle - arm middle

   glTexCoord2f(0.8,1); glVertex3f(+6,+4,+0.5);//front right middle - arm end
   glTexCoord2f(0.2,1); glVertex3f(+6,+4,-1);//back right middle - arm end

   glTexCoord2f(0,0.8); glVertex3f(+5,+4.5,-1.5);//back right middle - arm middle

   glEnd();

   //top
   glBegin(GL_POLYGON);
   glNormal3f(0.75,1.5,0);
   glTexCoord2f(0.2,0); glVertex3f(+3,+7,-1);//back right top - body
   glTexCoord2f(0.8,0); glVertex3f(+3,+7,+0.5);//front right top - body

   glTexCoord2f(1,0.8); glVertex3f(+5,+6,+1);//front right top - arm middle

   glTexCoord2f(0.8,1); glVertex3f(+6,+5,+0.5);//front right top - arm end
   glTexCoord2f(0.2,1); glVertex3f(+6,+5,-1);//back right top - arm end

   glTexCoord2f(0,0.8); glVertex3f(+5,+6,-1.5);//back right top - arm middle

   glEnd();

   //front left
   glBegin(GL_QUADS);
   glNormal3f(-0.75,0,1.5);
   glTexCoord2f(0,0); glVertex3f(+4,+5,+0.5);//front right middle - body
   glTexCoord2f(1,0); glVertex3f(+5,+4.5,+1);//front right middle - arm middle bottom
   glTexCoord2f(1,1); glVertex3f(+5,+6,+1);//front right top - arm middle top
   glTexCoord2f(0,1); glVertex3f(+3,+7,+0.5);//front right top - body
   glEnd();

   //front right
   glBegin(GL_QUADS);
   glNormal3f(0.75,0,1.5);
   glTexCoord2f(0,0); glVertex3f(+5,+4.5,+1);//front right middle - arm middle bottom
   glTexCoord2f(1,0); glVertex3f(+6,+4,+0.5);//front right middle - arm end bottom
   glTexCoord2f(1,1); glVertex3f(+6,+5,+0.5);//front right top - arm end top
   glTexCoord2f(0,1); glVertex3f(+5,+6,+1);//front right top - arm middle top
   glEnd();

   //back left
   glBegin(GL_QUADS);
   glNormal3f(-0.75,0,-1.5);
   glTexCoord2f(0,0); glVertex3f(+4,+5,-1);//back right middle - body
   glTexCoord2f(1,0); glVertex3f(+5,+4.5,-1.5);//back right middle - arm middle bottom
   glTexCoord2f(1,1); glVertex3f(+5,+6,-1.5);//back right top - arm middle top
   glTexCoord2f(0,1); glVertex3f(+3,+7,-1);//back right top - body
   glEnd();

   //back right
   glBegin(GL_QUADS);
   glNormal3f(0.75,0,-1.5);
   glTexCoord2f(0,0); glVertex3f(+5,+4.5,-1.5);//back right middle - arm middle
   glTexCoord2f(1,0); glVertex3f(+6,+4,-1);//back right middle - arm end bottom
   glTexCoord2f(1,1); glVertex3f(+6,+5,-1);//back right top - arm end top
   glTexCoord2f(0,1); glVertex3f(+5,+6,-1.5);//back right top - arm middle
   glEnd();

   //  Right hand
   //left top
   glBegin(GL_QUADS);
   glNormal3f(-1.5,0.75,0);
   glTexCoord2f(0,0); glVertex3f(+6,+4,-1);//arm back end bottom
   glTexCoord2f(1,0); glVertex3f(+6,+4,+0.5);//arm front end bottom
   glTexCoord2f(1,1); glVertex3f(+5.5,+3,+1);//hand front middle left
   glTexCoord2f(0,1); glVertex3f(+5.5,+3,-1.5);//hand back middle left
   glEnd();
   //left bottom
   glBegin(GL_TRIANGLES);
   glNormal3f(-5,-1.25,0);
   glTexCoord2f(0,0); glVertex3f(+5.5,+3,-1.5);//hand back middle left
   glTexCoord2f(1,0); glVertex3f(+5.5,+3,+1);//hand front middle left
   glTexCoord2f(0.5,1); glVertex3f(+6,+1,+0);//hand bottom
   glEnd();
   //right top
   glBegin(GL_QUADS);
   glNormal3f(3,1.5,0);
   glTexCoord2f(0,0); glVertex3f(+6,+5,-1);//hand back top
   glTexCoord2f(1,0); glVertex3f(+6,+5,+0.5);//hand front top
   glTexCoord2f(1,1); glVertex3f(+7,+3,+1);//hand front middle right
   glTexCoord2f(0,1); glVertex3f(+7,+3,-1.5);//hand back middle right
   glEnd();
   //right bottom
   glBegin(GL_TRIANGLES);
   glNormal3f(5,-2.5,0);
   glTexCoord2f(0,0); glVertex3f(+7,+3,-1.5);//hand back middle right
   glTexCoord2f(1,0); glVertex3f(+7,+3,+1);//hand front middle right
   glTexCoord2f(0.5,1); glVertex3f(+6,+1,+0);//hand bottom
   glEnd();

   //front bottom
   glBegin(GL_TRIANGLES);
   glNormal3f(0,-0.75,3); //wrong!!
   glTexCoord2f(0,0); glVertex3f(+5.5,+3,+1);//hand front middle left
   glTexCoord2f(1,0); glVertex3f(+7,+3,+1);//hand front middle right
   glTexCoord2f(0.5,1); glVertex3f(+6,+1,+0);//hand bottom
   glEnd();
   //front top
   glBegin(GL_TRIANGLES);
   glNormal3f(0,0.75,3);
   glTexCoord2f(0,0); glVertex3f(+5.5,+3,+1);//hand front middle left
   glTexCoord2f(1,0); glVertex3f(+7,+3,+1);//hand front middle right
   glTexCoord2f(0.5,1); glVertex3f(+6,+5,+0.5);//hand front top
   glEnd();

   //back bottom
   glBegin(GL_TRIANGLES);
   glNormal3f(0,-2.25,-3);
   glTexCoord2f(0,0); glVertex3f(+5.5,+3,-1.5);//hand back middle left
   glTexCoord2f(1,0); glVertex3f(+7,+3,-1.5);//hand back middle right
   glTexCoord2f(0.5,1); glVertex3f(+6,+1,+0);//hand bottom
   glEnd();
   //back top
   glBegin(GL_TRIANGLES);
   glNormal3f(0,2.25,-3);
   glTexCoord2f(0,0); glVertex3f(+5.5,+3,-1.5);//hand back middle left
   glTexCoord2f(1,0); glVertex3f(+7,+3,-1.5);//hand back middle right
   glTexCoord2f(0.5,1); glVertex3f(+6,+5,-1);//hand back top
   glEnd();



   //  Left arm
   //bottom
   glBegin(GL_POLYGON);
   glNormal3f(0.75,-1.5,0); 
   glTexCoord2f(0.2,0); glVertex3f(-4,+5,-1);//back left middle - body
   glTexCoord2f(0.8,0); glVertex3f(-4,+5,+0.5);//front left middle - body

   glTexCoord2f(1,0.8); glVertex3f(-5,+4.5,+1);//front left middle - arm middle

   glTexCoord2f(0.8,1); glVertex3f(-6,+4,+0.5);//front left middle - arm end
   glTexCoord2f(0.2,1); glVertex3f(-6,+4,-1);//back left middle - arm end

   glTexCoord2f(0,0.8); glVertex3f(-5,+4.5,-1.5);//back left middle - arm middle

   glEnd();

   //top
   glBegin(GL_POLYGON);
   glNormal3f(-0.75,1.5,0);
   glTexCoord2f(0.2,0); glVertex3f(-3,+7,-1);//back right top - body
   glTexCoord2f(0.8,0); glVertex3f(-3,+7,+0.5);//front right top - body

   glTexCoord2f(1,0.8); glVertex3f(-5,+6,+1);//front right top - arm middle

   glTexCoord2f(0.8,1); glVertex3f(-6,+5,+0.5);//front right top - arm end
   glTexCoord2f(0.2,1); glVertex3f(-6,+5,-1);//back right top - arm end

   glTexCoord2f(0,0.8); glVertex3f(-5,+6,-1.5);//back right top - arm middle

   glEnd();

   //front left
   glBegin(GL_QUADS);
   glNormal3f(0.75,0,1.5);
   glTexCoord2f(0,0); glVertex3f(-4,+5,+0.5);//front right middle - body
   glTexCoord2f(1,0); glVertex3f(-5,+4.5,+1);//front right middle - arm middle bottom
   glTexCoord2f(1,1); glVertex3f(-5,+6,+1);//front right top - arm middle top
   glTexCoord2f(0,1); glVertex3f(-3,+7,+0.5);//front right top - body
   glEnd();

   //front right
   glBegin(GL_QUADS);
   glNormal3f(-0.75,0,1.5);
   glTexCoord2f(0,0); glVertex3f(-5,+4.5,+1);//front right middle - arm middle bottom
   glTexCoord2f(1,0); glVertex3f(-6,+4,+0.5);//front right middle - arm end bottom
   glTexCoord2f(1,1); glVertex3f(-6,+5,+0.5);//front right top - arm end top
   glTexCoord2f(0,1); glVertex3f(-5,+6,+1);//front right top - arm middle top
   glEnd();

   //back left
   glBegin(GL_QUADS);
   glNormal3f(0.75,0,-1.5);
   glTexCoord2f(0,0); glVertex3f(-4,+5,-1);//back right middle - body
   glTexCoord2f(1,0); glVertex3f(-5,+4.5,-1.5);//back right middle - arm middle bottom
   glTexCoord2f(1,1); glVertex3f(-5,+6,-1.5);//back right top - arm middle top
   glTexCoord2f(0,1); glVertex3f(-3,+7,-1);//back right top - body
   glEnd();

   //back right
   glBegin(GL_QUADS);
   glNormal3f(-0.75,0,-1.5);
   glTexCoord2f(0,0); glVertex3f(-5,+4.5,-1.5);//back right middle - arm middle
   glTexCoord2f(1,0); glVertex3f(-6,+4,-1);//back right middle - arm end bottom
   glTexCoord2f(1,1); glVertex3f(-6,+5,-1);//back right top - arm end top
   glTexCoord2f(0,1); glVertex3f(-5,+6,-1.5);//back right top - arm middle
   glEnd();

   //  Left hand
   //right top
   glBegin(GL_QUADS);
   glNormal3f(1.5,0.75,0);
   glTexCoord2f(0,0); glVertex3f(-6,+4,-1);//arm back end bottom
   glTexCoord2f(1,0); glVertex3f(-6,+4,+0.5);//arm front end bottom
   glTexCoord2f(1,1); glVertex3f(-5.5,+3,+1);//hand front middle left
   glTexCoord2f(0,1); glVertex3f(-5.5,+3,-1.5);//hand back middle left
   glEnd();
   //right bottom
   glBegin(GL_TRIANGLES);
   glNormal3f(5,-1.25,0);
   glTexCoord2f(0,0); glVertex3f(-5.5,+3,-1.5);//hand back middle left
   glTexCoord2f(1,0); glVertex3f(-5.5,+3,+1);//hand front middle left
   glTexCoord2f(0.5,1); glVertex3f(-6,+1,+0);//hand bottom
   glEnd();
   //left top
   glBegin(GL_QUADS);
   glNormal3f(-3,1.5,0);
   glTexCoord2f(0,0); glVertex3f(-6,+5,-1);//hand back top
   glTexCoord2f(1,0); glVertex3f(-6,+5,+0.5);//hand front top
   glTexCoord2f(1,1); glVertex3f(-7,+3,+1);//hand front middle right
   glTexCoord2f(0,1); glVertex3f(-7,+3,-1.5);//hand back middle right
   glEnd();
   //left bottom
   glBegin(GL_TRIANGLES);
   glNormal3f(-5,-2.5,0);
   glTexCoord2f(0,0); glVertex3f(-7,+3,-1.5);//hand back middle right
   glTexCoord2f(1,0); glVertex3f(-7,+3,+1);//hand front middle right
   glTexCoord2f(0.5,1); glVertex3f(-6,+1,+0);//hand bottom
   glEnd();

   //front bottom
   glBegin(GL_TRIANGLES);
   glNormal3f(0,-0.75,3); //wrong!!
   glTexCoord2f(0,0); glVertex3f(-5.5,+3,+1);//hand front middle left
   glTexCoord2f(1,0); glVertex3f(-7,+3,+1);//hand front middle right
   glTexCoord2f(0.5,1); glVertex3f(-6,+1,+0);//hand bottom
   glEnd();
   //front top
   glBegin(GL_TRIANGLES);
   glNormal3f(0,0.75,3);
   glTexCoord2f(0,0); glVertex3f(-5.5,+3,+1);//hand front middle left
   glTexCoord2f(1,0); glVertex3f(-7,+3,+1);//hand front middle right
   glTexCoord2f(0.5,1); glVertex3f(-6,+5,+0.5);//hand front top
   glEnd();

   //back bottom
   glBegin(GL_TRIANGLES);
   glNormal3f(0,-2.25,-3);
   glTexCoord2f(0,0); glVertex3f(-5.5,+3,-1.5);//hand back middle left
   glTexCoord2f(1,0); glVertex3f(-7,+3,-1.5);//hand back middle right
   glTexCoord2f(0.5,1); glVertex3f(-6,+1,+0);//hand bottom
   glEnd();
   //back top
   glBegin(GL_TRIANGLES);
   glNormal3f(0,2.25,-3);
   glTexCoord2f(0,0); glVertex3f(-5.5,+3,-1.5);//hand back middle left
   glTexCoord2f(1,0); glVertex3f(-7,+3,-1.5);//hand back middle right
   glTexCoord2f(0.5,1); glVertex3f(-6,+5,-1);//hand back top
   glEnd();


   //    Hexagon crystal core
   //front
   glBindTexture(GL_TEXTURE_2D,texture[7]);
   //Crystal top
   //front right
   glBegin(GL_TRIANGLES);
   glNormal3f(0.5,1,0.5);
   glTexCoord2f(0.5,1.0); glVertex3d(+0.0, +6.0, +0.0);  //top vertex
   glTexCoord2f(1.0,0.0); glVertex3d(+0.0, +5.5, +1.0);  //left vertex
   glTexCoord2f(0.0,0.0); glVertex3d(+1.0, +5.5, +0.0);  //right vertex
   glEnd();
   //front left
   glBegin(GL_TRIANGLES);
   glNormal3f(-0.5,1,0.5);
   glTexCoord2f(0.5,1.0); glVertex3d(+0.0, +6.0, +0.0);  //top vertex
   glTexCoord2f(1.0,0.0); glVertex3d(+0.0, +5.5, +1.0);  //right vertex
   glTexCoord2f(0.0,0.0); glVertex3d(-1.0, +5.5, +0.0);  //left vertex
   glEnd();
   //back left
   glBegin(GL_TRIANGLES);
   glNormal3f(-0.5,1,-0.5);
   glTexCoord2f(0.5,1.0); glVertex3d(+0.0, +6.0, +0.0);  //top vertex
   glTexCoord2f(1.0,0.0); glVertex3d(+0.0, +5.5, -1.0);  //right vertex
   glTexCoord2f(0.0,0.0); glVertex3d(-1.0, +5.5, +0.0);  //left vertex
   glEnd();
   //back right
   glBegin(GL_TRIANGLES);
   glNormal3f(0.5,1,-0.5);
   glTexCoord2f(0.5,1.0); glVertex3d(+0.0, +6.0, +0.0);  //top vertex
   glTexCoord2f(1.0,0.0); glVertex3d(+0.0, +5.5, -1.0);  //left vertex
   glTexCoord2f(0.0,0.0); glVertex3d(+1.0, +5.5, +0.0);  //left vertex
   glEnd();

   //Crystal middle
   glBegin(GL_QUADS);
   //front right
   glNormal3f(+1,0,+1);
   glTexCoord2f(0.0,0.0); glVertex3d(+0.0, +4.5, +1.0);  //left bottom
   glTexCoord2f(1.0,0.0); glVertex3d(+1.0, +4.5, +0.0);  //right bottom
   glTexCoord2f(1.0,1.0); glVertex3d(+1.0, +5.5, +0.0);  //right top
   glTexCoord2f(0.0,1.0); glVertex3d(+0.0, +5.5, +1.0);  //left top

   //front left
   glNormal3f(-1,0,+1);
   glTexCoord2f(0.0,0.0); glVertex3d(+0.0, +4.5, +1.0);  //right bottom
   glTexCoord2f(1.0,0.0); glVertex3d(-1.0, +4.5, +0.0);  //left bottom
   glTexCoord2f(1.0,1.0); glVertex3d(-1.0, +5.5, +0.0);  //left top
   glTexCoord2f(0.0,1.0); glVertex3d(+0.0, +5.5, +1.0);  //right top

   //back left
   glNormal3f(-1,0,-1);
   glTexCoord2f(0.0,0.0); glVertex3d(+0.0, +4.5, -1.0);  //right bottom
   glTexCoord2f(1.0,0.0); glVertex3d(-1.0, +4.5, +0.0);  //left bottom
   glTexCoord2f(1.0,1.0); glVertex3d(-1.0, +5.5, +0.0);  //left top
   glTexCoord2f(0.0,1.0); glVertex3d(+0.0, +5.5, -1.0);  //right top

   //back right
   glNormal3f(+1,0,-1);
   glTexCoord2f(0.0,0.0); glVertex3d(+0.0, +4.5, -1.0);  //left bottom
   glTexCoord2f(1.0,0.0); glVertex3d(+1.0, +4.5, +0.0);  //right bottom
   glTexCoord2f(1.0,1.0); glVertex3d(+1.0, +5.5, +0.0);  //right top
   glTexCoord2f(0.0,1.0); glVertex3d(+0.0, +5.5, -1.0);  //left top
   glEnd();

   //Crystal bottom
   //front right
   glBegin(GL_TRIANGLES);
   glNormal3f(0.5,-1,0.5);
   glTexCoord2f(0.5,1.0); glVertex3d(+0.0, +4.0, +0.0);  //bottom vertex
   glTexCoord2f(1.0,0.0); glVertex3d(+0.0, +4.5, +1.0);  //left vertex
   glTexCoord2f(0.0,0.0); glVertex3d(+1.0, +4.5, +0.0);  //right vertex
   glEnd();
   //front left
   glBegin(GL_TRIANGLES);
   glNormal3f(-0.5,-1,0.5);
   glTexCoord2f(0.5,1.0); glVertex3d(+0.0, +4.0, +0.0);  //bottom vertex
   glTexCoord2f(1.0,0.0); glVertex3d(+0.0, +4.5, +1.0);  //right vertex
   glTexCoord2f(0.0,0.0); glVertex3d(-1.0, +4.5, +0.0);  //left vertex
   glEnd();
   //back left
   glBegin(GL_TRIANGLES);
   glNormal3f(-0.5,-1,-0.5);
   glTexCoord2f(0.5,1.0); glVertex3d(+0.0, +4.0, +0.0);  //bottom vertex
   glTexCoord2f(1.0,0.0); glVertex3d(+0.0, +4.5, -1.0);  //right vertex
   glTexCoord2f(0.0,0.0); glVertex3d(-1.0, +4.5, +0.0);  //left vertex
   glEnd();
   //back right
   glBegin(GL_TRIANGLES);
   glNormal3f(0.5,-1,-0.5);
   glTexCoord2f(0.5,1.0); glVertex3d(+0.0, +4.0, +0.0);  //bottom vertex
   glTexCoord2f(1.0,0.0); glVertex3d(+0.0, +4.5, -1.0);  //left vertex
   glTexCoord2f(0.0,0.0); glVertex3d(+1.0, +4.5, +0.0);  //right vertex
   glEnd();

   glDisable(GL_BLEND);
   glDepthMask(1);
   glDisable(GL_TEXTURE_2D);
   //  Undo transofrmations
   glPopMatrix();
}

static void sphere(double x,double y,double z,double r,double rotate)
{
   const int d=15;
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //rotate the sphere around y axis
   glRotated(rotate,0,1,0);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);

   //assign all phase to texture[7]
   glBindTexture(GL_TEXTURE_2D,texture[7]);

   //  Latitude bands
   for (int ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=d)
      {
         //assign texcoord to vertex using floating values, otherwise weird things would happen
         glTexCoord2f((float)th/360,((float)ph+180)/180); Vertex(th,ph);
         glTexCoord2f((float)th/360,((float)ph+180+d)/180); Vertex(th,ph+d);
      }
      glEnd();
   }
   //  Undo transformations
   glPopMatrix();
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
}

static void spear(double x,double y,double z,double r,double rotate)
{
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //rotate the sphere around y axis
   glRotated(rotate,0,0,1);

   //  Spike
   const double nose=4;
   const double cone=0.2;
   const double wid=0.4;
   const double tail=-0.30;
   
   //  Enable textures
   glEnable(GL_TEXTURE_2D);

   //assign all phase to texture[33]
   glBindTexture(GL_TEXTURE_2D,texture[33]);
   SetColor(1,1,1);

   //stick
   glBegin(GL_QUAD_STRIP);
   for (int th=0;th<=360;th+=10)
   {
      //perfectly calculated normal vector
      glNormal3f(-(tail-1.0)*wid*(Sin(th+10)-Sin(th)),0,(tail-1.0)*wid*(Cos(th+10)-Cos(th)));
      glTexCoord2f((float)(th+90)/360,1); glVertex3d(0.1*Cos(th),1.0,0.1*Sin(th));
      glTexCoord2f((float)(th+90)/360,0); glVertex3d(0.1*Cos(th),tail,0.1*Sin(th));
      
   }
   glEnd();

   // end circle for stick
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,1,0);
   glTexCoord2f(0.5,1); glVertex3d(0.0, 1.0, 0.0);
   for (int th=0;th<=360;th+=10){
      glTexCoord2f((Cos(th+90) + 1.0)*0.5,(Sin(th+90) + 1.0)*0.5); glVertex3d(0.1*Cos(th),1.0,0.1*Sin(th));
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[6]);
   //down spike
   glBegin(GL_TRIANGLE_STRIP);
   for (int th=0;th<=360;th+=3){
      glNormal3f(Cos(th),-cone,Sin(th));
      glTexCoord2f(0.5,1.0); glVertex3d(0.0, -nose, 0.0);
      glTexCoord2f(wid*Cos(th),wid*Sin(th)); glVertex3d(wid*Cos(th),-cone,wid*Sin(th));
      glTexCoord2f(wid*Cos(th+5),wid*Sin(th+5)); glVertex3d(wid*Cos(th+5),-cone,wid*Sin(th+5));
   }
   glEnd();

   // end circle for spike
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,1,0);
   glTexCoord2f(0.5,1); glVertex3d(0.0, -cone, 0.0);
   for (int th=0;th<=360;th+=10){
      glTexCoord2f((Cos(th+90) + 1.0)*0.5,(Sin(th+90) + 1.0)*0.5); glVertex3d(wid*Cos(th),-cone,wid*Sin(th));
   }
   glEnd();


   //  hanger at handle part
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_QUADS);
   //front
   glNormal3f(0,0,+1);
   glTexCoord2f(0,0); glVertex3d(-0.4,-0.3,+0.5);
   glTexCoord2f(1,0); glVertex3d(+0.4,-0.3,+0.5);
   glTexCoord2f(1,1); glVertex3d(+0.4,-0.1,+0.5);
   glTexCoord2f(0,1); glVertex3d(-0.4,-0.1,+0.5);
   //another layer with opposite normal vector
   glNormal3f(0,0,-1);
   glTexCoord2f(0,0); glVertex3d(-0.4,-0.3,+0.49);
   glTexCoord2f(1,0); glVertex3d(+0.4,-0.3,+0.49);
   glTexCoord2f(1,1); glVertex3d(+0.4,-0.1,+0.49);
   glTexCoord2f(0,1); glVertex3d(-0.4,-0.1,+0.49);

   //left
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3d(-0.4,-0.3,-0.5);
   glTexCoord2f(1,0); glVertex3d(-0.4,-0.3,+0.5);
   glTexCoord2f(1,1); glVertex3d(-0.4,-0.1,+0.5);
   glTexCoord2f(0,1); glVertex3d(-0.4,-0.1,-0.5);
   //another layer with opposite normal vector
   glNormal3f(+1,0,0);
   glTexCoord2f(0,0); glVertex3d(-0.39,-0.3,-0.5);
   glTexCoord2f(1,0); glVertex3d(-0.39,-0.3,+0.5);
   glTexCoord2f(1,1); glVertex3d(-0.39,-0.1,+0.5);
   glTexCoord2f(0,1); glVertex3d(-0.39,-0.1,-0.5);

   //right
   glNormal3f(+1,0,0);
   glTexCoord2f(0,0); glVertex3d(+0.4,-0.3,-0.5);
   glTexCoord2f(1,0); glVertex3d(+0.4,-0.3,+0.5);
   glTexCoord2f(1,1); glVertex3d(+0.4,-0.1,+0.5);
   glTexCoord2f(0,1); glVertex3d(+0.4,-0.1,-0.5);
   //another layer with opposite normal vector
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3d(+0.39,-0.3,-0.5);
   glTexCoord2f(1,0); glVertex3d(+0.39,-0.3,+0.5);
   glTexCoord2f(1,1); glVertex3d(+0.39,-0.1,+0.5);
   glTexCoord2f(0,1); glVertex3d(+0.39,-0.1,-0.5);
   glEnd();

   //  hanger at spear part
   glBegin(GL_QUADS);
   //front
   glNormal3f(0,0,+1);
   glTexCoord2f(0,0); glVertex3d(-0.3,-1.9,+0.3);
   glTexCoord2f(1,0); glVertex3d(+0.3,-1.9,+0.3);
   glTexCoord2f(1,1); glVertex3d(+0.3,-1.7,+0.3);
   glTexCoord2f(0,1); glVertex3d(-0.3,-1.7,+0.3);
   //another layer with opposite normal vector
   glNormal3f(0,0,-1);
   glTexCoord2f(0,0); glVertex3d(-0.3,-1.9,+0.29);
   glTexCoord2f(1,0); glVertex3d(+0.3,-1.9,+0.29);
   glTexCoord2f(1,1); glVertex3d(+0.3,-1.7,+0.29);
   glTexCoord2f(0,1); glVertex3d(-0.3,-1.7,+0.29);

   //left
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3d(-0.3,-1.9,-0.5);
   glTexCoord2f(1,0); glVertex3d(-0.3,-1.9,+0.3);
   glTexCoord2f(1,1); glVertex3d(-0.3,-1.7,+0.3);
   glTexCoord2f(0,1); glVertex3d(-0.3,-1.7,-0.5);
   //another layer with opposite normal vector
   glNormal3f(+1,0,0);
   glTexCoord2f(0,0); glVertex3d(-0.29,-1.9,-0.5);
   glTexCoord2f(1,0); glVertex3d(-0.29,-1.9,+0.3);
   glTexCoord2f(1,1); glVertex3d(-0.29,-1.7,+0.3);
   glTexCoord2f(0,1); glVertex3d(-0.29,-1.7,-0.5);

   //right
   glNormal3f(+1,0,0);
   glTexCoord2f(0,0); glVertex3d(+0.3,-1.9,-0.5);
   glTexCoord2f(1,0); glVertex3d(+0.3,-1.9,+0.3);
   glTexCoord2f(1,1); glVertex3d(+0.3,-1.7,+0.3);
   glTexCoord2f(0,1); glVertex3d(+0.3,-1.7,-0.5);
   //another layer with opposite normal vector
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3d(+0.29,-1.9,-0.5);
   glTexCoord2f(1,0); glVertex3d(+0.29,-1.9,+0.3);
   glTexCoord2f(1,1); glVertex3d(+0.29,-1.7,+0.3);
   glTexCoord2f(0,1); glVertex3d(+0.29,-1.7,-0.5);
   glEnd();

   //  Undo transformations
   glPopMatrix();
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
}

static void treasure_box(double x,double y,double z,double r,double rotate)
{
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //rotate the sphere around y axis
   glRotated(rotate,0,1,0);

   //  circle setting
   const double cone=0.4;
   const double wid=0.3;
   const double tail=-0.4;
   
   //  Enable textures
   glEnable(GL_TEXTURE_2D);

   glBindTexture(GL_TEXTURE_2D,texture[22]);
   SetColor(1,1,1);

   //box hat
   glBegin(GL_QUAD_STRIP);
   //half cylinder OR semicircle facing up
   for (int th=-90;th<=90;th+=10)
   {
      //perfectly calculated normal vector
      glNormal3f(0,-(tail-cone)*wid*(Sin(th+10)-Sin(th)),(tail-cone)*wid*(Cos(th+10)-Cos(th)));
      //double the texture on full cylinder first so that semicircle can have one full texture, hence do *2
      //th+90 to turn the texture 90 degress so that we can see the full picture on this semicircle facing up
      //otherwise th will just give two texture that both cutted into half
      glTexCoord2f(0,(float)(th+90)/360*2); glVertex3d(cone,wid*Cos(th),wid*Sin(th));
      glTexCoord2f(1,(float)(th+90)/360*2); glVertex3d(tail,wid*Cos(th),wid*Sin(th));
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[26]);
   // left semicircle
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(-1,0,0);
   glTexCoord2f(0.5,0.5); glVertex3d(tail, 0.0, 0.0);
   for (int th=-90;th<=90;th+=10){
      //rearrange the cos and sin value fron (-1,1) to (0,1)
      glTexCoord2f((Cos(th+90) + 1.0)*0.5,(Sin(th+90) + 1.0)*0.5); glVertex3d(tail,wid*Cos(th),wid*Sin(th));
   }
   glEnd();

   // right semicircle
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(+1,0,0);
   glTexCoord2f(0.5,0.5); glVertex3d(cone, 0.0, 0.0);
   for (int th=-90;th<=90;th+=10){
      //rearrange the cos and sin value fron (-1,1) to (0,1)
      glTexCoord2f((Cos(th+90) + 1.0)*0.5,(Sin(th+90) + 1.0)*0.5); glVertex3d(cone,wid*Cos(th),wid*Sin(th));
   }
   glEnd();

   //square box
   //front
   glBindTexture(GL_TEXTURE_2D,texture[23]);
   glBegin(GL_QUADS);
   glNormal3f(0,0,+1);
   glTexCoord2f(0,0); glVertex3d(tail, -wid, +wid);
   glTexCoord2f(1,0); glVertex3d(cone, -wid, +wid);
   glTexCoord2f(1,1); glVertex3d(cone, +0, +wid);
   glTexCoord2f(0,1); glVertex3d(tail, +0, +wid);
   glEnd();

   //back
   glBindTexture(GL_TEXTURE_2D,texture[24]);
   glBegin(GL_QUADS);
   glNormal3f(0,0,-1);
   glTexCoord2f(0,0); glVertex3d(tail, -wid, -wid);
   glTexCoord2f(1,0); glVertex3d(cone, -wid, -wid);
   glTexCoord2f(1,0.97); glVertex3d(cone, +0, -wid);
   glTexCoord2f(0,0.97); glVertex3d(tail, +0, -wid);
   glEnd();

   //left
   glBindTexture(GL_TEXTURE_2D,texture[25]);
   glBegin(GL_QUADS);
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3d(tail, -wid, -wid);
   glTexCoord2f(1,0); glVertex3d(tail, -wid, +wid);
   glTexCoord2f(1,0.987); glVertex3d(tail, +0, +wid);
   glTexCoord2f(0,0.987); glVertex3d(tail, +0, -wid);
   glEnd();

   //right
   glBegin(GL_QUADS);
   glNormal3f(+1,0,0);
   glTexCoord2f(0,0); glVertex3d(cone, -wid, -wid);
   glTexCoord2f(1,0); glVertex3d(cone, -wid, +wid);
   glTexCoord2f(1,0.987); glVertex3d(cone, +0, +wid);
   glTexCoord2f(0,0.987); glVertex3d(cone, +0, -wid);
   glEnd();

   //  Undo transformations
   glPopMatrix();
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
}

static void treasure_box_open(double x,double y,double z,double r,double rotate)
{
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //rotate the sphere around y axis
   glRotated(rotate,0,1,0);

   //  circle setting
   const double cone=0.4;
   const double wid=0.3;
   const double tail=-0.4;
   
   //  Enable textures
   glEnable(GL_TEXTURE_2D);

   glBindTexture(GL_TEXTURE_2D,texture[22]);
   SetColor(1,1,1);

   //box hat
   glBegin(GL_QUAD_STRIP);
   //half cylinder OR semicircle facing up
   for (int th=-180;th<=-0;th+=10)
   {
      //perfectly calculated normal vector
      glNormal3f(0,-(tail-cone)*wid*(Sin(th+10)-Sin(th)),(tail-cone)*wid*(Cos(th+10)-Cos(th)));
      //double the texture on full cylinder first so that semicircle can have one full texture, hence do *2
      //th+90 to turn the texture 90 degress so that we can see the full picture on this semicircle facing up
      //otherwise th will just give two texture that both cutted into half
      glTexCoord2f(0,(float)(th+0)/360*2); glVertex3d(cone,wid*Cos(th)+0.3,wid*Sin(th)-0.3);
      glTexCoord2f(1,(float)(th+0)/360*2); glVertex3d(tail,wid*Cos(th)+0.3,wid*Sin(th)-0.3);
   }
   glEnd();

   //box hat again but with opposite normal vector
   glBegin(GL_QUAD_STRIP);
   //half cylinder OR semicircle facing up
   for (int th=-180;th<=-0;th+=10)
   {
      //perfectly calculated normal vector
      glNormal3f(0,(tail-cone)*wid*(Sin(th+10)-Sin(th)),-(tail-cone)*wid*(Cos(th+10)-Cos(th)));
      //double the texture on full cylinder first so that semicircle can have one full texture, hence do *2
      //th+90 to turn the texture 90 degress so that we can see the full picture on this semicircle facing up
      //otherwise th will just give two texture that both cutted into half
      glTexCoord2f(0,(float)(th+0)/360*2); glVertex3d(cone,wid*Cos(th)+0.3,wid*Sin(th)-0.299);
      glTexCoord2f(1,(float)(th+0)/360*2); glVertex3d(tail,wid*Cos(th)+0.3,wid*Sin(th)-0.299);
   }
   glEnd();


   glBindTexture(GL_TEXTURE_2D,texture[26]);
   // left semicircle
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(-1,0,0);
   glTexCoord2f(0.5,0.5); glVertex3d(tail, 0.3, -0.3);
   for (int th=-180;th<=0;th+=10){
      //rearrange the cos and sin value fron (-1,1) to (0,1)
      glTexCoord2f((Cos(th+180) + 1.0)*0.5,(Sin(th+180) + 1.0)*0.5); glVertex3d(tail,wid*Cos(th)+0.3,wid*Sin(th)-0.3);
   }
   glEnd();

   // left semicircle again with opposite normal vector
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(+1,0,0);
   glTexCoord2f(0.5,0.5); glVertex3d(tail+0.01, 0.3, -0.3);
   for (int th=-180;th<=0;th+=10){
      //rearrange the cos and sin value fron (-1,1) to (0,1)
      glTexCoord2f((Cos(th+180) + 1.0)*0.5,(Sin(th+180) + 1.0)*0.5); glVertex3d(tail+0.01,wid*Cos(th)+0.3,wid*Sin(th)-0.3);
   }
   glEnd();

   // right semicircle
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(+1,0,0);
   glTexCoord2f(0.5,0.5); glVertex3d(cone, 0.3, -0.3);
   for (int th=-180;th<=0;th+=10){
      //rearrange the cos and sin value fron (-1,1) to (0,1)
      glTexCoord2f((Cos(th+180) + 1.0)*0.5,(Sin(th+180) + 1.0)*0.5); glVertex3d(cone,wid*Cos(th)+0.3,wid*Sin(th)-0.3);
   }
   glEnd();

   // right semicircle again with opposite normal vector
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(-1,0,0);
   glTexCoord2f(0.5,0.5); glVertex3d(cone-0.01, 0.3, -0.3);
   for (int th=-180;th<=0;th+=10){
      //rearrange the cos and sin value fron (-1,1) to (0,1)
      glTexCoord2f((Cos(th+180) + 1.0)*0.5,(Sin(th+180) + 1.0)*0.5); glVertex3d(cone-0.01,wid*Cos(th)+0.3,wid*Sin(th)-0.3);
   }
   glEnd();

   //square box
   //front
   glBindTexture(GL_TEXTURE_2D,texture[23]);
   glBegin(GL_QUADS);
   glNormal3f(0,0,+1);
   glTexCoord2f(0,0); glVertex3d(tail, -wid, +wid);
   glTexCoord2f(1,0); glVertex3d(cone, -wid, +wid);
   glTexCoord2f(1,1); glVertex3d(cone, +0, +wid);
   glTexCoord2f(0,1); glVertex3d(tail, +0, +wid);
   glEnd();

   //back
   glBindTexture(GL_TEXTURE_2D,texture[24]);
   glBegin(GL_QUADS);
   glNormal3f(0,0,-1);
   glTexCoord2f(0,0); glVertex3d(tail, -wid, -wid);
   glTexCoord2f(1,0); glVertex3d(cone, -wid, -wid);
   glTexCoord2f(1,0.97); glVertex3d(cone, +0, -wid);
   glTexCoord2f(0,0.97); glVertex3d(tail, +0, -wid);
   glEnd();

   //left
   glBindTexture(GL_TEXTURE_2D,texture[25]);
   glBegin(GL_QUADS);
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3d(tail, -wid, -wid);
   glTexCoord2f(1,0); glVertex3d(tail, -wid, +wid);
   glTexCoord2f(1,0.987); glVertex3d(tail, +0, +wid);
   glTexCoord2f(0,0.987); glVertex3d(tail, +0, -wid);
   glEnd();

   //right
   glBegin(GL_QUADS);
   glNormal3f(+1,0,0);
   glTexCoord2f(0,0); glVertex3d(cone, -wid, -wid);
   glTexCoord2f(1,0); glVertex3d(cone, -wid, +wid);
   glTexCoord2f(1,0.987); glVertex3d(cone, +0, +wid);
   glTexCoord2f(0,0.987); glVertex3d(cone, +0, -wid);
   glEnd();

   //bottom
   glBegin(GL_QUADS);
   glNormal3f(0,+1,0);
   glTexCoord2f(0,0); glVertex3d(tail, -wid, +wid);
   glTexCoord2f(1,0); glVertex3d(cone, -wid, +wid);
   glTexCoord2f(1,0.987); glVertex3d(cone, -wid, -wid);
   glTexCoord2f(0,0.987); glVertex3d(tail, -wid, -wid);
   glEnd();

   //  Undo transformations
   glPopMatrix();
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
}

static void bomb(double x,double y,double z,double r,double rotate)
{
   //  Save transformation
   glPushMatrix();
   sphere(x,y,z,r,rotate);
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //rotate the sphere around y axis
   glRotated(rotate,0,1,0);

   //  Spikes
   const double nose=2;
   const double cone=0.2;
   const double wid=0.5;
   
   //  Enable textures
   glEnable(GL_TEXTURE_2D);

   //assign all phase to texture[2]
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   SetColor(1,1,1);
   //right spike   
   glBegin(GL_TRIANGLE_STRIP);
   for (int th=0;th<=360;th+=5){
      glNormal3f(cone,Cos(th),Sin(th));
      glTexCoord2f(0.5,1.0); glVertex3d(nose, 0.0, 0.0);  //A
      glTexCoord2f(0.0,0.0); glVertex3d(cone,wid*Cos(th),wid*Sin(th));  //B
      glTexCoord2f(1.0,0.0); glVertex3d(cone,wid*Cos(th+5),wid*Sin(th+5));  //C
   }
   glEnd();

//we get vectors by A-B as x (BA->), C-B as y (BC->). Cross product on these two vector x X y, result should be normal vector for this phase 

   //left spike
   glBegin(GL_TRIANGLE_STRIP);
   for (int th=0;th<=360;th+=5){
      glNormal3f(-cone,Cos(th),Sin(th));
      glTexCoord2f(0.5,1.0); glVertex3d(-nose, 0.0, 0.0);
      glTexCoord2f(0.0,0.0); glVertex3d(-cone,wid*Cos(th),wid*Sin(th));
      glTexCoord2f(1.0,0.0); glVertex3d(-cone,wid*Cos(th+5),wid*Sin(th+5));
   }
   glEnd();

   //up spike
   glBegin(GL_TRIANGLE_STRIP);
   for (int th=0;th<=360;th+=5){
      glNormal3f(Cos(th),cone,Sin(th));
      glTexCoord2f(0.5,1.0); glVertex3d(0.0, nose, 0.0);
      glTexCoord2f(0.0,0.0); glVertex3d(wid*Cos(th),cone,wid*Sin(th));
      glTexCoord2f(1.0,0.0); glVertex3d(wid*Cos(th+5),cone,wid*Sin(th+5));
   }
   glEnd();

   //down spike
   glBegin(GL_TRIANGLE_STRIP);
   for (int th=0;th<=360;th+=5){
      glNormal3f(Cos(th),-cone,Sin(th));
      glTexCoord2f(0.5,1.0); glVertex3d(0.0, -nose, 0.0);
      glTexCoord2f(0.0,0.0); glVertex3d(wid*Cos(th),-cone,wid*Sin(th));
      glTexCoord2f(1.0,0.0); glVertex3d(wid*Cos(th+5),-cone,wid*Sin(th+5));
   }
   glEnd();

   //front spike
   glBegin(GL_TRIANGLE_STRIP);
   for (int th=0;th<=360;th+=5){
      glNormal3f(Cos(th),Sin(th),cone);
      glTexCoord2f(0.5,1.0); glVertex3d(0.0, 0.0, nose);
      glTexCoord2f(0.0,0.0); glVertex3d(wid*Cos(th),wid*Sin(th),cone);
      glTexCoord2f(1.0,0.0); glVertex3d(wid*Cos(th+5),wid*Sin(th+5),cone);
   }
   glEnd();

   //back spike
   glBegin(GL_TRIANGLE_STRIP);
   for (int th=0;th<=360;th+=5){
      glNormal3f(Cos(th),Sin(th),cone);
      glTexCoord2f(0.5,1.0); glVertex3d(0.0, 0.0, -nose);
      glTexCoord2f(0.0,0.0); glVertex3d(wid*Cos(th),wid*Sin(th),-cone);
      glTexCoord2f(1.0,0.0); glVertex3d(wid*Cos(th+5),wid*Sin(th+5),-cone);
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
}

static void table(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   //translate y axis by dy to set all building starting from the ground
   glTranslated(x,y+dy,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Enable polygon offset
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   //assign all phase to texture[2]
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   //  Draw Table
   glBegin(GL_QUADS);
   glColor3f(1,1,1);
   //  Front
   glNormal3f( 0, 0, 1);
   //assigning unit texture together with shaping the phase
   glTexCoord2f(0.0,0.0); glVertex3f(-1,0.5, 1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,0.5, 1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1, 1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1, 1);
   //  Back
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,0.5,-1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,0.5,-1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1,-1);
   //  Right
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,0.5, 1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,0.5,-1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(+1,+1, 1);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,0.5, 1);
   glTexCoord2f(rep,0.0); glVertex3f(-1,0.5,-1);
   glTexCoord2f(rep,rep); glVertex3f(-1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1, 1);
   //  Top
   glNormal3f(0, +1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,+1, 1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,+1, 1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1,-1);
   //  Bottom
   glNormal3f(0, -1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,+0.5, 1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,+0.5, 1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+0.5,-1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+0.5,-1);
   

   //  Table legs

   //  Front left leg

   //  Front
   glNormal3f( 0, 0, 1);
   //assigning unit texture together with shaping the phase
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, 1);
   glTexCoord2f(rep,0.0); glVertex3f(-0.875,-1, 1);
   glTexCoord2f(rep,rep); glVertex3f(-0.875,0.5, 1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,0.5, 1);
   //  Back
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, 0.75);
   glTexCoord2f(rep,0.0); glVertex3f(-0.875,-1, 0.75);
   glTexCoord2f(rep,rep); glVertex3f(-0.875,0.5, 0.75);
   glTexCoord2f(0.0,rep); glVertex3f(-1,0.5, 0.75);
   //  Right
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.875,-1, 1);
   glTexCoord2f(rep,0.0); glVertex3f(-0.875,-1, 0.75);
   glTexCoord2f(rep,rep); glVertex3f(-0.875,0.5, 0.75);
   glTexCoord2f(0.0,rep); glVertex3f(-0.875,0.5, 1);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, 1);
   glTexCoord2f(rep,0.0); glVertex3f(-1,-1, 0.75);
   glTexCoord2f(rep,rep); glVertex3f(-1,0.5, 0.75);
   glTexCoord2f(0.0,rep); glVertex3f(-1,0.5, 1);

   //  Front right leg

   //  Front
   glNormal3f( 0, 0, 1);
   //assigning unit texture together with shaping the phase
   glTexCoord2f(0.0,0.0); glVertex3f(0.875,-1, 1);
   glTexCoord2f(rep,0.0); glVertex3f(1,-1, 1);
   glTexCoord2f(rep,rep); glVertex3f(1,0.5, 1);
   glTexCoord2f(0.0,rep); glVertex3f(0.875,0.5, 1);
   //  Back
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0.0,0.0); glVertex3f(0.875,-1, 0.75);
   glTexCoord2f(rep,0.0); glVertex3f(1,-1, 0.75);
   glTexCoord2f(rep,rep); glVertex3f(1,0.5, 0.75);
   glTexCoord2f(0.0,rep); glVertex3f(0.875,0.5, 0.75);
   //  Right
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(1,-1, 1);
   glTexCoord2f(rep,0.0); glVertex3f(1,-1, 0.75);
   glTexCoord2f(rep,rep); glVertex3f(1,0.5, 0.75);
   glTexCoord2f(0.0,rep); glVertex3f(1,0.5, 1);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0.875,-1, 1);
   glTexCoord2f(rep,0.0); glVertex3f(0.875,-1, 0.75);
   glTexCoord2f(rep,rep); glVertex3f(0.875,0.5, 0.75);
   glTexCoord2f(0.0,rep); glVertex3f(0.875,0.5, 1);

   //  Back left leg

   //  Front
   glNormal3f( 0, 0, 1);
   //assigning unit texture together with shaping the phase
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, -0.75);
   glTexCoord2f(rep,0.0); glVertex3f(-0.875,-1, -0.75);
   glTexCoord2f(rep,rep); glVertex3f(-0.875,0.5, -0.75);
   glTexCoord2f(0.0,rep); glVertex3f(-1,0.5, -0.75);
   //  Back
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, -1);
   glTexCoord2f(rep,0.0); glVertex3f(-0.875,-1, -1);
   glTexCoord2f(rep,rep); glVertex3f(-0.875,0.5, -1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,0.5, -1);
   //  Right
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.875,-1, -0.75);
   glTexCoord2f(rep,0.0); glVertex3f(-0.875,-1, -1);
   glTexCoord2f(rep,rep); glVertex3f(-0.875,0.5, -1);
   glTexCoord2f(0.0,rep); glVertex3f(-0.875,0.5, -0.75);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, -0.75);
   glTexCoord2f(rep,0.0); glVertex3f(-1,-1, -1);
   glTexCoord2f(rep,rep); glVertex3f(-1,0.5, -1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,0.5, -0.75);

   //  Back right leg

   //  Front
   glNormal3f( 0, 0, 1);
   //assigning unit texture together with shaping the phase
   glTexCoord2f(0.0,0.0); glVertex3f(0.875,-1, -0.75);
   glTexCoord2f(rep,0.0); glVertex3f(1,-1, -0.75);
   glTexCoord2f(rep,rep); glVertex3f(1,0.5, -0.75);
   glTexCoord2f(0.0,rep); glVertex3f(0.875,0.5, -0.75);
   //  Back
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0.0,0.0); glVertex3f(0.875,-1, -1);
   glTexCoord2f(rep,0.0); glVertex3f(1,-1, -1);
   glTexCoord2f(rep,rep); glVertex3f(1,0.5, -1);
   glTexCoord2f(0.0,rep); glVertex3f(0.875,0.5, -1);
   //  Right
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(1,-1, -0.75);
   glTexCoord2f(rep,0.0); glVertex3f(1,-1, -1);
   glTexCoord2f(rep,rep); glVertex3f(1,0.5, -1);
   glTexCoord2f(0.0,rep); glVertex3f(1,0.5, -0.75);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0.875,-1, -0.75);
   glTexCoord2f(rep,0.0); glVertex3f(0.875,-1, -1);
   glTexCoord2f(rep,rep); glVertex3f(0.875,0.5, -1);
   glTexCoord2f(0.0,rep); glVertex3f(0.875,0.5, -0.75);

   //  End
   glEnd();

   //  Disable polygon offset
   glDisable(GL_POLYGON_OFFSET_FILL);
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
   //  Undo transformations
   glPopMatrix();
}

static void hourglass(double x,double y,double z,double r,double rotate)
{
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y+r,z);
   glScaled(r,r,r);
   //rotate the sphere around y axis
   glRotated(rotate,0,1,0);

   //  Spikes
   //const double nose=2;
   const double cone=0.2;
   const double wid=0.1;
   
   //  Set transparency
   glEnable(GL_BLEND);
   glColor4f(1,1,1,alpha);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   glDepthMask(0);
   glEnable(GL_TEXTURE_2D);

   glBindTexture(GL_TEXTURE_2D,texture[28]);
   SetColor(1,1,1);

   //up spike - spike point downwards
   glBegin(GL_TRIANGLE_STRIP);
   for (int th=0;th<=360;th+=5){
      glNormal3f(Cos(th),-cone,Sin(th));
      glTexCoord2f(0.5,0.5); glVertex3d(0.0, 0, 0.0);
      glTexCoord2f((Cos(th) + 1.0)*0.5,(Sin(th) + 1.0)*0.5); glVertex3d(wid*Cos(th),cone,wid*Sin(th));
      glTexCoord2f((Cos(th+5) + 1.0)*0.5,(Sin(th+5) + 1.0)*0.5); glVertex3d(wid*Cos(th+5),cone,wid*Sin(th+5));
      
   }
   glEnd();

   //down spike - spike point upwards
   glBegin(GL_TRIANGLE_STRIP);
   for (int th=0;th<=360;th+=5){
      glNormal3f(Cos(th),+cone,Sin(th));
      glTexCoord2f(0.5,0.5); glVertex3d(0.0, 0, 0.0);
      glTexCoord2f((Cos(th) + 1.0)*0.5,(Sin(th) + 1.0)*0.5); glVertex3d(wid*Cos(th),-cone,wid*Sin(th));
      glTexCoord2f((Cos(th+5) + 1.0)*0.5,(Sin(th+5) + 1.0)*0.5); glVertex3d(wid*Cos(th+5),-cone,wid*Sin(th+5));
   }
   glEnd();

   // down circle
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,+1,0);
   glTexCoord2f(0.5,0.5); glVertex3d(0.0, cone, 0.0);
   for (int th=0;th<=360;th+=10){
      //rearrange the cos and sin value fron (-1,1) to (0,1)
      glTexCoord2f((Cos(th+90) + 1.0)*0.5,(Sin(th+90) + 1.0)*0.5); glVertex3d(wid*Cos(th),cone,wid*Sin(th));
   }
   glEnd();

   // up circle
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,-1,0);
   glTexCoord2f(0.5,0.5); glVertex3d(0.0, -cone, 0.0);
   for (int th=0;th<=360;th+=10){
      //rearrange the cos and sin value fron (-1,1) to (0,1)
      glTexCoord2f((Cos(th+90) + 1.0)*0.5,(Sin(th+90) + 1.0)*0.5); glVertex3d(wid*Cos(th),-cone,wid*Sin(th));
   }
   glEnd();

   glDisable(GL_BLEND);
   glDepthMask(1);
   //  Undo transformations
   glPopMatrix();
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
}

static void powder(double x,double y,double z,double r,double rotate)
{
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y+r,z);
   glScaled(r,r,r);
   //rotate the sphere around y axis
   glRotated(rotate,0,1,0);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);

   //  Spikes
   const double cone=0.2;
   const double wid=0.1;
   
   //up powder
   glBindTexture(GL_TEXTURE_2D,texture[29]);
   glBegin(GL_TRIANGLE_STRIP);
   for (int th=0;th<=360;th+=5){
      glNormal3f(Cos(th),+cone,Sin(th));
      //TOP of powder = -cone*0.5 so it is not full
      glTexCoord2f(0.5,0.5); glVertex3d(0.0, -cone*0.5, 0.0);
      glTexCoord2f((Cos(th) + 1.0)*0.5,(Sin(th) + 1.0)*0.5); glVertex3d(wid*Cos(th),-cone,wid*Sin(th));
      glTexCoord2f((Cos(th+5) + 1.0)*0.5,(Sin(th+5) + 1.0)*0.5); glVertex3d(wid*Cos(th+5),-cone,wid*Sin(th+5));
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
}

//general room object, can be modifed by not initializing some phases, which is controled by parameter mode
//mode=1, general room
//mode=2, hallway, so no left and right phases
//mode=3, corner, so no left and back phases
//mode=4, t-cross
//mode=5, x-cross, only top and bottom
//mode=6, main room, no right phase
static void room(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int mode)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   //translate y axis by dy to set all building starting from the ground
   glTranslated(x,y+dy,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Enable polygon offset
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   //assign all phase to texture[30]
   glBindTexture(GL_TEXTURE_2D,texture[30]);
   //  Cube
   //the drawing of the cube should have texture applied on it with rep*dx/dy/dz so that each phase's texture
   //will not be stretched by different dx, dy, dz values
   glBegin(GL_QUADS);
   SetColor(1,1,1);
   //if not x-crossing, then create front phase
   if(mode!=5){
      //  Front
      glNormal3f( 0, 0, -1);
      //assigning unit texture together with shaping the phase
      
      glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, 1);
      glTexCoord2f((rep+5)*dx,0.0); glVertex3f(+1,-1, 1);
      glTexCoord2f((rep+5)*dx,(rep+5)*dy); glVertex3f(+1,+1, 1);
      glTexCoord2f(0.0,(rep+5)*dy); glVertex3f(-1,+1, 1);

      //The 4 lines above were the easiest way for drawing that phase, but I have learned from HW5 that
      //for each phase, I can seperate it into multiple small squares
      //so that the lighting can apply on them more precisely when the light source and the surface is close enough

      //later we figured out that this can be done with shader but not with drawing multiple small squares in the same area
      //hence leaving it commented but as an evidence showing that I have done research in this direction

      // for (double i=-1;i<=0.9;i+=0.1){
      //    for (double j=-1;j<=0.9;j+=0.1){
      //       //Originally we were doing rep+5, rep=1, so rep+5=6
      //       //so we need to map (-1,1) to (0,6) for applying the same amount of textures on this surface,
      //       //therefore the formula for the mapping is (i+1)*3, (j+1)*3, for i,j in range [-1,1].
      //       //As usual, we multiply them by dx,dy to make sure that texture blocks appied to the surface
      //       //will not be stretched into weird shapes (longer, smaller, etc.).
      //       float s=(i+1)*3*dx;
      //       float t=(j+1)*3*dy;
      //       glTexCoord2f(s,t); glVertex3f(i, j, 1);
      //       s=(i+0.1+1)*3*dx;
      //       glTexCoord2f(s,t); glVertex3f(i+0.1, j, 1);
      //       t=(j+0.1+1)*3*dy;
      //       glTexCoord2f(s,t); glVertex3f(i+0.1, j+0.1, 1);
      //       s=(i+1)*3*dx;
      //       glTexCoord2f(s,t); glVertex3f(i,j+0.1, 1);
      //    }
      // }
   }
   //if not corner or t-crossing, then create back phase
   if(mode!=3 && mode!=4 && mode!=5){
      //  Back
      glNormal3f( 0, 0, +1);
      glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
      glTexCoord2f((rep+5)*dx,0.0); glVertex3f(+1,-1,-1);
      glTexCoord2f((rep+5)*dx,(rep+5)*dy); glVertex3f(+1,+1,-1);
      glTexCoord2f(0.0,(rep+5)*dy); glVertex3f(-1,+1,-1);
   }
   
   //if not hallway or t-crossing or main room, then create right phase
   if(mode!=2 && mode!=4 && mode!=5 && mode!=6){
      //  Right
      glNormal3f(-1, 0, 0);
      glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,-1);
      glTexCoord2f((rep+5)*dx,0.0); glVertex3f(+1,-1,+1);
      glTexCoord2f((rep+5)*dx,(rep+5)*dy); glVertex3f(+1,+1,+1);
      glTexCoord2f(0.0,(rep+5)*dy); glVertex3f(+1,+1,-1);
   }
   //if not hallway or corner or t-crossing, then create left phase
   if(mode!=2 && mode!=3 && mode!=4 && mode!=5){
      //  Left
      glNormal3f(+1, 0, 0);
      glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
      glTexCoord2f((rep+5)*dx,0.0); glVertex3f(-1,-1,+1);
      glTexCoord2f((rep+5)*dx,(rep+5)*dy); glVertex3f(-1,+1,+1);
      glTexCoord2f(0.0,(rep+5)*dy); glVertex3f(-1,+1,-1);
   }
   glEnd();
   
   //  Bottom
   //change texture to floor brick
   glBindTexture(GL_TEXTURE_2D,texture[31]);
   glBegin(GL_QUADS);
   glNormal3f(0, +1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f((rep+5)*dx,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f((rep+5)*dx,(rep+5)*dy); glVertex3f(+1,-1,+1);
   glTexCoord2f(0.0,(rep+5)*dy); glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Disable polygon offset
   glDisable(GL_POLYGON_OFFSET_FILL);
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
   //  Undo transformations
   glPopMatrix();
}

static void ceiling(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int left_on, int right_on)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y+dy,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Set transparency
   glEnable(GL_BLEND);
   glColor4f(1,1,1,alpha);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   glDepthMask(0);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,texture[27]);

   glBegin(GL_QUAD_STRIP);
   //half cylinder OR semicylinder facing up but with opposite normal vector so we can see lighting from inside of the building
   for (int th=-90;th<=90;th+=10)
   {
      //TEXTCOORD with th+72.5 for mapping the specific texture more precisely
      glNormal3f(0,-(2)*(Sin(th+10)-Sin(th)),(2)*(Cos(th+10)-Cos(th)));
      glTexCoord2f(0,(float)(th+72.5)/360*dy); glVertex3d(-1,1+Cos(th),Sin(th));
      glTexCoord2f(1*dx,(float)(th+72.5)/360*dy); glVertex3d(1,1+Cos(th),Sin(th));
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[21]);
   if(left_on){
      // left semicircle
      glBegin(GL_TRIANGLE_FAN);
      glNormal3f(+1,0,0);
      glTexCoord2f(0.5,0.5); glVertex3d(-1, 1.0, 0.0);
      for (int th=-90;th<=90;th+=10){
         //rearrange the cos and sin value fron (-1,1) to (0,1)
         glTexCoord2f((Cos(th+90) + 1.0)*0.5,(Sin(th+90) + 1.0)*0.5); glVertex3d(-1,1+Cos(th),Sin(th));
      }
      glEnd();
   }
   
   if(right_on){
      // right semicircle
      glBegin(GL_TRIANGLE_FAN);
      glNormal3f(-1,0,0);
      glTexCoord2f(0.5,0.5); glVertex3d(+1, 1.0, 0.0);
      for (int th=-90;th<=90;th+=10){
         //rearrange the cos and sin value fron (-1,1) to (0,1)
         glTexCoord2f((Cos(th+90) + 1.0)*0.5,(Sin(th+90) + 1.0)*0.5); glVertex3d(+1,1+Cos(th),Sin(th));
      }
      glEnd();
   }
   
   glDisable(GL_BLEND);
   glDepthMask(1);
   glDisable(GL_TEXTURE_2D);
   //  Undo transofrmations
   glPopMatrix();
}

static void secret_room(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //set a different specular on this object and set it back to the original data later
   //therefore we are tracking the lighting type value defined in .frag file
   int id=glGetUniformLocation(shader,"lighting_type");
   //pre-assign it to 0 for those phases that requires normal lighting
   glUniform1i(id,0);
   //  Save transformation
   glPushMatrix();
   //  Offset
   //translate y axis by dy to set all building starting from the ground
   glTranslated(x,y+dy,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Enable polygon offset
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   //assign all phase to texture[30]
   glBindTexture(GL_TEXTURE_2D,texture[30]);
   //  Cube
   //the drawing of the cube should have texture applied on it with rep*dx/dy/dz so that each phase's texture
   //will not be stretched by different dx, dy, dz values
   SetColor(1.0,1.0,1.05);
   //set lighting value to 1 since we are applying special lighting effect on this phase
   glUniform1i(id,1);

   //semi cylinders as pillars
   //front pillar
   glBegin(GL_QUAD_STRIP);
   for (int th=0;th<=180;th+=10)
   {
      glNormal3f(-(2)*(Sin(th+10)-Sin(th)),0,(2)*(Cos(th+10)-Cos(th)));
      //move z value to the front by 1 so that the pillar is at the front of the room
      glTexCoord2f((float)(th+90)/360*10*dx,0); glVertex3d(0.5*(Cos(th)),-1,0.5*(Sin(th))+1.0);
      glTexCoord2f((float)(th+90)/360*10*dx,1*5*dz); glVertex3d(0.5*(Cos(th)),+1,0.5*(Sin(th))+1.0);
   }
   glEnd();

   //  Front left
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, -1);
   //assigning unit texture together with shaping the phase
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, 1);
   glTexCoord2f((rep+1.25)*dx,0.0); glVertex3f(-0.5,-1, 1);
   glTexCoord2f((rep+1.25)*dx,(rep+5)*dy); glVertex3f(-0.5,+1, 1);
   glTexCoord2f(0.0,(rep+5)*dy); glVertex3f(-1,+1, 1);
   glEnd();
   //  Front right
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, -1);
   //assigning unit texture together with shaping the phase
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1, 1);
   glTexCoord2f((rep+1.25)*dx,0.0); glVertex3f(+0.5,-1, 1);
   glTexCoord2f((rep+1.25)*dx,(rep+5)*dy); glVertex3f(+0.5,+1, 1);
   glTexCoord2f(0.0,(rep+5)*dy); glVertex3f(+1,+1, 1);
   glEnd();

   SetColor(1,1,1);
   //assign the lighting value back to 0 for normal lighting on the rest of the phases
   glUniform1i(id,0);
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   // front bottom semicircle
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,-1,0);
   glTexCoord2f(0.5*3,0.5*3); glVertex3d(0.0,-1.0, 1.0);
   for (int th=-90;th<=90;th+=10){
      //rearrange the cos and sin value fron (-1,1) to (0,1)
      glTexCoord2f((Cos(th+90) + 1.0)*0.5*3,(Sin(th+90) + 1.0)*0.5*3); glVertex3d(0.5*Sin(th),-1,1.0+0.5*Cos(th));
   }
   glEnd();

   // front top semicircle
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,+1,0);
   glTexCoord2f(0.5*3,0.5*3); glVertex3d(0.0,+1.0, 1.0);
   for (int th=-90;th<=90;th+=10){
      //rearrange the cos and sin value fron (-1,1) to (0,1)
      glTexCoord2f((Cos(th+90) + 1.0)*0.5*3,(Sin(th+90) + 1.0)*0.5*3); glVertex3d(0.5*Sin(th),+1,1.0+0.5*Cos(th));
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[30]);
   //if the boss room has not been entered, the illusory wall will be shown
   if(wall_break==0){
      //  Back
      glBegin(GL_QUADS);
      glNormal3f( 0, 0, +1);
      glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-0.999);
      glTexCoord2f((rep+5)*dx,0.0); glVertex3f(+1,-1,-0.999);
      glTexCoord2f((rep+5)*dx,(rep+5)*dy); glVertex3f(+1,+1,-0.999);
      glTexCoord2f(0.0,(rep+5)*dy); glVertex3f(-1,+1,-0.999);
      glEnd();

      // semi circle
      glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(0.5,0.5); glVertex3d(0.0, 1.0, -0.99);
      for (int th=-90;th<=90;th+=10){
         //rearrange the cos and sin value fron (-1,1) to (0,1)
         glTexCoord2f((Cos(th+90) + 1.0)*3,(Sin(th+90) + 1.0)*3); glVertex3d(Sin(th),1+Cos(th),-0.99);
      }
      glEnd();

      //Back again but with a different normal vector direction and a little bit different color
      //this is the illusory wall, and the squares for building this wall is less than usual
      SetColor(1,1,1);
      glBegin(GL_QUADS);
      glNormal3f( 0, 0, -1);
      glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
      glTexCoord2f((rep+5)*dx,0.0); glVertex3f(+1,-1,-1);
      glTexCoord2f((rep+5)*dx,(rep+5)*dy); glVertex3f(+1,+1,-1);
      glTexCoord2f(0.0,(rep+5)*dy); glVertex3f(-1,+1,-1);
      glEnd();

      // semi circle
      glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(0.5,0.5); glVertex3d(0.0, 1.0, -1);
      for (int th=-90;th<=90;th+=10){
         //rearrange the cos and sin value fron (-1,1) to (0,1)
         glTexCoord2f((Cos(th+90) + 1.0)*0.5,(Sin(th+90) + 1.0)*0.5); glVertex3d(Sin(th),1+Cos(th),-1);
      }
      glEnd();
   }
   
   //  Right
   //change color back to white
   SetColor(1,1,1);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f((rep+5)*dx,0.0); glVertex3f(+1,-1,+1);
   glTexCoord2f((rep+5)*dx,(rep+5)*dy); glVertex3f(+1,+1,+1);
   glTexCoord2f(0.0,(rep+5)*dy); glVertex3f(+1,+1,-1);
   //  Left
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f((rep+5)*dx,0.0); glVertex3f(-1,-1,+1);
   glTexCoord2f((rep+5)*dx,(rep+5)*dy); glVertex3f(-1,+1,+1);
   glTexCoord2f(0.0,(rep+5)*dy); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   //set the lighting value to 1 for applying special lighting setting on this phase
   glUniform1i(id,1);
   //change texture to floor brick
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_QUADS);
   glNormal3f(0, +1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f((rep+5)*dx,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f((rep+5)*dx,(rep+5)*dy); glVertex3f(+1,-1,+1);
   glTexCoord2f(0.0,(rep+5)*dy); glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //set lighting value back to 0 for normal lighting on the rest of the objects
   glUniform1i(id,0);

   //bomb run in half circle
   float Position[]  = {0,0.8*(Cos(zh)<=0?Cos(zh):-Cos(zh)),0.8*Sin(zh),1.0};
   //up and down movement
   bomb(Position[0],Position[1],Position[2] , 0.05, zh);

   bomb(Position[2],Position[1],Position[0] , 0.05, zh);

   //  Disable polygon offset
   glDisable(GL_POLYGON_OFFSET_FILL);
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
   //  Undo transformations
   glPopMatrix();
}

static void boss_room(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   //translate y axis by dy to set all building starting from the ground
   glTranslated(x,y+dy,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Enable polygon offset
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   //assign front wall to texture[9] for fog effect
   glBindTexture(GL_TEXTURE_2D,texture[9]);
   //  Front with fog wall
   glBegin(GL_QUADS);
   glColor3f(1,1,1);
   glNormal3f( 0, 0, -1);
   //assigning unit texture together with shaping the phase
   glTexCoord2f(0.0,timer/30); glVertex3f(-1,-1, 0.999);
   glTexCoord2f((rep/2)*dx,timer/30); glVertex3f(+1,-1, 0.999);
   glTexCoord2f((rep/2)*dx,(rep/2)*dy+timer/30); glVertex3f(+1,+1, 0.999);
   glTexCoord2f(0.0,(rep/2)*dy+timer/30); glVertex3f(-1,+1, 0.999);

   //another front wall with fog but have opposite normal vector
   glNormal3f( 0, 0, +1);
   //assigning unit texture together with shaping the phase
   glTexCoord2f(0.0,timer/30); glVertex3f(-1,-1, 1);
   glTexCoord2f((rep/2)*dx,timer/30); glVertex3f(+1,-1, 1);
   glTexCoord2f((rep/2)*dx,(rep/2)*dy+timer/30); glVertex3f(+1,+1, 1);
   glTexCoord2f(0.0,(rep/2)*dy+timer/30); glVertex3f(-1,+1, 1);

   glEnd();

   //change texture back
   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUADS);

   //  Back
   glNormal3f( 0, 0, +1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f((rep+1)*dx,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f((rep+1)*dx,(rep+1)*dy); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,(rep+1)*dy); glVertex3f(-1,+1,-1);
   //  Right
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f((rep+1)*dx,0.0); glVertex3f(+1,-1,+1);
   glTexCoord2f((rep+1)*dx,(rep+1)*dy); glVertex3f(+1,+1,+1);
   glTexCoord2f(0.0,(rep+1)*dy); glVertex3f(+1,+1,-1);
   //  Left
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f((rep+1)*dx,0.0); glVertex3f(-1,-1,+1);
   glTexCoord2f((rep+1)*dx,(rep+1)*dy); glVertex3f(-1,+1,+1);
   glTexCoord2f(0.0,(rep+1)*dy); glVertex3f(-1,+1,-1);
   //  Top
   glNormal3f(0, -1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,+1,-1);
   glTexCoord2f((rep+1)*dx,0.0); glVertex3f(+1,+1,-1);
   glTexCoord2f((rep+1)*dx,(rep+1)*dy); glVertex3f(+1,+1,+1);
   glTexCoord2f(0.0,(rep+1)*dy); glVertex3f(-1,+1,+1);
   glEnd();
   //  Bottom
   //change texture to floor brick
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_QUADS);
   glNormal3f(0, +1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f((rep+1)*dx,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f((rep+1)*dx,(rep+1)*dy); glVertex3f(+1,-1,+1);
   glTexCoord2f(0.0,(rep+1)*dy); glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //three eyes on the wall
   sphere( 0,0.5,-1,0.1,0);
   sphere(-1,0.5, 0,0.1,90);
   sphere(+1,0.5, 0,0.1,270);

   //  Disable polygon offset
   glDisable(GL_POLYGON_OFFSET_FILL);
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
   //  Undo transformations
   glPopMatrix();
}

static void pillar(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   //translate y axis by dy to set all building starting from the ground
   glTranslated(x,y+dy,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Enable polygon offset
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,texture[8]);

   //semi cylinders as pillars
   //left front pillar
   glBegin(GL_QUAD_STRIP);
   for (int th=-90;th<=90;th+=10)
   {
      glNormal3f((2)*(Sin(th+10)-Sin(th)),0,-(2)*(Cos(th+10)-Cos(th)));
      //WID=0.2, move x value to the left by 1 so the pillar is at the left side of the room
      //move z value to the front by 0.5 so that the pillar is at the front of the room
      glTexCoord2f((float)(th+90)/360*2*dx,0); glVertex3d(0.2*(Cos(th))-1,-1,0.2*(Sin(th))+0.5);
      glTexCoord2f((float)(th+90)/360*2*dx,1*2*dz); glVertex3d(0.2*(Cos(th))-1,+1,0.2*(Sin(th))+0.5);
   }
   glEnd();

   //left back pillar
   glBegin(GL_QUAD_STRIP);
   for (int th=-90;th<=90;th+=10)
   {
      glNormal3f((2)*(Sin(th+10)-Sin(th)),0,-(2)*(Cos(th+10)-Cos(th)));
      //WID=0.2, move x value to the left by 1 so the pillar is at the left side of the room
      //move z value to the back by 0.5 so that the pillar is at the front of the room
      glTexCoord2f((float)(th+90)/360*2*dx,0); glVertex3d(0.2*(Cos(th))-1,-1,0.2*(Sin(th))-0.5);
      glTexCoord2f((float)(th+90)/360*2*dx,1*2*dz); glVertex3d(0.2*(Cos(th))-1,+1,0.2*(Sin(th))-0.5);
   }
   glEnd();

   //right front pillar
   glBegin(GL_QUAD_STRIP);
   for (int th=90;th<=270;th+=10)
   {
      glNormal3f((2)*(Sin(th+10)-Sin(th)),0,-(2)*(Cos(th+10)-Cos(th)));
      //WID=0.2, move x value to the right by 1 so the pillar is at the left side of the room
      //move z value to the front by 0.5 so that the pillar is at the front of the room
      glTexCoord2f((float)(th+90)/360*2*dx,0); glVertex3d(0.2*(Cos(th))+1,-1,0.2*(Sin(th))+0.5);
      glTexCoord2f((float)(th+90)/360*2*dx,1*2*dz); glVertex3d(0.2*(Cos(th))+1,+1,0.2*(Sin(th))+0.5);
   }
   glEnd();

   //right back pillar
   glBegin(GL_QUAD_STRIP);
   for (int th=90;th<=270;th+=10)
   {
      glNormal3f((2)*(Sin(th+10)-Sin(th)),0,-(2)*(Cos(th+10)-Cos(th)));
      //WID=0.2, move x value to the right by 1 so the pillar is at the left side of the room
      //move z value to the back by 0.5 so that the pillar is at the front of the room
      glTexCoord2f((float)(th+90)/360*2*dx,0); glVertex3d(0.2*(Cos(th))+1,-1,0.2*(Sin(th))-0.5);
      glTexCoord2f((float)(th+90)/360*2*dx,1*2*dz); glVertex3d(0.2*(Cos(th))+1,+1,0.2*(Sin(th))-0.5);
   }
   glEnd();

   //  Disable polygon offset
   glDisable(GL_POLYGON_OFFSET_FILL);
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
   //  Undo transformations
   glPopMatrix();
}

static void painting(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int painting_id)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   //translate y axis by dy to set all building starting from the ground
   glTranslated(x,y+dy,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Enable polygon offset
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,texture[painting_id]);
   //  Front
   glBegin(GL_QUADS);
   glColor3f(1,1,1);
   glNormal3f( 0, 0, +1);
   glTexCoord2f(0,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,+1);
   glEnd();
   //  Frames
   //change texture to crate
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_QUADS);
   //  Right
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,+0.01);
   glTexCoord2f(1,1); glVertex3f(+1,+1,+0.01);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+0.01);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+0.01);
   glTexCoord2f(0,1); glVertex3f(-1,+1,+1);
   //  Top
   glNormal3f(0, +1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,+0.01);
   glTexCoord2f(0,1); glVertex3f(-1,+1,+0.01);
   //  Bottom
   glNormal3f(0, -1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+0.01);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+0.01);
   glEnd();
   //  Disable polygon offset
   glDisable(GL_POLYGON_OFFSET_FILL);
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
   //  Undo transformations
   glPopMatrix();
}

static void clock(double x,double y,double z,
                 double r, double d,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   //translate y axis by dy to set all building starting from the ground
   glTranslated(x,y+r,z);
   glRotated(th,0,1,0);
   glScaled(r,r,d);
   //  Enable polygon offset
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,texture[6]);

   //side ring
   glBegin(GL_QUAD_STRIP);
   for (int th=0;th<=360;th+=10)
   {
      glNormal3f(Cos(th),Sin(th),0);
      glTexCoord2f(0,0.5*th); glVertex3f(Cos(th),Sin(th),+1);
      glTexCoord2f(1,0.5*th); glVertex3f(Cos(th),Sin(th),-1);
   }
   glEnd();

   //  clock face
   glBindTexture(GL_TEXTURE_2D,texture[32]);
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,0,+1);
   glTexCoord2f(0.5,0.5); glVertex3d(0.0, 0.0, 1.0);
   for (int th=0;th<=360;th+=10){
      //rearrange the cos and sin value fron (-1,1) to (0,1)
      glTexCoord2f((Cos(th) + 1.0)*0.5,(Sin(th) + 1.0)*0.5); glVertex3d(Cos(th),Sin(th),1);
   }
   glEnd();
   
   //  Disable polygon offset
   glDisable(GL_POLYGON_OFFSET_FILL);
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
   //  Undo transformations
   glPopMatrix();
}

// A brown ground surface
static void ground(){
   //set offset for the ground so that it is "away" from other items on the same y axis.
   //so that it will not overlap with other things on the same layer in the view.
   //cannot set the y axis to negative value, otherwise it is "comming towards" the eye/camera
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(2,2);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   //assign all phase to texture[3]
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   //draw the ground surface
   glBegin(GL_QUADS);
   glColor3f(1,1,1);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-15,0.0,-15);
   glTexCoord2f(rep+15,0.0); glVertex3f(+15,0.0,-15);
   glTexCoord2f(rep+15,rep+15); glVertex3f(+15,0.0,+15);
   glTexCoord2f(0.0,rep+15); glVertex3f(-15,0.0,+15);
   glEnd();
   glDisable(GL_POLYGON_OFFSET_FILL);
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
}

// A skybox
static void sky(double x,double y,double z,double r,double rotate){
   const int d=15;
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //rotate the sphere around y axis
   glRotated(rotate,0,1,0);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);

   glBindTexture(GL_TEXTURE_2D,texture[20]);

   //  Latitude bands
   for (int ph=-90;ph<90;ph+=d)
   {
      //semi sphere for the covering the whole scene under the sky
      glBegin(GL_QUAD_STRIP);
      for (int th=-90;th<=90;th+=d)
      {
         //assign texcoord to vertex using floating values, otherwise weird things would happen
         glTexCoord2f((float)th/360*1,((float)ph+180)/180*1); Vertex(th,ph);
         glTexCoord2f((float)th/360*1,((float)ph+180+d)/180*1); Vertex(th,ph+d);
      }
      glEnd();
   }
   //  Undo transformations
   glPopMatrix();
   //  Disable texture
   glDisable(GL_TEXTURE_2D);
}

/*
 *  Draw the UI as an overlay
 *  Must be called last
 */
void UI()
{
   //  Screen edge
   float w = asp>2 ? asp : 2;
   //  Save transform attributes (Matrix Mode and Enabled Modes)
   glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
   //  Save projection matrix and set unit transform
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(-asp,+asp,-1,1,-1,1);
   //  Save model view matrix and set to indentity
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   //  Draw UI with texture
   glColor3f(1,1,1);

   //hp bar
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0);glVertex2f(-w,0.9);
   glTexCoord2d(1,0);glVertex2f(0,0.9);
   glTexCoord2d(1,1);glVertex2f(0,0.95);
   glTexCoord2d(0,1);glVertex2f(-w,0.95);
   glEnd();
   
   //mp bar
   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0.45);glVertex2f(-w,0.85);
   glTexCoord2d(1,0.45);glVertex2f(-0.5,0.85);
   glTexCoord2d(1,0.55);glVertex2f(-0.5,0.9);
   glTexCoord2d(0,0.55);glVertex2f(-w,0.9);
   glEnd();
   //stamina bar
   glBindTexture(GL_TEXTURE_2D,texture[13]);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0.45);glVertex2f(-w,0.8);
   glTexCoord2d(1,0.45);glVertex2f(0,0.8);
   glTexCoord2d(1,0.55);glVertex2f(0,0.85);
   glTexCoord2d(0,0.55);glVertex2f(-w,0.85);
   glEnd();

   //down object - estus flask
   glBindTexture(GL_TEXTURE_2D,texture[14]);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0);glVertex2f(-w*0.8,-0.8);
   glTexCoord2d(1,0);glVertex2f(-w*0.7,-0.8);
   glTexCoord2d(1,1);glVertex2f(-w*0.7,-0.5);
   glTexCoord2d(0,1);glVertex2f(-w*0.8,-0.5);
   glEnd();

   //up object - magic
   glBindTexture(GL_TEXTURE_2D,texture[15]);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0);glVertex2f(-w*0.8,-0.45);
   glTexCoord2d(1,0);glVertex2f(-w*0.7,-0.45);
   glTexCoord2d(1,1);glVertex2f(-w*0.7,-0.15);
   glTexCoord2d(0,1);glVertex2f(-w*0.8,-0.15);
   glEnd();

   //right object - right weapon 
   glBindTexture(GL_TEXTURE_2D,texture[16]);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0);glVertex2f(-w*0.8+0.225,-0.65);
   glTexCoord2d(1,0);glVertex2f(-w*0.7+0.225,-0.65);
   glTexCoord2d(1,1);glVertex2f(-w*0.7+0.225,-0.35);
   glTexCoord2d(0,1);glVertex2f(-w*0.8+0.225,-0.35);
   glEnd();

   //left object - left weapon
   glBindTexture(GL_TEXTURE_2D,texture[17]);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0);glVertex2f(-w*0.8-0.225,-0.65);
   glTexCoord2d(1,0);glVertex2f(-w*0.7-0.225,-0.65);
   glTexCoord2d(1,1);glVertex2f(-w*0.7-0.225,-0.35);
   glTexCoord2d(0,1);glVertex2f(-w*0.8-0.225,-0.35);
   glEnd();

   //soul counter
   glBindTexture(GL_TEXTURE_2D,texture[18]);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0);glVertex2f(w*0.7,-0.8);
   glTexCoord2d(1,0);glVertex2f(w*0.9,-0.8);
   glTexCoord2d(1,1);glVertex2f(w*0.9,-0.7);
   glTexCoord2d(0,1);glVertex2f(w*0.7,-0.7);
   glEnd();

   //light source
   //lantern
   glBindTexture(GL_TEXTURE_2D,texture[34]);
   glBegin(GL_QUADS);
   glTexCoord2d(0.35,0.03);glVertex2f(w*0.2,-1.9);
   glTexCoord2d(0.6,0.03);glVertex2f(w*0.4,-1.9);
   glTexCoord2d(0.6,0.483);glVertex2f(w*0.4,-0.5);
   glTexCoord2d(0.35,0.483);glVertex2f(w*0.2,-0.5);
   glEnd();
   //fire
   glBindTexture(GL_TEXTURE_2D,texture[34]);
   glBegin(GL_QUADS);
   glTexCoord2d(0.45,0.5);glVertex2f(w*0.28,-0.5);
   glTexCoord2d(0.48,0.5);glVertex2f(w*0.3,-0.5);
   glTexCoord2d(0.48,0.6);glVertex2f(w*0.29,-0.32);
   glTexCoord2d(0.45,0.6);glVertex2f(w*0.281,-0.3);
   glEnd();

   glDisable(GL_TEXTURE_2D);
   //  Reset model view matrix
   glPopMatrix();
   //  Reset projection matrix
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   //  Pop transform attributes (Matrix Mode and Enabled Modes)
   glPopAttrib();
}

void boss_UI()
{
   //  Save transform attributes (Matrix Mode and Enabled Modes)
   glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
   //  Save projection matrix and set unit transform
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(-asp,+asp,-1,1,-1,1);
   //  Save model view matrix and set to indentity
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   //  Draw UI with texture
   glColor3f(1,1,1);
   //boss_hp bar
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0);glVertex2f(-asp*0.6,-0.7);
   glTexCoord2d(1,0);glVertex2f(asp*0.6,-0.7);
   glTexCoord2d(1,1);glVertex2f(asp*0.6,-0.65);
   glTexCoord2d(0,1);glVertex2f(-asp*0.6,-0.65);
   glEnd();

   glDisable(GL_TEXTURE_2D);

   //boss name
   //a textbox that changes the position based on the current screen size
   glWindowPos2i(screen_width*0.2,screen_height*0.18);
   Print("Oceiros, the Cursed Phantom Golem");
   //  Reset model view matrix
   glPopMatrix();
   //  Reset projection matrix
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   //  Pop transform attributes (Matrix Mode and Enabled Modes)
   glPopAttrib();
}

//low-level collision detection that uses bunches of if else statements for checking the player's current position
//if the position is in those banned zones, then stop the movement
void position_detection(double old_eye_x, double old_eye_z, double new_eye_x, double new_eye_z) {
   //boundary detection
   if(new_eye_z<=-8.8||new_eye_z>=10){
      //if out of boundary on z axis then cancel the move on z axis but still update x axis so the player is sliding on the boundary
      Eye_z=old_eye_z;
   }
   if(new_eye_x<=-10||new_eye_x>=10.15){
      //if out of boundary on x axis then cancel the move on x axis but still update z axis so the player is sliding on the boundary
      Eye_x=old_eye_x;
   } 
   
   //left and right wall detected by z value
   if(new_eye_z>-9 && new_eye_z<3){
      //left wall of the main room
      if(new_eye_x>-1.2 && new_eye_x<-0.8){
         if(new_eye_z>-1 && new_eye_z<1){
            Eye_x=old_eye_x;
         }
      }
      //right wall before boss room
      else if(new_eye_x>8.8 && new_eye_x<9.2){
         if(new_eye_z>=-3.1){
            Eye_x=old_eye_x;
         }
      }
      //left wall before the boss room
      else if(new_eye_x>6.8 && new_eye_x<7.2){
         //left wall before the boss room or left room in secret room
         if((new_eye_z<-0.92&&new_eye_z>=-3.1) || new_eye_z>0.92){
            Eye_x=old_eye_x;
         }
      }
      //for table
      else if(new_eye_x>4.45 && new_eye_x<5.55){
         //right and left side of table
         if(new_eye_z>-1.0 && new_eye_z<-0.39){
            Eye_x=old_eye_x;
         }
      }
      else if(new_eye_x>=4.8 && new_eye_x<=6){
         if(new_eye_z>-9.0 && new_eye_z<-3.1){
            Eye_x=old_eye_x;
         }
      }
   }

   //back and front wall detected by x value
   if(new_eye_x>-1 && new_eye_x<11){
      //back wall
      if(new_eye_z>0.8 && new_eye_z<1.2){
         //back wall before t-crossing
         if(new_eye_x<7.1){
            Eye_z=old_eye_z;
         }
         else if(!wall_break){
            Eye_z=old_eye_z;
         }
      }
      //back wall after t-crossing - the one in secret room
      else if(new_eye_z>2.77 && new_eye_z<3.2){
         if(new_eye_x>7.1){
            Eye_z=old_eye_z;
         }
      }
      //front wall
      else if(new_eye_z<-0.8 && new_eye_z>-1.2){
         //front wall before t-crossing
         if(new_eye_x<7.1){
            Eye_z=old_eye_z;
         }
      }
      //for table
      else if(new_eye_z<-0.39 && new_eye_z>-1.2){
         //front and back side
         if(new_eye_x>4.5 && new_eye_x<5.5){
            Eye_z=old_eye_z;
         }
      }
      //boss room's back wall
      else if(new_eye_z>-3.2&&new_eye_z<-2.8){
         //left AND right back wall
         if((new_eye_x<=7&&new_eye_x>=4.8) || new_eye_x>=9){
            Eye_z=old_eye_z;
         }
      }
   }
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   // timer set for relocating the texture coordinate of the fog gate to make it "moving"
   timer = fmod(t,30);

   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode==1)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else if (mode==0)
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
   //  First person - set eye orientation
   else{
        //change direction position in  a sphere space
        //sign will all be opposite to the one in perspective mode so that the camera will move in the same direction as direction key
        
        //change x value of camera
        direction_x = +1*Sin(th)*Cos(ph);
        //change y value of camera
        direction_y = +1        *Sin(ph);
        //change z value of camera
        direction_z = -1*Cos(th)*Cos(ph);
         
        //unit moving value in 2D (x, y values) based on the current view angle
        //will not be used until WASD been pressed

        //update x value for camera movement in 2D at the current view angle, 
        //not affected by ph, otherwise moving speed will also depends on viewing angle on y axis
        move_x = +1*Sin(th);

        //update z value for camera movement in 2D at the current view angle
        //not affected by ph, otherwise moving speed will also depends on viewing angle on y axis
        move_z = -1*Cos(th);
         
        //set camera location on eye, target location on eye+direction unit so that we will always focus on the direction
        //that have been set by pressing direction keys
        //still set the upward direction to Cos(ph) instead of 1
        //otherwise when turning over 360, the WASD function will operate reversively
        //since technically the camera angle is reversed but the scene I am seeing is still in the original form
        gluLookAt(Eye_x,Eye_y,Eye_z, Eye_x+direction_x,Eye_y+direction_y,Eye_z+direction_z , 0,Cos(ph),0);
   }
   //  Lightning, copied directly from ex13.c
   //  if we put this whole if statement before gluLookAt from above
   //  then we can have a light ball that is always runnning around the camera
   //  can be modified to a torch type object
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float lightPos[]  = {Eye_x+direction_x/10,Eye_y+direction_y/10,Eye_z+direction_z/10, 1};
      
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,lightPos);

      //  Draw light position as sphere (still no lighting here)

      //for shader:
      //  Set materials 
      glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,Shinyness);
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Specular);
      glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   }
   else
      glDisable(GL_LIGHTING);

   //  Select shader
   glUseProgram(shader);
   float time = roll ? 0.001*glutGet(GLUT_ELAPSED_TIME) : 0;
   int id = glGetUniformLocation(shader,"Xcenter");
   glUniform1f(id,X);
   id = glGetUniformLocation(shader,"Ycenter");
   glUniform1f(id,Y);
   id = glGetUniformLocation(shader,"Zoom");
   glUniform1f(id,Z);
   id = glGetUniformLocation(shader,"time");
   glUniform1f(id,time);

   id = glGetUniformLocation(shader,"lighting_type");
   glUniform1i(id,0);

   //set up objects in the scene
   //  Scene at the center
   //main room
   room(0,0,0 , 1,1,1 , 0, 6);
   //painting in the room
   painting(0,1,1, 0.3,0.4,0.03, 180, 1);
   painting(2.5,1,1, 0.3,0.4,0.03, 180, 4);
   painting(5.0,1,1, 0.3,0.4,0.03, 180, 35);
   painting(2.5,1,-1, 0.3,0.4,0.03, 0, 10);
   painting(9,1,0, 0.3,0.4,0.03, 270, 5);
   
   //floating box like a pickable item
   float Position_box[]  = {4.75,0.05*Cos(zh)+0.32,-0.7,1.0};
   treasure_box_open(Position_box[0],Position_box[1],Position_box[2], 0.2, zh); //DEMO box on the table

   spear(0,1.3,-0.8, 0.3, 70);
   spear(5,1.3,-0.8, 0.3, 290);
   treasure_box(8.0,0.15,3.25, 0.5, 180);
   
   //table
   table(5,0,-0.75 , 0.4,0.1,0.2 , 0);

   //clock
   clock(-0.9,1.2,0, 0.4,0.01, 90);
   

   //  Scene to the right
   //hallway to the right
   room(3,0,0,  2,1,1, 0, 2);
   //hallway to the right
   room(6,0,0,  1,1,1, 0, 2);
   //t-crossing
   room(8,0,0, 1,1,1, 90, 4);
   //hidden room
   secret_room(8,0,2 , 1,1,1 , 0);
   //hallway
   room(8,0,-2,  1,1,1, 90, 2);
   //boss room
   boss_room(8,0,-6, 3,3,3, 0);
   //pillars in the boss room
   pillar(8,0,-6, 3,3,3, 0);

   ground();
   sky(5,0,0, 13, 0);

   //hourglass together with powder in it. Powder been generated first since it is not transparent
   powder(5,0,-0.6 , 0.25, 0);
   //generate transparent ones at the end of other objects
   hourglass(5,0,-0.6 , 0.25, 0);
   
   //floating golem
   //move up and down
   float Position[]  = {8,0.8*Cos(zh)+0.8,-7,1.0};
   golem(Position[0],Position[1],Position[2], 0.3,0.5,0.5, 0);

   //generate see-through ceillings for each room except boss room
   //main room
   ceiling(0,0,0 , 1,1,1 , 0, 1, 0);
   //hallway to the right
   ceiling(3,0,0,  2,1,1, 0 ,0, 0);
   //hallway to the right
   ceiling(6,0,0,  1,1,1, 0, 0, 1);
   //t-crossing
   ceiling(8,0,0, 1,1,1, 90, 0, 0);
   // //hidden room
   ceiling(8,0,2, 1,1,1, 90, 1, 0);
   //hallway
   ceiling(8,0,-2,  1,1,1, 90, 0, 0);

   //  Revert to fixed pipeline
   glUseProgram(0);

   //  White
   glColor3f(1,1,1);
   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   //  Draw axes
   if (axes)
   {
      const double len=1.5;  //  Length of axes
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Display parameters in developer mode only
   if(developer_mode==1){
      glWindowPos2i(5,5);
      Print("Angle=%d,%d  Mode=%s Dim=%.1f FOV=%d Light=%s",
        th,ph,text[mode],dim,fov,light?"On":"Off");
      if (light)
      {
         glWindowPos2i(5,45);
         Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
         glWindowPos2i(5,25);
         Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
         glWindowPos2i(5,65);
         Print("Eye_x=%.2f  Eye_z=%.2f Collision Detection=%s",Eye_x,Eye_z,collision_detection?"On":"Off");
         glWindowPos2i(5,85);
         Print("You Are In Developer Mode!");
      }
   }
   //if enter boss room then generate boss health bar, and it is over the general UI layer
   if(Eye_x>=4.9&&Eye_x<=11.1&&Eye_z<=-2.9&&Eye_z>=-9.1){
      boss_UI();
      //also set boss_entered to 1, hence illusory wall at main room disappear
      wall_break=1;
   }
   //call the UI function to generate the UI layer at the front of the screen
   //only show the UI in first person view
   if(mode==2){
      UI();
   }
   
   //  Render the scene
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  Set projection, different from the default Project function defined in the library
 */
static void Project_modified()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode==1)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else if(mode==0)
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  First person view
   else{
   //  Set zNear value to be smaller than the pre-set Eye_y value so that you can see the ground
      gluPerspective(fov,asp,Eye_y/4,4*dim);
   }
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //adjustable parameters in developer mode
   if(developer_mode==1){
      //  Smooth color model
      if (key == GLUT_KEY_F1)
         smooth = 1-smooth;
      //  Local Viewer
      else if (key == GLUT_KEY_F2)
         local = 1-local;
      else if (key == GLUT_KEY_F3)
         distance = (distance==1) ? 5 : 1;
   }
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project_modified();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */

void key(unsigned char ch,int x,int y)
{
   //store the current player's position
   double old_eye_x=Eye_x;
   double old_eye_z=Eye_z;
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Switch to developer mode
   else if (ch == 'o' || ch == 'O'){
        developer_mode = 1 - developer_mode;
   }

   //  Move in 2D (on the ground) based on movement units 
   
   //  but not direction units (that's in 3D, hence y value / ph value affects the moving speed)
   //  Move forward 
   else if (ch == 'w' || ch == 'W'){
        Eye_z+=move_z*0.1;
        Eye_x+=move_x*0.1;
   }
   //  Move backward
   else if (ch == 's' || ch == 'S'){
        Eye_z-=move_z*0.1;
        Eye_x-=move_x*0.1;
   }
   //  Move left
   else if (ch == 'a' || ch == 'A'){
        Eye_x+=move_z*0.1;
        Eye_z-=move_x*0.1;
   }
   //  Move right
   else if (ch == 'd' || ch == 'D'){
        Eye_x-=move_z*0.1;
        Eye_z+=move_x*0.1;
   }
   //do position detection to check if the move was valid
   if(collision_detection==1){
      position_detection(old_eye_x,old_eye_z,Eye_x,Eye_z);
   }

   //adjustable parameters in developer mode
   if(developer_mode==1){
      //  Reset view angle
      if (ch == '0'){
         th = ph = 0;
         direction_x=0;
         direction_y=0;
         direction_z=-1;
         Eye_x=0;
         Eye_y=0.4;
         Eye_z=0;
         wall_break=0;
         th=90;
      }
      //  Toggle axes
      else if (ch == '1' || ch == '1')
         axes = 1-axes;
      //  Switch display mode
      else if (ch == 'm')
         mode = (mode+1)%3;
      else if (ch == 'M')
         mode = (mode+2)%3;
      //  Toggle movement of light
      else if (ch == 'l')
         move = 1-move;
      //  Toggle lighting
      else if (ch == 'L')
         light = 1-light;
      //  Change field of view angle
      else if (ch == '-' && ch>1)
         fov--;
      else if (ch == '+' && ch<179)
         fov++;
      //  Ambient level
      else if (ch=='q' && ambient>0)
         ambient -= 5;
      else if (ch=='Q' && ambient<100)
         ambient += 5;
      //  Diffuse level
      else if (ch=='f' && diffuse>0)
         diffuse -= 5;
      else if (ch=='F' && diffuse<100)
         diffuse += 5;
      //  Specular level
      else if (ch=='x' && specular>0)
         specular -= 5;
      else if (ch=='X' && specular<100)
         specular += 5;
      //  Emission level
      else if (ch=='e' && emission>0)
         emission -= 5;
      else if (ch=='E' && emission<100)
         emission += 5;
      //  Shininess level
      else if (ch=='n' && shininess>-1)
         shininess -= 1;
      else if (ch=='N' && shininess<7)
         shininess += 1;
      //  Collision detection
      else if (ch == 'c' || ch == 'C')
         collision_detection = 1-collision_detection;
   }

   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   Project_modified();
   //  Animate if requested
   glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Track the current screen width and height
   screen_width=width;
   screen_height=height;
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Set projection
   Project_modified();
}

/*
 *  Read text file
 */
char* ReadText(char *file)
{
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) Fatal("Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   int n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

/*
 *  Print Shader Log
 */
void PrintShaderLog(int obj,char* file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file,buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal("Error compiling %s\n",file);
}

/*
 *  Print Program Log
 */
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal("Error linking program\n");
}

/*
 *  Create Shader
 */
int CreateShader(GLenum type,char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   fprintf(stderr,"Compile %s\n",file);
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Return name
   return shader;
}

/*
 *  Create Shader Program
 */
int CreateShaderProg(char* VertFile,char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   int vert = CreateShader(GL_VERTEX_SHADER,VertFile);
   //  Create and compile fragment shader
   int frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
   //  Attach vertex shader
   glAttachShader(prog,vert);
   //  Attach fragment shader
   glAttachShader(prog,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Project Sitong Lu - Dark Souls 3.5");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);

   //  Load textures
   //wall textures
   texture[0] = LoadTexBMP("floor_brick.bmp");
   texture[1] = LoadTexBMP("scene_painting2.bmp");
   texture[2] = LoadTexBMP("crate.bmp");
   //ground texture
   texture[3] = LoadTexBMP("ground.bmp");
   //painting texture
   texture[4] = LoadTexBMP("castle_painting_2.bmp");
   //painting texture
   texture[5] = LoadTexBMP("castle_painting.bmp");
   //spike texture
   texture[6] = LoadTexBMP("metal.bmp");
   //ball texture
   texture[7] = LoadTexBMP("eye.bmp");
   //boss room wall texture
   texture[8] = LoadTexBMP("boss_wall_2.bmp");
   //fog wall texture
   texture[9] = LoadTexBMP("fog.bmp");
   //painting
   texture[10] = LoadTexBMP("cat_painting.bmp");
   //health_bar
   texture[11] = LoadTexBMP("red.bmp");
   //magic_bar
   texture[12] = LoadTexBMP("blue.bmp");
   //stamina bar
   texture[13] = LoadTexBMP("green.bmp");
   //estus flask
   texture[14] = LoadTexBMP("estus_flask.bmp");
   //magic
   texture[15] = LoadTexBMP("afinidad.bmp");
   //dark sword
   texture[16] = LoadTexBMP("dark_sword.bmp");
   //spider shield
   texture[17] = LoadTexBMP("spider_shield.bmp");
   //soul counter
   texture[18] = LoadTexBMP("soul.bmp");
   //crystal
   texture[19] = LoadTexBMP("crystal.bmp");
   //sky
   texture[20] = LoadTexBMP("sky.bmp");
   //ceiling
   texture[21] = LoadTexBMP("ceiling.bmp");
   //treasure box top
   texture[22] = LoadTexBMP("box_top.bmp");
   //treasure box front
   texture[23] = LoadTexBMP("box_front.bmp");
   //treasure box back
   texture[24] = LoadTexBMP("box_back.bmp");
   //treasure box sides
   texture[25] = LoadTexBMP("box_side.bmp");
   //treasure box sides with semicircle
   texture[26] = LoadTexBMP("box_side_semi.bmp");
   //ceiling top
   texture[27] = LoadTexBMP("ceiling_top.bmp");
   //glass for hourglass texture
   texture[28] = LoadTexBMP("glass.bmp");
   //powder
   texture[29] = LoadTexBMP("powder.bmp");
   //palace wall
   texture[30] = LoadTexBMP("palace_wall.bmp");
   //palace wall
   texture[31] = LoadTexBMP("palace_floor.bmp");
   //clock face
   texture[32] = LoadTexBMP("clock_face.bmp");
   //wood wand
   texture[33] = LoadTexBMP("wood.bmp");
   //lantern
   texture[34] = LoadTexBMP("lantern.bmp");
   //wolf painting
   texture[35] = LoadTexBMP("wolf_painting.bmp");

   //  Switch font to nearest
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   //  Create Shader Programs
   //sitong.frag is the modifed version of pixtex.frag from ex27
   shader = CreateShaderProg("pixtex.vert","sitong.frag");

   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}