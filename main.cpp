#include <windows.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include<math.h>
#include<direct.h>

#define PI 3.14
// ��ת���ں���ת����
static float moon_year = 0, year = 0, mercury_year = 0, venus_year = 0,
mars_year = 0, jupiter_year = 0, saturn_year = 0,
uranus_year = 0, neptune_year = 0,
moon_day = 0, day = 0, mercury_day = 0, venus_day = 0,
mars_day = 0, jupiter_day = 0, saturn_day = 0,
uranus_day = 0, neptune_day = 0, sun_day = 0;
static int longitude = 200, latitude=200;  //��ľ�ϸ��
// �ӽǱ任����
GLfloat dx=0.0, dy=1.0, sx=0.0, sy=0.0, sz=5, sx1=0.0, sy1=0.0, sz1=5;
GLfloat theta=0;
float star[1000][3]; // ����λ��
float center[] = { 0, 0, 0 };
int late = 30; // �����Զ�����ʱ�������ǵ��ٶ�
int mode = 1; //����̫��ϵ�Ƿ��Զ�����
int dim = 1;  //2D/3D�ӽ�
GLfloat height = 0;
int flag = 1;
// ��ͼ����
static GLuint texture[10]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static GLfloat xequalzero[] = {1.0, 0.0, 0.0, 0.0};
static GLfloat yequalzero[] = {0.0, 1.0, 0.0, 0.0};
static GLfloat slanted[] = {1.0, 1.0, 1.0, 0.0};
static GLfloat *currentCoeff;
static GLenum currentPlane;
static GLint currentGenMode;

// ��ȡ.bmp�ļ�����
// ���� filename���ļ���ַ��imagewidth����Ƭ��ȣ�imageheight����Ƭ�߶�
// ��� pixeldata���ļ�����
GLubyte*  readImg(char* filename, int * imagewidth, int * imageheight)
{
 //���ļ�
  int pixellength;
  GLubyte *  pixeldata ;
  FILE* pfile=fopen(filename,"rb");
  if(pfile == 0) exit(0);
//��ȡͼ���С
   fseek(pfile,0x0012,SEEK_SET);
   fread(imagewidth,sizeof(*imagewidth),1,pfile);
   fread(imageheight,sizeof(*imageheight),1,pfile);

//�����������ݳ���
   pixellength=(*imagewidth)*3;
   while(pixellength%4 != 0)pixellength++;
   pixellength *= (*imageheight);

 //��ȡ��������
   pixeldata = (GLubyte*)malloc(pixellength);
   if(pixeldata == 0) exit(0);

   fseek(pfile,54,SEEK_SET);
   fread(pixeldata,pixellength,1,pfile);

    //�ر��ļ�
   fclose(pfile);
   return pixeldata;
}

// ��������
// ���� filename���ļ���ַ��texture����ͼID
void LoadTexture(char * filename, GLuint &texture)
{
    GLubyte * data;
	GLint width, height;
	FILE * file;

	//���ļ�
    data=readImg(filename, &width, &height );
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);//������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//������ͼ
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //�Զ������������꣬ʹ������ӳ��
    currentCoeff = xequalzero;
    currentGenMode = GL_OBJECT_LINEAR;
    currentPlane = GL_OBJECT_PLANE;

	//������1:GL_UNPACK_ALIGNMENT ��ָ��OpenGL��δ����ݻ������н��ͼ������
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

	glEnable(GL_TEXTURE_2D);//������ӳ��

	free(data); //�ͷ�����ͼ�����ݣ���������������һ�����ɲ����浽�������У�ʹ����Ϻ�Ӧ��glDeleteTextures�ͷ�������
}

