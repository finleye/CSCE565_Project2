/* Corey Finley                               */
/* 31 October 2013                             */
/* Interactive viewing of a user drawn extruded polygon     */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h> 

// Mac Header Files
// #include <GLUT/glut.h>
// #include <OpenGL/gl.h>

// Linux Header Files
#include <GL/glut.h> 
#include <GL/gl.h>

int  l_click = GLUT_UP; // initial value for left click
int  r_click = GLUT_UP; // initial value for right click
int  draw_type = GL_POLYGON; // for toggling between solid object, and wireframe

/* vertex struct */
typedef struct {
  GLfloat x;
  GLfloat y;
  GLfloat z;
}vertex;

vertex front_face[20];
int num_verticies = 0;
bool extruded = false;

float front_face_z = -2.0; // default for front face drawing

float extrusion_depth = 0.5; // default for extrusion depth

int start_x, start_y; // variables for start x and y when clicking the mouse for rotation
double theta_x, theta_y, mouse_delta_x, mouse_delta_y; // rotation variables
GLfloat matrix[16]; // matrix state for rotation

double HEIGHT = 500; // window size
double WIDTH = 500; // window size

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

  /* set up matrix for rotation after extrusion */
  glLoadMatrixf(matrix);
  glLoadIdentity();
  glTranslatef(0.0,0.0,-3.0);
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  glDisable(GL_CULL_FACE);

  setLighting(); // call method to set the lighting effects
  num_verticies = 0;
  printf("Click to draw Polygon!\n");
}

/* create a point from mouse click */
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

  // calculate x and y position in the window
  glReadPixels( x, (int)window_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &window_z );
  gluUnProject( window_x, window_y, window_z, model_view, projection, viewport, &position_x, &position_y, &position_z);

  // set point values
  point.x = position_x;
  point.y = position_y;
  point.z = front_face_z;

  return point;
}

/* Material Attributes */
void setMaterial(){
  GLfloat amb[]={0.0215, 0.1745, 0.0215, 1.0};
  GLfloat diff[]={0.07568, 0.61424, 0.07568, 1.0};
  GLfloat spec[]={0.633, 0.727811, 0.633, 1.0};
  GLfloat shine[]={0.3};

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   amb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   diff);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  spec);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}

/* Draw poitns for creating the polygon */
void drawPoints(vertex* input_shape){
  int i;
  vertex temp;
  glBegin(GL_LINE_LOOP);
  for(i=0; i< num_verticies; i++){
    temp = input_shape[i];
    glVertex3f((float)temp.x, (float)temp.y, -2.0f);
  }
  glEnd();
}

/* Find the center of the object by taking the average distance
 * of the verticies from the origin
 */
vertex findCenter(vertex* input_shape){
  vertex point;

  int i;
  vertex temp;
  for(i=0; i< num_verticies; i++){
    temp = input_shape[i];
    point.x += temp.x;
    point.y += temp.y;
  }

  point.x = point.x/num_verticies;
  point.y = point.y/num_verticies;

  return point;
}

/* Move the object to the origin by finding the center of 
 * the object and translating each point that amount.
 */
void setCenter(vertex* input_shape){
  vertex center = findCenter(input_shape);

  int i;
  for(i=0; i< num_verticies; i++){
    input_shape[i].x = input_shape[i].x-center.x;
    input_shape[i].y = input_shape[i].y-center.y;
  }
}

/* Draw a front or back face, stored in the array. This face will
 * have a z offset value passed as a parameter. The front face while
 * drawing is set by deault
 */
void createFace(vertex* input_shape, float z_value){
  setMaterial();

  int i;
  vertex temp;
  vertex center = findCenter(input_shape);

  glBegin(draw_type);
  glVertex3f((float)center.x, (float)center.y, z_value); // use the found center as the center of the fan
  for(i=0; i< num_verticies; i++){
    temp = input_shape[i];
    glVertex3f((float)temp.x, (float)temp.y, z_value);
  }
  glVertex3f(input_shape[0].x, input_shape[0].y, z_value); // to complete the fan, use the starting click vertex again
  glEnd();
}

