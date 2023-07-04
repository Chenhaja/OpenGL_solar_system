#include <windows.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include<math.h>
#include<direct.h>

#define PI 3.14
// 公转周期和自转周期
static float moon_year = 0, year = 0, mercury_year = 0, venus_year = 0,
mars_year = 0, jupiter_year = 0, saturn_year = 0,
uranus_year = 0, neptune_year = 0,
moon_day = 0, day = 0, mercury_day = 0, venus_day = 0,
mars_day = 0, jupiter_day = 0, saturn_day = 0,
uranus_day = 0, neptune_day = 0, sun_day = 0;
static int longitude = 200, latitude=200;  //球的精细度
// 视角变换变量
GLfloat dx=0.0, dy=1.0, sx=0.0, sy=0.0, sz=5, sx1=0.0, sy1=0.0, sz1=5;
GLfloat theta=0;
float star[1000][3]; // 星星位置
float center[] = { 0, 0, 0 };
int late = 30; // 控制自动运行时，各行星的速度
int mode = 1; //控制太阳系是否自动运行
int dim = 1;  //2D/3D视角
GLfloat height = 0;
int flag = 1;
// 贴图变量
static GLuint texture[10]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static GLfloat xequalzero[] = {1.0, 0.0, 0.0, 0.0};
static GLfloat yequalzero[] = {0.0, 1.0, 0.0, 0.0};
static GLfloat slanted[] = {1.0, 1.0, 1.0, 0.0};
static GLfloat *currentCoeff;
static GLenum currentPlane;
static GLint currentGenMode;

// 读取.bmp文件数据
// 输入 filename：文件地址；imagewidth：照片宽度；imageheight：照片高度
// 输出 pixeldata：文件数据
GLubyte*  readImg(char* filename, int * imagewidth, int * imageheight)
{
 //打开文件
  int pixellength;
  GLubyte *  pixeldata ;
  FILE* pfile=fopen(filename,"rb");
  if(pfile == 0) exit(0);
//读取图像大小
   fseek(pfile,0x0012,SEEK_SET);
   fread(imagewidth,sizeof(*imagewidth),1,pfile);
   fread(imageheight,sizeof(*imageheight),1,pfile);

//计算像素数据长度
   pixellength=(*imagewidth)*3;
   while(pixellength%4 != 0)pixellength++;
   pixellength *= (*imageheight);

 //读取像素数据
   pixeldata = (GLubyte*)malloc(pixellength);
   if(pixeldata == 0) exit(0);

   fseek(pfile,54,SEEK_SET);
   fread(pixeldata,pixellength,1,pfile);

    //关闭文件
   fclose(pfile);
   return pixeldata;
}

// 加载纹理
// 输入 filename：文件地址；texture：贴图ID
void LoadTexture(char * filename, GLuint &texture)
{
    GLubyte * data;
	GLint width, height;
	FILE * file;

	//读文件
    data=readImg(filename, &width, &height );
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);//绑定纹理
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//线性滤图
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //自动生成纹理坐标，使用球面映射
    currentCoeff = xequalzero;
    currentGenMode = GL_OBJECT_LINEAR;
    currentPlane = GL_OBJECT_PLANE;

	//生参数1:GL_UNPACK_ALIGNMENT ，指定OpenGL如何从数据缓存区中解包图像数据
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

	glEnable(GL_TEXTURE_2D);//打开纹理映射

	free(data); //释放纹理图像数据，纹理数据已由上一句生成并保存到纹理缓存中，使用完毕后应用glDeleteTextures释放纹理缓存
}