void init(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);    //�Զ�����ڲ����е����ɫͨ���Զ���ζ�����ɫ��ֵ�������൱��gouraud��������

   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };  //������ϵ������ӦRGBA�ĸ�������A�����ڻ�Ͽ�������Ч�����շ�����ɫ
   GLfloat mat_shininess[] = { 10.0 }; //��ʾ���ʵĸ߹�ϵ�����ϸߵ�ֵ�������С�͸������ĸ߹⣬���ϵ͵�ֵ����������͸���͵ĸ߹⡣


   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);//���û�����ϵ�����������ϵ��
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);//���ò��Ϸ���ָ��

   // ����6���ƹ⣬��x��y��z���������
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glEnable(GL_LIGHT2);
   glEnable(GL_LIGHT3);
   glEnable(GL_LIGHT4);
   glEnable(GL_LIGHT5);

   LoadTexture("D:\\c\\final\\bmp\\sun.bmp",texture[0]);//���������������
   LoadTexture("D:\\c\\final\\bmp\\mercury.bmp",texture[1]);//���������������
   LoadTexture("D:\\c\\final\\bmp\\venus.bmp",texture[2]);//���������������
   LoadTexture("D:\\c\\final\\bmp\\earth.bmp",texture[3]);//���������������
   LoadTexture("D:\\c\\final\\bmp\\moon.bmp",texture[4]);//���������������
   LoadTexture("D:\\c\\final\\bmp\\mars.bmp",texture[5]);//���������������
   LoadTexture("D:\\c\\final\\bmp\\jupiter.bmp",texture[6]);//���������������
   LoadTexture("D:\\c\\final\\bmp\\saturn.bmp",texture[7]);//���������������
   LoadTexture("D:\\c\\final\\bmp\\uranus.bmp",texture[8]);//���������������
   LoadTexture("D:\\c\\final\\bmp\\neptune.bmp",texture[9]);//���������������

}

// ���õƹ����
void set_light()
{
    // ��Դλ��
	GLfloat light0_position0[] = {1.5,0.0,0.0,1.0};
	GLfloat light0_position1[] = {-1.5,0.0,0.0,1.0};
	GLfloat light0_position2[] = {0,1.5,0.0,1.0};
	GLfloat light0_position3[] = {0,-1.5,0.0,1.0};
	GLfloat light0_position4[] = {0,0,1.5,1.0};
	GLfloat light0_position5[] = {0,0,-1.5,1.0};
	GLfloat light0_diffuse[] = {0.7f, 0.7f, 0.7f, 0.5f};
	GLfloat light0_specular[] = {0.5f, 0.5f, 0.5f, 0.2f};
	GLfloat light0_shininess[] = {100};

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light0_position1);
	glLightfv(GL_LIGHT2, GL_POSITION, light0_position2);
	glLightfv(GL_LIGHT3, GL_POSITION, light0_position3);
	glLightfv(GL_LIGHT4, GL_POSITION, light0_position4);
	glLightfv(GL_LIGHT5, GL_POSITION, light0_position5);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_SHININESS, light0_shininess);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT1, GL_SHININESS, light0_shininess);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT2, GL_SHININESS, light0_shininess);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT3, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT3, GL_SHININESS, light0_shininess);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT4, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT4, GL_SHININESS, light0_shininess);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT5, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT5, GL_SHININESS, light0_shininess);


}

// ���

void myCircle(GLfloat R)
{
    int n = 80;
	//��������
	GLfloat star_ambient[] = {1.0,1.0,1.0,0};
	GLfloat star_emission[] = {1.0,1.0,1.0}; //�޹���ʱ��������ʾ����ɫ
	//�����������
    glMaterialfv(GL_FRONT, GL_AMBIENT, star_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, star_emission);
	glBegin(GL_LINE_LOOP); //����n����
	for (int i = 0; i<n; ++i)
		glVertex3f(R*cos(2 * PI / n*i), 0, R*sin(2 * PI / n*i));
	glEnd();
}

// ����
// �����������λ��
void init_stars()
{
    for (int i = 0; i < 1000; i++)
        for (int j = 0; j < 3; j++)
            star[i][j] = rand() % 20 - 10;
}

// �õ��������
void stars()
{
    //��������
	GLfloat star_ambient[] = {1.0,1.0,1.0,1};
	GLfloat star_emission[] = {1.0,1.0,1.0}; //�޹���ʱ��������ʾ����ɫ
	//�����������
    glMaterialfv(GL_FRONT, GL_AMBIENT, star_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, star_emission);
    glBegin(GL_POINTS);
    for (int i = 0; i < 1000; ++i) {
        glVertex3f(star[i][0], star[i][1], star[i][2]);
    }
    glEnd();
}