/* Draw the fron face, side wals, and back face from the passed in shape */
void createObject(vertex* input_shape){
  setMaterial();

  /* Rotation */
  glLoadMatrixf(matrix); // load matrix
  glRotatef(theta_x, 0, 1, 0); // rotate vertically
  glRotatef(theta_y, 1, 0, 0); // rotate horizontally
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix); // save matrix

  /* to prevent further rotation on redisplay */
  theta_x = 0;
  theta_y = 0;

  /* Draw Front Face*/
  createFace(front_face, extrusion_depth);
  /* draw back face */
  createFace(front_face, -extrusion_depth);

  /* Draw side walls */
  int i, j;
  for(i=0; i<num_verticies; i++){
    if(i == num_verticies-1){
      j = 0;
    } else {
      j = i+1;
    }
    glBegin(draw_type); 
    glVertex3f((float)input_shape[i].x, (float)input_shape[i].y, extrusion_depth);
    glVertex3f((float)input_shape[j].x, (float)input_shape[j].y, extrusion_depth);
    glVertex3f((float)input_shape[j].x, (float)input_shape[j].y, -extrusion_depth);
    glVertex3f((float)input_shape[i].x, (float)input_shape[i].y, -extrusion_depth);
    glEnd();
  }
}

/* display frame */
void display(){
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // clear the color buffer and the depth buffer
  if(num_verticies >= 3){
    if(extruded == true){
      createObject(front_face);
    } else {
      drawPoints(front_face);
    }
  }
  glutSwapBuffers(); // swap buffers in the double buffer
}

/* interprate keyboard functions */
void keyboard(unsigned char key, int x, int y){
  /* Press r to reset the object to starting rotation point
   * Press w to toggle between solid and wireframe view
   * Press s to start over from polygon draw
   */

  if('r' == key || 'R' == key){
    printf("Reset to starting rotation\n");
    glLoadMatrixf(matrix);
    glLoadIdentity();
    glTranslatef(0.0,0.0,-3.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);   

    glutPostRedisplay(); // redisplay 
  }
  if('w' == key || 'W' == key){
    if(draw_type == GL_POLYGON){
      printf("View Mode: Wireframe\n");
      draw_type = GL_LINE_LOOP;
    } else{
      printf("View Mode: Solid Object\n");
      draw_type = GL_POLYGON;
    }

    glutPostRedisplay(); // redisplay 
  }
  if('s' == key || 'S' == key){
    printf("Click to draw Polygon!\n");
    extruded = false;
    num_verticies = 0;

    /* reset rotation matrix */
    glLoadMatrixf(matrix);
    glLoadIdentity();
    glTranslatef(0.0,0.0,-3.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

    glutPostRedisplay(); // redisplay
  }
}

/* interprate mouse clicks */
void mouseClick(int button, int state, int x, int y){
  /* Get starting x and starting y for distance change calculation
   * Set the click state for mouseMotion
   */
  if(GLUT_LEFT_BUTTON == button && state == GLUT_DOWN){
    if(extruded == false){
      /* if the number of verticies has not been reached, add it. */
      if(num_verticies < 20){
        vertex point;
        point = createPoint(x, y);
        printf("front_face[%i]: %10f\t%10f\n", num_verticies, (float)point.x, (float)point.y);

        front_face[num_verticies] = point;
        num_verticies++;
        glutPostRedisplay(); // redisplay
      }else{
        printf("Vertext limit reached!\n");
        glutPostRedisplay();
      }
    } else {
      /* Get starting x and starting y for distance change calculation
       * Set the click state for mouseMotion
       */
      start_x = x;
      start_y = y;
      l_click = state;
    }
  }
  if(GLUT_RIGHT_BUTTON == button && state == GLUT_DOWN){
    if(extruded == false){
      printf("Extruded: Click and drag to rotate.\n");
      theta_x = 0.0;
      theta_y = 0.0;
      extruded = true;
      
      setCenter(front_face);
      glutPostRedisplay(); // redisplay
    }
  }
  state = GLUT_UP;
}


/* interprate mouse motion */
void mouseMotion(int x, int y){
  if(l_click == GLUT_DOWN){
    mouse_delta_x = (x-start_x)/2.f; // Change in x
    mouse_delta_y = (y-start_y)/2.f; // Change in y
    theta_x = (360*mouse_delta_x/sqrt(HEIGHT*HEIGHT+WIDTH*WIDTH)); // amount of rotation for x axis
    theta_y = (360*mouse_delta_y/sqrt(HEIGHT*HEIGHT+WIDTH*WIDTH)); // amount of rotation for y axis
    glutPostRedisplay(); // redisplay
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