void init(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);    //对多边形内部所有点的颜色通过对多边形顶点颜色插值产生，相当于gouraud明暗处理

   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };  //漫放射系数，对应RGBA四个分量，A分量在混合开启后有效，光照反射颜色
   GLfloat mat_shininess[] = { 10.0 }; //表示材质的高光系数。较高的值会产生更小和更明亮的高光，而较低的值则会产生更大和更柔和的高光。


   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);//设置环境光系数和漫反射光系数
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);//设置材料反射指数

   // 启用6个灯光，在x、y和z轴各有两个
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glEnable(GL_LIGHT2);
   glEnable(GL_LIGHT3);
   glEnable(GL_LIGHT4);
   glEnable(GL_LIGHT5);

   LoadTexture("D:\\c\\final\\bmp\\sun.bmp",texture[0]);//创建并绑定纹理对象
   LoadTexture("D:\\c\\final\\bmp\\mercury.bmp",texture[1]);//创建并绑定纹理对象
   LoadTexture("D:\\c\\final\\bmp\\venus.bmp",texture[2]);//创建并绑定纹理对象
   LoadTexture("D:\\c\\final\\bmp\\earth.bmp",texture[3]);//创建并绑定纹理对象
   LoadTexture("D:\\c\\final\\bmp\\moon.bmp",texture[4]);//创建并绑定纹理对象
   LoadTexture("D:\\c\\final\\bmp\\mars.bmp",texture[5]);//创建并绑定纹理对象
   LoadTexture("D:\\c\\final\\bmp\\jupiter.bmp",texture[6]);//创建并绑定纹理对象
   LoadTexture("D:\\c\\final\\bmp\\saturn.bmp",texture[7]);//创建并绑定纹理对象
   LoadTexture("D:\\c\\final\\bmp\\uranus.bmp",texture[8]);//创建并绑定纹理对象
   LoadTexture("D:\\c\\final\\bmp\\neptune.bmp",texture[9]);//创建并绑定纹理对象

}