// ̫��
void sun()
{
    //̫������
    GLfloat s_ambient[] = {1.0,0.0,0.0,1};
	GLfloat s_emission[] = {1.0,0,0};

	//���̫������
	glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, s_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    gluQuadricTexture(qobj, GL_TRUE);//������,ʹ�ø��������꺯��ʱ����Ҫͬʱʹ�����������Զ�����glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) sun_day, 0.0, 1.0, 0.0);    // ̫����ת
    gluSphere(qobj, 1.5, longitude, latitude);//��������qobj
    gluDeleteQuadric(qobj);

}

//ˮ��
void mercury()
{
    //ˮ�ǲ���
	GLfloat mercury_ambient[] = {0.8,0.8,0.8,1};
	GLfloat mercury_emission[] = {0.2,0.2,0.2}; //�޹���ʱ��������ʾ����ɫ

	//���ˮ�ǲ���
	glMaterialfv(GL_FRONT, GL_AMBIENT, mercury_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, mercury_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    gluQuadricTexture(qobj, GL_TRUE);//������,ʹ�ø��������꺯��ʱ����Ҫͬʱʹ�����������Զ�����glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) mercury_year, 0.0, 1.0, 0.0);  // ˮ����̫����ת
    glTranslatef (2.0, 0.0, 0.0);               //  ƽ�Ʊ任T2
    glRotatef ((GLfloat) mercury_day, 0.0, 1.0, 0.0);    // ˮ����ת
    gluSphere(qobj, 0.15, longitude, latitude);//��������qobj
    gluDeleteQuadric(qobj);

}

//����
void venus()
{
    //���ǲ���
    GLfloat s_ambient[] = {1.0,0.0,0.0,1};
    GLfloat s_emission[] = {0.2,0.2,0.2}; //�޹���ʱ��������ʾ����ɫ

	//��ӽ��ǲ���
	glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, s_emission);
    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    gluQuadricTexture(qobj, GL_TRUE);//������,ʹ�ø��������꺯��ʱ����Ҫͬʱʹ�����������Զ�����glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) venus_year, 0.0, 1.0, 0.0);  // ������̫����ת
    glTranslatef (2.5, 0.0, 0.0);               //  ƽ�Ʊ任T2
    glRotatef ((GLfloat) venus_day, 0.0, 1.0, 0.0);    // ������ת
    gluSphere(qobj, 0.2, longitude, latitude);//��������qobj
    gluDeleteQuadric(qobj);

}

//����ģ��
void earth()
{
    //�������
	GLfloat e_ambient[] = {0.0,0.0,1.0,1};// �������ɫ
	GLfloat e_emission[] = {0.2,0.2,0.2}; //�޹���ʱ��������ʾ����ɫ
	//��������
	GLfloat m_ambient[] = {0.5,0.5,1.0,1};
	GLfloat m_emission[] = {0.2,0.2,0.2}; //�޹���ʱ��������ʾ����ɫ

	//��ӵ������
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, e_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, e_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    gluQuadricTexture(qobj, GL_TRUE);//������,ʹ�ø��������꺯��ʱ����Ҫͬʱʹ�����������Զ�����glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) year, 0.0, 1.0, 0.0);  // ������̫����ת
    glTranslatef (3.5, 0.0, 0.0);               //  ƽ�Ʊ任T2
    if(!dim)
    {
        myCircle(0.4);
    }
    glPushMatrix();
    glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);    // ������ת
    gluSphere(qobj, 0.3, longitude, latitude);//��������qobj
    gluDeleteQuadric(qobj);

    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    gluQuadricTexture(qobj, GL_TRUE);//������,ʹ�ø��������꺯��ʱ����Ҫͬʱʹ�����������Զ�����glEnable(GL_TEXTURE_GEN_S);
    glPopMatrix();
    glRotatef ((GLfloat) moon_year, 0.0, 1.0, 0.0);  // �����Ƶ���ת
    glTranslatef (0.4, 0.0, 0.0);   //  ƽ�Ʊ任T2
    glRotatef ((GLfloat) moon_day, 0.0, 1.0, 0.0);    // ������ת
    //�����������
    glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, m_emission);
    gluSphere(qobj, 0.09, longitude, latitude);//��������qobj
    gluDeleteQuadric(qobj);

}

