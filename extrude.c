/* Corey Finley                               */
/* 31 October 2013                             */
/* Interactive viewing of a user drawn extruded polygon     */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h> 

// Mac Header Files
#include <GLUT/glut.h>
#include <OpenGL/gl.h>

// Linux Header Files
// #include <GL/glut.h> 
// #include <GL/gl.h>

int  l_click = GLUT_UP; // initial value for left click
int  r_click = GLUT_UP; // initial value for right click

typedef struct {
  GLfloat x;
  GLfloat y;
  GLfloat z;
}vertex;

vertex front_face[20];
int num_verticies = 0;
bool extruded = false;

float extrusion_offset = 2;

// int start_x1, start_y1, start_x2, start_y2; // variables for start x and y for teapots 1 and 2 respectivly
// double theta_x, theta_y, mouse_delta_x, mouse_delta_y; // rotation variables for teapot 1
double HEIGHT = 500; // window size
double WIDTH = 500; // window size

// GLfloat matrix[16]; // matrix for remembering transformations

// void teapot1(){
//   /* Attributes for Teapot 1 
//    * Size=1.0, Location: (-2.0, 0.0, -2.5)
//    * Material 
//    *  ambience (0.0215, 0.1745, 0.0215, 1.0), 
//    *  diffusive (0.07568, 0.61424, 0.07568, 1.0), 
//    *  specular (0.633, 0.727811, 0.633, 1.0), 
//    *  shininess=0.6
//    */
//   GLfloat amb[]={0.0215, 0.1745, 0.0215, 1.0};
//   GLfloat diff[]={0.07568, 0.61424, 0.07568, 1.0};
//   GLfloat spec[]={0.633, 0.727811, 0.633, 1.0};
//   GLfloat shine[]={0.6};

//   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   amb);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   diff);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  spec);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);

//   glLoadMatrixf(matrix1); // load matrix
//   glRotatef(theta_x1, 0, 1, 0); // rotate vertically
//   glRotatef(theta_y1, 1, 0, 0); // rotate horizontally
//   glGetFloatv(GL_MODELVIEW_MATRIX, matrix1); // save matrix

//   // to prevent further rotation on redisplay
//   theta_x1 = 0;
//   theta_y1 = 0;

//   glutSolidTeapot(1.0); // print teapot
// }

void setLighting(){
  glLoadIdentity();
  // lighting from assignment
  GLfloat position[]={.0, 3.0, -3.0, 1.0};
  GLfloat amb[]={1.0, 1.0, 1.0, 1.0};
  GLfloat dif[]={1.0, 1.0, 1.0, 1.0};
  GLfloat spc[]={1.0, 1.0, 1.0, 1.0};
  GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};
  GLfloat local_view[] = {0.0};

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
  glLightfv(GL_LIGHT0, GL_SPECULAR, spc);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

  gluLookAt(         // Eye
    0.0, 0.0, 0.0,   // Location
    0.0, 0.0, -1.0,  // Direction
    0.0, 1.0, 0.0);  // Up Direction
}

void init()
{
  glClearColor (0.0,0.0,0.0,1.0); // set background to black

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-5.0, 5.0, -5.0, 5.0, 0.0, 5.0); // bounding volume for the projection
  
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_DEPTH_TEST); // enable hidden surface removal

  // glLoadMatrixf(matrix);
  // glLoadIdentity();
  // glTranslatef(0.0,0.0,-2.0);
  // glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  setLighting(); // call method to set the lighting effects
  num_verticies = 0;
}

vertex createPoint(int x, int y){
  GLint viewport[4];
  GLdouble model_view[16];
  GLdouble projection[16];
  GLfloat window_x, window_y, window_z;
  GLdouble position_x, position_y, position_z;
  vertex point;

  glGetDoublev(GL_MODELVIEW_MATRIX, model_view); // get the model view matrix and put it into the local var
  glGetDoublev(GL_PROJECTION_MATRIX, projection); // get the projection matrix and put it into the local var
  glGetIntegerv(GL_VIEWPORT, viewport); // get viewport and put it into the local var

  window_x = (float)x;
  window_y = (float)viewport[3] - (float)y;
  glReadPixels( x, (int)window_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &window_z );
  gluUnProject( window_x, window_y, window_z, model_view, projection, viewport, &position_x, &position_y, &position_z);
  point.x = position_x;
  point.y = position_y;
  point.z = -4.0;
  return point;
}