// 设置灯光参数
void set_light()
{
    // 光源位置
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

// 轨道

void myCircle(GLfloat R)
{
    int n = 80;
	//月亮材质
	GLfloat star_ambient[] = {1.0,1.0,1.0,0};
	GLfloat star_emission[] = {1.0,1.0,1.0}; //无关照时，物体显示的颜色
	//添加月亮材质
    glMaterialfv(GL_FRONT, GL_AMBIENT, star_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, star_emission);
	glBegin(GL_LINE_LOOP); //画正n边形
	for (int i = 0; i<n; ++i)
		glVertex3f(R*cos(2 * PI / n*i), 0, R*sin(2 * PI / n*i));
	glEnd();
}

// 星星
// 随机生成星星位置
void init_stars()
{
    for (int i = 0; i < 1000; i++)
        for (int j = 0; j < 3; j++)
            star[i][j] = rand() % 20 - 10;
}

// 用点代表星星
void stars()
{
    //月亮材质
	GLfloat star_ambient[] = {1.0,1.0,1.0,1};
	GLfloat star_emission[] = {1.0,1.0,1.0}; //无关照时，物体显示的颜色
	//添加月亮材质
    glMaterialfv(GL_FRONT, GL_AMBIENT, star_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, star_emission);
    glBegin(GL_POINTS);
    for (int i = 0; i < 1000; ++i) {
        glVertex3f(star[i][0], star[i][1], star[i][2]);
    }
    glEnd();
}

// 太阳
void sun()
{
    //太阳材质
    GLfloat s_ambient[] = {1.0,0.0,0.0,1};
	GLfloat s_emission[] = {1.0,0,0};

	//添加太阳材质
	glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, s_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    gluQuadricTexture(qobj, GL_TRUE);//纹理函数,使用该纹理坐标函数时，不要同时使用纹理坐标自动生成glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) sun_day, 0.0, 1.0, 0.0);    // 太阳自转
    gluSphere(qobj, 1.5, longitude, latitude);//二次曲面qobj
    gluDeleteQuadric(qobj);

}

//水星
void mercury()
{
    //水星材质
	GLfloat mercury_ambient[] = {0.8,0.8,0.8,1};
	GLfloat mercury_emission[] = {0.2,0.2,0.2}; //无关照时，物体显示的颜色

	//添加水星材质
	glMaterialfv(GL_FRONT, GL_AMBIENT, mercury_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, mercury_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    gluQuadricTexture(qobj, GL_TRUE);//纹理函数,使用该纹理坐标函数时，不要同时使用纹理坐标自动生成glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) mercury_year, 0.0, 1.0, 0.0);  // 水星绕太阳公转
    glTranslatef (2.0, 0.0, 0.0);               //  平移变换T2
    glRotatef ((GLfloat) mercury_day, 0.0, 1.0, 0.0);    // 水星自转
    gluSphere(qobj, 0.15, longitude, latitude);//二次曲面qobj
    gluDeleteQuadric(qobj);

}

//金星
void venus()
{
    //金星材质
    GLfloat s_ambient[] = {1.0,0.0,0.0,1};
    GLfloat s_emission[] = {0.2,0.2,0.2}; //无关照时，物体显示的颜色

	//添加金星材质
	glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, s_emission);
    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    gluQuadricTexture(qobj, GL_TRUE);//纹理函数,使用该纹理坐标函数时，不要同时使用纹理坐标自动生成glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) venus_year, 0.0, 1.0, 0.0);  // 金星绕太阳公转
    glTranslatef (2.5, 0.0, 0.0);               //  平移变换T2
    glRotatef ((GLfloat) venus_day, 0.0, 1.0, 0.0);    // 金星自转
    gluSphere(qobj, 0.2, longitude, latitude);//二次曲面qobj
    gluDeleteQuadric(qobj);

}

//地月模型
void earth()
{
    //地球材质
	GLfloat e_ambient[] = {0.0,0.0,1.0,1};// 物体的颜色
	GLfloat e_emission[] = {0.2,0.2,0.2}; //无关照时，物体显示的颜色
	//月亮材质
	GLfloat m_ambient[] = {0.5,0.5,1.0,1};
	GLfloat m_emission[] = {0.2,0.2,0.2}; //无关照时，物体显示的颜色

	//添加地球材质
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, e_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, e_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    gluQuadricTexture(qobj, GL_TRUE);//纹理函数,使用该纹理坐标函数时，不要同时使用纹理坐标自动生成glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) year, 0.0, 1.0, 0.0);  // 地球绕太阳公转
    glTranslatef (3.5, 0.0, 0.0);               //  平移变换T2
    if(!dim)
    {
        myCircle(0.4);
    }
    glPushMatrix();
    glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);    // 地球自转
    gluSphere(qobj, 0.3, longitude, latitude);//二次曲面qobj
    gluDeleteQuadric(qobj);

    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    gluQuadricTexture(qobj, GL_TRUE);//纹理函数,使用该纹理坐标函数时，不要同时使用纹理坐标自动生成glEnable(GL_TEXTURE_GEN_S);
    glPopMatrix();
    glRotatef ((GLfloat) moon_year, 0.0, 1.0, 0.0);  // 月亮绕地球公转
    glTranslatef (0.4, 0.0, 0.0);   //  平移变换T2
    glRotatef ((GLfloat) moon_day, 0.0, 1.0, 0.0);    // 月亮自转
    //添加月亮材质
    glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, m_emission);
    gluSphere(qobj, 0.09, longitude, latitude);//二次曲面qobj
    gluDeleteQuadric(qobj);

}

//火星
void mars()
{
    //火星材质
    GLfloat s_ambient[] = {0.5,0.7,0.0,1};
    GLfloat s_emission[] = {0.2,0.2,0.2}; //无关照时，物体显示的颜色

	//添加火星材质
	glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, s_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    gluQuadricTexture(qobj, GL_TRUE);//纹理函数,使用该纹理坐标函数时，不要同时使用纹理坐标自动生成glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) mars_year, 0.0, 1.0, 0.0);  // 火星绕太阳公转
    glTranslatef (4.3, 0.0, 0.0);               //  平移变换T2
    glRotatef ((GLfloat) mars_day, 0.0, 1.0, 0.0);    // 火星自转
    gluSphere(qobj, 0.25, longitude, latitude);//二次曲面qobj
    gluDeleteQuadric(qobj);
}