//����
void mars()
{
    //���ǲ���
    GLfloat s_ambient[] = {0.5,0.7,0.0,1};
    GLfloat s_emission[] = {0.2,0.2,0.2}; //�޹���ʱ��������ʾ����ɫ

	//��ӻ��ǲ���
	glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, s_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    gluQuadricTexture(qobj, GL_TRUE);//������,ʹ�ø��������꺯��ʱ����Ҫͬʱʹ�����������Զ�����glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) mars_year, 0.0, 1.0, 0.0);  // ������̫����ת
    glTranslatef (4.3, 0.0, 0.0);               //  ƽ�Ʊ任T2
    glRotatef ((GLfloat) mars_day, 0.0, 1.0, 0.0);    // ������ת
    gluSphere(qobj, 0.25, longitude, latitude);//��������qobj
    gluDeleteQuadric(qobj);
}

//ľ��
void jupiter()
{
    //ľ�ǲ���
    GLfloat s_ambient[] = {1.0,0.0,0.0,1};
    GLfloat s_emission[] = {0.2,0.2,0.2}; //�޹���ʱ��������ʾ����ɫ

	//���ľ�ǲ���
	glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, s_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    gluQuadricTexture(qobj, GL_TRUE);//������,ʹ�ø��������꺯��ʱ����Ҫͬʱʹ�����������Զ�����glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) jupiter_year, 0.0, 1.0, 0.0);  // ľ����̫����ת
    glTranslatef (5.2, 0.0, 0.0);               //  ƽ�Ʊ任T2
    glRotatef ((GLfloat) jupiter_day, 0.0, 1.0, 0.0);    // ľ����ת
    gluSphere(qobj, 0.5, longitude, latitude);//��������qobj
    gluDeleteQuadric(qobj);
}

//����
void saturn()
{
    //���ǲ���
    GLfloat s_ambient[] = {1.0,0.0,0.0,1};
    GLfloat s_emission[] = {0.2,0.2,0.2}; //�޹���ʱ��������ʾ����ɫ

	//������ǲ���
	glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, s_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    gluQuadricTexture(qobj, GL_TRUE);//������,ʹ�ø��������꺯��ʱ����Ҫͬʱʹ�����������Զ�����glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) saturn_year, 0.0, 1.0, 0.0);  // ������̫����ת
    glTranslatef (7, 0.0, 0.0);               //  ƽ�Ʊ任T2
    glRotatef ((GLfloat) 30, 1.0, 0.0, 0.0);    // ������ת
    glPushMatrix();
    glRotatef ((GLfloat) saturn_day, 0.0, 1.0, 0.0);  // ������̫����ת
    gluSphere(qobj, 0.5, longitude, latitude);//��������qobj
    gluDeleteQuadric(qobj);

     //���ǻ�����
    GLfloat tor_ambient[] = {1.0,0.0,0.0,1};
    GLfloat tor_emission[] = {0.2,0.2,0.2}; //�޹���ʱ��������ʾ����ɫ

	//������ǻ�����
	glMaterialfv(GL_FRONT, GL_AMBIENT, tor_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, tor_emission);

    glPopMatrix();
    glutSolidTorus(0.18, 0.75, 2, 200);

}