void createPolygon(vertex* input_shape){
  glColor3f(1.0f, 0.0f, 0.0f);
  glBegin(GL_POLYGON);
  int i;
  vertex temp;
    for(i=0; i< num_verticies; i++){
      temp = input_shape[i];
      printf("CT: input_shape[%i] => [%f, %f, %f]\n", i, (float)temp.x, (float)temp.y, (float)temp.z);
      glVertex3f((float)temp.x, (float)temp.y, (float)temp.z);
    }
  glEnd();
}

void createObject(vertex* input_shape){
  printf("CT Enter\n");

  glColor3f(1.0f, 0.0f, 0.0f);

  glBegin(GL_POLYGON);
  int i;
  vertex temp;
    for(i=0; i<num_verticies; i++){
      temp = input_shape[i];
      glVertex3f((float)temp.x, (float)temp.y, (float)temp.z);
    }
  glEnd();

  for(i=0; i<num_verticies; i++){
    if(i == num_verticies-1){
      glBegin(GL_POLYGON);
        glVertex3f((float)input_shape[i].x, (float)input_shape[i].y, (float)input_shape[i].z);
        glVertex3f((float)input_shape[i].x, (float)input_shape[i].y, (float)input_shape[i].z+extrusion_offset);
        glVertex3f((float)input_shape[0].x, (float)input_shape[0].y, (float)input_shape[0].z);
        glVertex3f((float)input_shape[0].x, (float)input_shape[0].y, (float)input_shape[0].z+extrusion_offset);
      glEnd();
    } else{
      glBegin(GL_POLYGON);
        glVertex3f((float)input_shape[i].x, (float)input_shape[i].y, (float)input_shape[i].z);
        glVertex3f((float)input_shape[i].x, (float)input_shape[i].y, (float)input_shape[i].z+extrusion_offset);
        glVertex3f((float)input_shape[i+1].x, (float)input_shape[i+1].y, (float)input_shape[i+1].z);
        glVertex3f((float)input_shape[i+1].x, (float)input_shape[i+1].y, (float)input_shape[i+1].z+extrusion_offset);
      glEnd();
    }
  }

  glBegin(GL_POLYGON);
    for(i=0; i< num_verticies; i++){
      temp = input_shape[i];
      glVertex3f((float)temp.x, (float)temp.y, (float)temp.z+extrusion_offset);
    }
  glEnd();

  printf("CT Exit\n");
}

/* Callbacks */

/* display frame */
void display(){
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // clear the color buffer and the depth buffer
  if(num_verticies >= 3){
    if(extruded == false){
      createPolygon(front_face);
    } else {
      createObject(front_face);
    }
  }
  glutSwapBuffers(); // swap buffers in the double buffer
}

/* interprate keyboard functions */
void keyboard(unsigned char key, int x, int y){
  /* Load the matrix for the teapot1 and teapot2 for l key 
   * press and r key press respectivly. Load the identity
   * matrix to clear it out, then reiterate the translate.
   * This resets the teapot, without any rotation.
   */
  if('l' == key || 'L' == key){

  }
  if('r' == key || 'R' == key){

  }
}

/* interprate mouse clicks */
void mouseClick(int button, int state, int x, int y){
  /* Get starting x and starting y for distance change calculation
   * Set the click state for mouseMotion
   */
  if(GLUT_LEFT_BUTTON == button){
    if(extruded == false){
      if(num_verticies < 20 && state == GLUT_DOWN){
        vertex point;
        point = createPoint(x, y);
        printf("front_face[%i] => x %f, y %f, z %f\n", num_verticies, point.x, point.y, point.z);

        front_face[num_verticies] = point;
        num_verticies++;
        display();

        state = GLUT_UP;
      }
    }
  }
  if(GLUT_RIGHT_BUTTON == button){
    printf("Right Click\n");
    if(extruded == false){
      printf("Extrude!\n");
      extruded = true;
      display();
    }
  }
}

/* interprate mouse motion */
void mouseMotion(int x, int y){
  /* For both right and left click, first measure change in x, and
   */
  if(l_click == GLUT_DOWN){

  }
  if(r_click == GLUT_DOWN){

  }
}



/* Main */
int main(int argc, char* argv[])
{
	  /* standard GLUT initialization */
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); // use double buffer to prevent flicker
    glutInitWindowSize(HEIGHT,WIDTH); // set window with globals
    glutInitWindowPosition(100,100);
    glutCreateWindow("Polygon Extrude"); // window title

    init(); // set attributes

    /* Callbacks */
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMotion);
    glutMainLoop(); // start event loop
}