//木星
void jupiter()
{
    //木星材质
    GLfloat s_ambient[] = {1.0,0.0,0.0,1};
    GLfloat s_emission[] = {0.2,0.2,0.2}; //无关照时，物体显示的颜色

	//添加木星材质
	glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, s_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    gluQuadricTexture(qobj, GL_TRUE);//纹理函数,使用该纹理坐标函数时，不要同时使用纹理坐标自动生成glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) jupiter_year, 0.0, 1.0, 0.0);  // 木星绕太阳公转
    glTranslatef (5.2, 0.0, 0.0);               //  平移变换T2
    glRotatef ((GLfloat) jupiter_day, 0.0, 1.0, 0.0);    // 木星自转
    gluSphere(qobj, 0.5, longitude, latitude);//二次曲面qobj
    gluDeleteQuadric(qobj);
}

//土星
void saturn()
{
    //土星材质
    GLfloat s_ambient[] = {1.0,0.0,0.0,1};
    GLfloat s_emission[] = {0.2,0.2,0.2}; //无关照时，物体显示的颜色

	//添加土星材质
	glMaterialfv(GL_FRONT, GL_AMBIENT, s_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, s_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    gluQuadricTexture(qobj, GL_TRUE);//纹理函数,使用该纹理坐标函数时，不要同时使用纹理坐标自动生成glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) saturn_year, 0.0, 1.0, 0.0);  // 土星绕太阳公转
    glTranslatef (7, 0.0, 0.0);               //  平移变换T2
    glRotatef ((GLfloat) 30, 1.0, 0.0, 0.0);    // 土星自转
    glPushMatrix();
    glRotatef ((GLfloat) saturn_day, 0.0, 1.0, 0.0);  // 土星绕太阳公转
    gluSphere(qobj, 0.5, longitude, latitude);//二次曲面qobj
    gluDeleteQuadric(qobj);

     //土星环材质
    GLfloat tor_ambient[] = {1.0,0.0,0.0,1};
    GLfloat tor_emission[] = {0.2,0.2,0.2}; //无关照时，物体显示的颜色

	//添加土星环材质
	glMaterialfv(GL_FRONT, GL_AMBIENT, tor_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, tor_emission);

    glPopMatrix();
    glutSolidTorus(0.18, 0.75, 2, 200);

}

//天王星
void uranus()
{
    //天王星材质
	GLfloat uranus_ambient[] = {0.0,0.0,0.0,1};
	GLfloat uranus_emission[] = {0.2,0.2,0.2}; //无关照时，物体显示的颜色

	//添加天王星材质
	glMaterialfv(GL_FRONT, GL_AMBIENT, uranus_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, uranus_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    gluQuadricTexture(qobj, GL_TRUE);//纹理函数,使用该纹理坐标函数时，不要同时使用纹理坐标自动生成glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) uranus_year, 0.0, 1.0, 0.0);  // 天王星绕太阳公转
    glTranslatef (8.5, 0.0, 0.0);               //  平移变换T2
    glRotatef ((GLfloat) uranus_day, 0.0, 1.0, 0.0);    // 天王星自转
    gluSphere(qobj, 0.30, longitude, latitude);//二次曲面qobj
    gluDeleteQuadric(qobj);
}

// 海王星
void neptune()
{
    //水星材质
	GLfloat neptune_ambient[] = {0.0,0.0,0.0,1};
	GLfloat neptune_emission[] = {0.2,0.2,0.2}; //无关照时，物体显示的颜色

	//添加水星材质
	glMaterialfv(GL_FRONT, GL_AMBIENT, neptune_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, neptune_emission);

    GLUquadricObj*  qobj;
    qobj = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    gluQuadricTexture(qobj, GL_TRUE);//纹理函数,使用该纹理坐标函数时，不要同时使用纹理坐标自动生成glEnable(GL_TEXTURE_GEN_S);
    glRotatef ((GLfloat) neptune_year, 0.0, 1.0, 0.0);  // 水星绕太阳公转
    glTranslatef (9.8, 0.0, 0.0);               //  平移变换T2
    glRotatef ((GLfloat) neptune_day, 0.0, 1.0, 0.0);    // 水星自转
    gluSphere(qobj, 0.35, longitude, latitude);//二次曲面qobj
    gluDeleteQuadric(qobj);
}

void display(void)
{

    glClear (GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);  //使用模型视图矩阵栈保存后面的几何变换
    glPushMatrix();              //保存世界坐标系到观察坐标系的变换矩阵

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
    else // 在转换成2D后，绘制轨道
    {
        gluLookAt(0.0, height, 0.0,
              0.0, 0.0, 0.0,
              1.0, 0.0, 0.0);
              myCircle(2.0);
        //轨道
        myCircle(2.5);
        myCircle(3.5);
        myCircle(4.3);
        myCircle(5.2);
        myCircle(7.0);
        myCircle(8.5);
        myCircle(9.8);
    }

    //星星
    stars();

    set_light();

    // 太阳
    glPushMatrix();
    sun();
    glPopMatrix();

    //水星
    glPushMatrix();
    mercury();
    glPopMatrix();

    // 金星
    glPushMatrix();
    venus();
    glPopMatrix();

    //火星
    glPushMatrix();
    mars();
    glPopMatrix();

    //木星
    glPushMatrix();
    jupiter();
    glPopMatrix();

    //土星
    glPushMatrix();
    saturn();
    glPopMatrix();

    //天王星
    glPushMatrix();
    uranus();
    glPopMatrix();

    //海王星
    glPushMatrix();
    neptune();
    glPopMatrix();

    //地球和月亮
    glPushMatrix();
    earth();
    glPopMatrix();

    glPopMatrix();                //恢复世界坐标系到观察坐标系的变换矩阵
    glutSwapBuffers();

}

// 逆时针公转
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

// 顺时针公转
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

//逆时针自转
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

//顺时针自转
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

// 窗口重新设置大小,并且是图形不受影响
// 输入 w：窗口宽；h：窗口长
void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);  //使视区大小保持与窗口的显示区域大小一致
   glMatrixMode(GL_MODELVIEW);                    //使用模型视图矩阵栈
   glLoadIdentity();

/* gluLookAt 函数设定了观察视点，和观察方向
函数原型：gluLookAt(GLdoble eyex,GLdouble eyey,GLdouble eyez,GLdouble centerx,GLdouble centery,GLdouble centerz,GLdouble upx,GLdouble upy,GLdouble upz);
它选取 eyex,eyey, eyez　做为原点（观察坐标系的坐标（用世界坐标系来表示的）），
然后centerx, centery,centerz　指定了观察方向（ｚ轴的反方向），　
upx,upy,upz　指定了ｙ轴的正方向的近似方向（它不一定和ｚ轴方向正交，但可以通过相关的运算，找到正交的Ｙ轴的正方向，
从而也可以找到ｘ轴的正方向．主要就是通过向量的点乘和叉乘来计算的））．
使用了gluLookAt函数，实际上是建立了从世界坐标系到观察坐标系的转换的矩阵，后面的投影变换将在观察坐标系下进行
*/
   gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

   glMatrixMode (GL_PROJECTION);                 //使用投影矩阵栈，准备设定投影矩阵
   glLoadIdentity ();                            //初始化投影矩阵为单位矩阵

// gluPerspective函数定义视景体（裁剪棱台），四个参数的含义见图，
// 该函数实际上会根据四个参数计算用以投影变换的透视投影矩阵
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.5, 30.0);
}