//������
void uranus()
{
    //�����ǲ���
	GLfloat uranus_ambient[] = {0.0,0.0,0.0,1};
	GLfloat uranus_emission[] = {0.2,0.2,0.2}; //�޹���ʱ��������ʾ����ɫ

	//��������ǲ���
	glMaterialfv(GL_FRONT, GL_AMBIENT, uranus_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, uranus_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    gluQuadricTexture(qobj, GL_TRUE);//������,ʹ�ø��������꺯��ʱ����Ҫͬʱʹ�����������Զ�����glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) uranus_year, 0.0, 1.0, 0.0);  // ��������̫����ת
    glTranslatef (8.5, 0.0, 0.0);               //  ƽ�Ʊ任T2
    glRotatef ((GLfloat) uranus_day, 0.0, 1.0, 0.0);    // ��������ת
    gluSphere(qobj, 0.30, longitude, latitude);//��������qobj
    gluDeleteQuadric(qobj);
}

// ������
void neptune()
{
    //ˮ�ǲ���
	GLfloat neptune_ambient[] = {0.0,0.0,0.0,1};
	GLfloat neptune_emission[] = {0.2,0.2,0.2}; //�޹���ʱ��������ʾ����ɫ

	//���ˮ�ǲ���
	glMaterialfv(GL_FRONT, GL_AMBIENT, neptune_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, neptune_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    gluQuadricTexture(qobj, GL_TRUE);//������,ʹ�ø��������꺯��ʱ����Ҫͬʱʹ�����������Զ�����glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) neptune_year, 0.0, 1.0, 0.0);  // ˮ����̫����ת
    glTranslatef (9.8, 0.0, 0.0);               //  ƽ�Ʊ任T2
    glRotatef ((GLfloat) neptune_day, 0.0, 1.0, 0.0);    // ˮ����ת
    gluSphere(qobj, 0.35, longitude, latitude);//��������qobj
    gluDeleteQuadric(qobj);
}

void display(void)
{

    glClear (GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);  //ʹ��ģ����ͼ����ջ�������ļ��α任
    glPushMatrix();              //������������ϵ���۲�����ϵ�ı任����

    if(dim)
    {
        if(flag)
        {
            gluLookAt(sx, sy, sz,
              center[0], center[1], center[2],
              0.0, 1.0, 0.0);
        }
        else
        {
            gluLookAt(sx1, sy1, sz1,
              center[0], center[1], center[2],
              0.0, 1.0, 0.0);
        }
    }
    else // ��ת����2D�󣬻��ƹ��
    {
        gluLookAt(0.0, height, 0.0,
              0.0, 0.0, 0.0,
              1.0, 0.0, 0.0);
              myCircle(2.0);
        //���
        myCircle(2.5);
        myCircle(3.5);
        myCircle(4.3);
        myCircle(5.2);
        myCircle(7.0);
        myCircle(8.5);
        myCircle(9.8);
    }

    //����
    stars();

    set_light();

    // ̫��
    glPushMatrix();
    sun();
    glPopMatrix();

    //ˮ��
    glPushMatrix();
    mercury();
    glPopMatrix();

    // ����
    glPushMatrix();
    venus();
    glPopMatrix();

    //����
    glPushMatrix();
    mars();
    glPopMatrix();

    //ľ��
    glPushMatrix();
    jupiter();
    glPopMatrix();

    //����
    glPushMatrix();
    saturn();
    glPopMatrix();

    //������
    glPushMatrix();
    uranus();
    glPopMatrix();

    //������
    glPushMatrix();
    neptune();
    glPopMatrix();

    //���������
    glPushMatrix();
    earth();
    glPopMatrix();

    glPopMatrix();                //�ָ���������ϵ���۲�����ϵ�ı任����
    glutSwapBuffers();

}

// ��ʱ�빫ת
void revolution()
{
    year = fmod((year + 7), 360);
    moon_year = fmod((moon_year + 2), 360);
    mercury_year = fmod((mercury_year + 20), 360);
    venus_year = fmod((venus_year + 10), 360);
    mars_year = fmod((mars_year + 4), 360);
    jupiter_year = fmod((jupiter_year + 1.6), 360);
    saturn_year = fmod((saturn_year + 0.6), 360);
    uranus_year = fmod((uranus_year + 0.2), 360);
    neptune_year = fmod((neptune_year + 0.1), 360);
}

// ˳ʱ�빫ת
void n_revolution()
{
    year = year = fmod((year - 7), 360);
    moon_year = fmod((moon_year - 2), 360);
    mercury_year = fmod((mercury_year - 20), 360);
    venus_year = fmod((venus_year - 10), 360);
    mars_year = fmod((mars_year - 4), 360);
    jupiter_year = fmod((jupiter_year - 1.6), 360);
    saturn_year = fmod((saturn_year - 0.6), 360);
    uranus_year = fmod((uranus_year - 0.2), 360);
    neptune_year = fmod((neptune_year - 0.1), 360);
}

//��ʱ����ת
void rotation()
{
    sun_day = fmod((sun_day + 2), 360);
    moon_day = fmod((moon_day + 2), 360);
    day = fmod((day + 7), 360);
    mercury_day = fmod((mercury_day + 0.7), 360);
    venus_day = fmod((venus_day + 0.2), 360);
    mars_day = fmod((mars_day + 7), 360);
    jupiter_day = fmod((jupiter_day + 18), 360);
    saturn_day = fmod((saturn_day + 0.3), 360);
    uranus_day = fmod((uranus_day + 11), 360);
    neptune_day = fmod((neptune_year + 10), 360);

}

//˳ʱ����ת
void n_rotation()
{
    sun_day = fmod((sun_day - 2), 360);
    moon_day = fmod((moon_day - 2), 360);
    day = fmod((day - 7), 360);
    mercury_day = fmod((mercury_day - 0.5), 360);
    venus_day = fmod((venus_day - 0.15), 360);
    mars_day = fmod((mars_day - 7), 360);
    jupiter_day = fmod((jupiter_day - 18), 360);
    saturn_day = fmod((saturn_day - 0.2), 360);
    uranus_day = fmod((uranus_day - 11), 360);
    neptune_day = fmod((neptune_year - 10), 360);

}

// �����������ô�С,������ͼ�β���Ӱ��
// ���� w�����ڿ�h�����ڳ�
void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);  //ʹ������С�����봰�ڵ���ʾ�����Сһ��
   glMatrixMode(GL_MODELVIEW);                    //ʹ��ģ����ͼ����ջ
   glLoadIdentity();

/* gluLookAt �����趨�˹۲��ӵ㣬�͹۲췽��
����ԭ�ͣ�gluLookAt(GLdoble eyex,GLdouble eyey,GLdouble eyez,GLdouble centerx,GLdouble centery,GLdouble centerz,GLdouble upx,GLdouble upy,GLdouble upz);
��ѡȡ eyex,eyey, eyez����Ϊԭ�㣨�۲�����ϵ�����꣨����������ϵ����ʾ�ģ�����
Ȼ��centerx, centery,centerz��ָ���˹۲췽�򣨣���ķ����򣩣���
upx,upy,upz��ָ���ˣ����������Ľ��Ʒ�������һ���ͣ��᷽��������������ͨ����ص����㣬�ҵ������ģ����������
�Ӷ�Ҳ�����ҵ��������������Ҫ����ͨ�������ĵ�˺Ͳ��������ģ�����
ʹ����gluLookAt������ʵ�����ǽ����˴���������ϵ���۲�����ϵ��ת���ľ��󣬺����ͶӰ�任���ڹ۲�����ϵ�½���
*/
   gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

   glMatrixMode (GL_PROJECTION);                 //ʹ��ͶӰ����ջ��׼���趨ͶӰ����
   glLoadIdentity ();                            //��ʼ��ͶӰ����Ϊ��λ����

// gluPerspective���������Ӿ��壨�ü���̨�����ĸ������ĺ����ͼ��
// �ú���ʵ���ϻ�����ĸ�������������ͶӰ�任��͸��ͶӰ����
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.5, 30.0);
}

