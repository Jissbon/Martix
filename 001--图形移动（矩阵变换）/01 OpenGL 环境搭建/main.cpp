
/*
 案例：实现矩阵的移动，利用矩阵的平移、旋转、综合变化等
 */
#include "GLTools.h"
#include "GLShaderManager.h"
#include "math3d.h"

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLBatch	squareBatch;
GLShaderManager	shaderManager;


GLfloat blockSize = 0.1f;
GLfloat vVerts[] = {
    -blockSize, -blockSize, 0.0f,
    blockSize, -blockSize, 0.0f,
    blockSize,  blockSize, 0.0f,
    -blockSize,  blockSize, 0.0f};



void SetupRC()
{
    //1.初始化
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f );
    shaderManager.InitializeStockShaders();
    
    //2.加载三角形
    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vVerts);
    squareBatch.End();
}

GLfloat xPos = 0.0f;//x轴平移量
GLfloat yPos = 0.0f;//y轴平移量
GLfloat zRot = 0;//旋转角度 - 需要抓换成弧度才能用于求矩阵
GLfloat xScale = 1;//x轴缩放量 缩放的计算方式是,原边长*缩放因子的绝对值
GLfloat yScale = 1;//y轴缩放量

void SpecialKeys(int key ,int x, int y){
    
    GLfloat stepSize = 0.025;
    
    if (key == GLUT_KEY_UP) {
        yPos += stepSize;
        zRot += 10.0;
        yScale += 0.1;
        xScale += 0.1;
    }
    
    if (key == GLUT_KEY_DOWN) {
        yPos -= stepSize;
        zRot -= 10.0;
        yScale -= 0.1;
        xScale -= 0.1;
    }
    
    if (key == GLUT_KEY_LEFT) {
        xPos -= stepSize;
        zRot -= 10.0;
        xScale -= 0.1;
        yScale -= 0.1;
    }
    
    if (key == GLUT_KEY_RIGHT) {
        xPos += stepSize;
        zRot += 10.0;
        xScale += 0.1;
        yScale += 0.1;
    }
    
    
    //检测边沿碰撞
    if (blockSize+xPos > 1) xPos = 1-blockSize;
    
    if (-blockSize + xPos < -1) xPos = blockSize - 1;

    if (blockSize + yPos > 1)  yPos = 1-blockSize;
    
    if (-blockSize + yPos < -1) yPos = blockSize - 1;
    
    
    glutPostRedisplay();
}

/// 开始渲染
void RenderScence(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    GLfloat redColor[] = { 1.0,0.0,0.0,1.0f};
    
    //定义矩阵
    M3DMatrix44f mFinalTranslation , mTranslationMatrix , mRotationMartix ,mScaleMartix;
    
    //计算平移矩阵
    m3dTranslationMatrix44(mTranslationMatrix, xPos, yPos, 0);
    
    //计算旋转矩阵
    m3dRotationMatrix44(mRotationMartix, m3dDegToRad(zRot),0.0, 0.0, 1.0);
    
    //计算缩放矩阵
    m3dScaleMatrix44(mScaleMartix, xScale, yScale, 0);
    
    // 平移 & 旋转 矩阵叉乘
    m3dMatrixMultiply44(mFinalTranslation, mTranslationMatrix, mRotationMartix);
    
    // 平移旋转后再缩放
    m3dMatrixMultiply44(mFinalTranslation, mFinalTranslation, mScaleMartix);
    
    
//    shaderManager.UseStockShader(GLT_SHADER_FLAT,mTranslationMatrix,redColor);//平移
//    shaderManager.UseStockShader(GLT_SHADER_FLAT,mRotationMartix,redColor);//旋转
//    shaderManager.UseStockShader(GLT_SHADER_FLAT,mScaleMartix,redColor);//缩放
    shaderManager.UseStockShader(GLT_SHADER_FLAT,mFinalTranslation,redColor); //叉乘方式

    squareBatch.Draw();
    
    glutSwapBuffers();
}



void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
}


int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Move Block with Arrow Keys");
    
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScence);
    glutSpecialFunc(SpecialKeys);
    
    SetupRC();
    
    glutMainLoop();
    return 0;
}