// 输入 key：键名称 (x，y)：按键时，鼠标的坐标
void keyboard (unsigned char key, int x, int y)
{

    switch (key)
    {
    case 'p':
        rotation();   //增加自转的角度，逆时针旋转
        glutPostRedisplay();
        break;
    case 'P':
        n_rotation();   //增加自转的角度，顺时针旋转
        glutPostRedisplay();
        break;
    case 'q':
        revolution();   //增加公转的角度，逆时针旋转
        glutPostRedisplay();
        break;
    case 'Q':
        n_revolution();//增加公转的角度，顺时针旋转
        glutPostRedisplay();
     break;
    case 'l':
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //使用线框模型显示物体
        glDisable(GL_LIGHTING);
        longitude = 20;
        latitude=20;
        glutPostRedisplay();
        break;
    case 'f':
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //使用多边形填充模型型显示物体
        glEnable(GL_LIGHTING);
        longitude = 200;
        latitude=200;
        glutPostRedisplay();
        break;
    case 'w':   //前进
        flag = 1;
        sz -= 1;
        sz1 = sz;
        center[2] -= 1;
        break;
    case 'a':  //向右
        flag = 1;
        sx -= 1;
        sx1 = sx;
        center[0] -= 1;
        break;
    case 's':  //后退
        flag =1;
        sz += 1;
        sz1 = sz;
        center[2] += 1;
        break;
    case 'd':  //向左
        flag = 1;
        sx += 1;
        sx1 = sx;
        center[0] += 1;
        break;
    case 'z':  //向下
        flag = 1;
        sy -= 1;
        sy1 = sy;
        center[1] -= 1;
        break;
    case 'c':  //向上
        flag = 1;
        sy += 1;
        sy1 = sy;
        center[1] += 1;
        break;
    case 'r':  //回到初始位置
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
    case 't':   //控制太阳系自动运转速度
        late+=10;
        break;
    case 'T':
        if(late > 0)
        {
            late-=10;
        }
        break;
    case 'x':  //控制太阳系自动
        mode = 1;
        break;
    case 'X':
        mode = 0;
        break;
    case '2':  //2D视角
        height = 15.0;
        dim = 0;
        break;
    case '3':  //3D视角
        dim = 1;
        break;
    case 'e':  //中止程序
        exit(0);
        break;
    default:
        break;
    }
    glutPostRedisplay();

}