// ���� key�������� (x��y)������ʱ����������
void keyboard (unsigned char key, int x, int y)
{

    switch (key)
    {
    case 'p':
        rotation();   //������ת�ĽǶȣ���ʱ����ת
        glutPostRedisplay();
        break;
    case 'P':
        n_rotation();   //������ת�ĽǶȣ�˳ʱ����ת
        glutPostRedisplay();
        break;
    case 'q':
        revolution();   //���ӹ�ת�ĽǶȣ���ʱ����ת
        glutPostRedisplay();
        break;
    case 'Q':
        n_revolution();//���ӹ�ת�ĽǶȣ�˳ʱ����ת
        glutPostRedisplay();
     break;
    case 'l':
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //ʹ���߿�ģ����ʾ����
        glDisable(GL_LIGHTING);
        longitude = 20;
        latitude=20;
        glutPostRedisplay();
        break;
    case 'f':
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //ʹ�ö�������ģ������ʾ����
        glEnable(GL_LIGHTING);
        longitude = 200;
        latitude=200;
        glutPostRedisplay();
        break;
    case 'w':   //ǰ��
        flag = 1;
        sz -= 1;
        sz1 = sz;
        center[2] -= 1;
        break;
    case 'a':  //����
        flag = 1;
        sx -= 1;
        sx1 = sx;
        center[0] -= 1;
        break;
    case 's':  //����
        flag =1;
        sz += 1;
        sz1 = sz;
        center[2] += 1;
        break;
    case 'd':  //����
        flag = 1;
        sx += 1;
        sx1 = sx;
        center[0] += 1;
        break;
    case 'z':  //����
        flag = 1;
        sy -= 1;
        sy1 = sy;
        center[1] -= 1;
        break;
    case 'c':  //����
        flag = 1;
        sy += 1;
        sy1 = sy;
        center[1] += 1;
        break;
    case 'r':  //�ص���ʼλ��
        flag = 1;
        sx = 0.0;
        sy = 0.0;
        sz = 3.0;
        sx1 = sx;
        sy1 = sy;
        sz1 = sz;
        center[0] = 0;
        center[1] = 0;
        center[2] = 0;
        break;
    case 't':   //����̫��ϵ�Զ���ת�ٶ�
        late+=10;
        break;
    case 'T':
        if(late > 0)
        {
            late-=10;
        }
        break;
    case 'x':  //����̫��ϵ�Զ�
        mode = 1;
        break;
    case 'X':
        mode = 0;
        break;
    case '2':  //2D�ӽ�
        height = 15.0;
        dim = 0;
        break;
    case '3':  //3D�ӽ�
        dim = 1;
        break;
    case 'e':  //��ֹ����
        exit(0);
        break;
    default:
        break;
    }
    glutPostRedisplay();

}