// 实现太阳系自动运转
void my_idle()
{
    if(mode) //0太阳系需手动运转，1太阳系自动运转
    {
        Sleep(late);  //控制速度
        rotation();
        revolution();
        glutPostRedisplay();
    }
}

// 获得上下键信息，以控制视角的旋转
// 输入 key：键名称 (x，y)：按键时，鼠标的坐标
void my_special_keyboard(int key, int x, int y)
{
    flag = 0;
    GLfloat tmp;
    // 绕点顺时针旋转视角
    if(key == GLUT_KEY_LEFT)
    {
        theta = -0.03;
        tmp = sz1*cos(theta)-sx1*sin(theta);
        sx1 = sz1*sin(theta)+sx1*cos(theta);
        sz1 = tmp;
    }
    // 绕点逆时针旋转视角
    else if(key == GLUT_KEY_RIGHT)
    {
        theta = 0.03 ;//(theta-1)%360;
        tmp = sz1*cos(theta)-sx1*sin(theta);
        sx1 = sz1*sin(theta)+sx1*cos(theta);
        sz1 = tmp;
    }
    glutPostRedisplay();
}

// 得到鼠标左右键信息，控制2D视角下俯视的高度
// 输入 button：键名称 state：键的状态 (x，y)：按键时，鼠标的坐标
void myMouse(int button, int state, int x, int y)
{
    // 增加2D视角下高度
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        height ++;
    }
    // 降低2D视角下高度
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
    printf("若想结束，请按e。\n");
    printf("若想逆时针/顺时针公转，请按q/Q。\n");
    printf("若想逆时针/顺时针自转，请按p/P。\n");
    printf("若想线框模型/填充模型显示行星，请按l/f。\n");
    printf("若想更换为2D/3D视角，请按2/3。\n");
    printf("若想左右旋转，请点击左右方向键。\n");
    printf("若想前后左右上下移动，请点击wsadzc键。\n");
    printf("若想视角位置回到初始位置，请按r。\n");
    printf("若想控制太阳系各行星公转变慢/快，请按t/T。\n");
    printf("若想控制太阳系自动运转/停止运转，请按x/X。\n");
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
    glutMouseFunc(myMouse);     // 鼠标点击回调函数
    glutIdleFunc(my_idle);

    glutMainLoop();

    return 0;
}