// ʵ��̫��ϵ�Զ���ת
void my_idle()
{
    if(mode) //0̫��ϵ���ֶ���ת��1̫��ϵ�Զ���ת
    {
        Sleep(late);  //�����ٶ�
        rotation();
        revolution();
        glutPostRedisplay();
    }
}

// ������¼���Ϣ���Կ����ӽǵ���ת
// ���� key�������� (x��y)������ʱ����������
void my_special_keyboard(int key, int x, int y)
{
    flag = 0;
    GLfloat tmp;
    // �Ƶ�˳ʱ����ת�ӽ�
    if(key == GLUT_KEY_LEFT)
    {
        theta = -0.03;
        tmp = sz1*cos(theta)-sx1*sin(theta);
        sx1 = sz1*sin(theta)+sx1*cos(theta);
        sz1 = tmp;
    }
    // �Ƶ���ʱ����ת�ӽ�
    else if(key == GLUT_KEY_RIGHT)
    {
        theta = 0.03 ;//(theta-1)%360;
        tmp = sz1*cos(theta)-sx1*sin(theta);
        sx1 = sz1*sin(theta)+sx1*cos(theta);
        sz1 = tmp;
    }
    glutPostRedisplay();
}

// �õ�������Ҽ���Ϣ������2D�ӽ��¸��ӵĸ߶�
// ���� button�������� state������״̬ (x��y)������ʱ����������
void myMouse(int button, int state, int x, int y)
{
    // ����2D�ӽ��¸߶�
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        height ++;
    }
    // ����2D�ӽ��¸߶�
    else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        if(height > 0)
        {
            height --;
        }
    }
}

int main(int argc, char** argv)
{
    printf("����������밴e��\n");
    printf("������ʱ��/˳ʱ�빫ת���밴q/Q��\n");
    printf("������ʱ��/˳ʱ����ת���밴p/P��\n");
    printf("�����߿�ģ��/���ģ����ʾ���ǣ��밴l/f��\n");
    printf("�������Ϊ2D/3D�ӽǣ��밴2/3��\n");
    printf("����������ת���������ҷ������\n");
    printf("����ǰ�����������ƶ�������wsadzc����\n");
    printf("�����ӽ�λ�ûص���ʼλ�ã��밴r��\n");
    printf("�������̫��ϵ�����ǹ�ת����/�죬�밴t/T��\n");
    printf("�������̫��ϵ�Զ���ת/ֹͣ��ת���밴x/X��\n");
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (1000, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);

    init ();
    init_stars();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(my_special_keyboard);
    glutMouseFunc(myMouse);     // ������ص�����
    glutIdleFunc(my_idle);

    glutMainLoop();

    return 0;
}
