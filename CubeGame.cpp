#include <GL/glut.h>
#include <cstdlib>
#include <memory>
#include <vector>

using namespace std;

static void Timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(10, Timer, value);
}

class Cube
{

public:
	float x;
	float y;
	float width;
	float height;
	float speed;

	Cube(float xp, float yp, float w, float h, float s)
	{
		x = xp;
		y = yp;
		width = w;
		height = h;
		speed = s;
	}

	virtual void draw() = 0;
	virtual void move() {};
};

class Enemy :public Cube
{

public:
	float fanLength;
	float fanSpeed;
	float fanX;
	float fanY;
	float rColor;
	float gColor;
	float bColor;

	Enemy(float xp, float yp, float w, float h, float s, float rc, float gc, float bc) :Cube(xp, yp, w, h, s)
	{
		fanLength = 48;
		fanSpeed = 2;
		fanX = xp + w/2;
		fanY = yp + h + 2;

		rColor = rc;
		gColor = gc;
		bColor = bc;
	}

	void draw()
	{
		glColor3f(rColor, gColor, bColor);

		glBegin(GL_POLYGON);
		glVertex3f(x, y, 0.0);
		glVertex3f(x + width, y, 0.0);
		glVertex3f(x + width, y + height, 0.0);
		glVertex3f(x, y + height, 0.0);
		glEnd();
		glFlush();

		fanLength += fanSpeed;

		if (fanLength >= 50)
		{
			fanSpeed *= -1;
			fanLength = 48;
		}

		if (fanLength <= 0)
		{
			fanSpeed *= -1;
			fanLength = 2;
		}

		glColor3f(1.0, 0.0, 0.7);

		glBegin(GL_LINES);
		glVertex2f(fanX - (fanLength / 2), fanY);
		glVertex2f(fanX + (fanLength / 2), fanY);
		glEnd();
		glFlush();
	}

	void move()
	{
		if (speed <= 0)
		{
			if (x >= 0)
			{
				x += speed;
				fanX += speed;
			}
			else
				speed *= -1;
		}

		else 
		{	
			if (x + width <= 480)
			{
				x += speed;
				fanX += speed;
			}
			else
				speed *= -1;
		}
	}

	void levelUp()
	{
		speed *= 1.5;
	}
};

class Player :public Cube
{

public:
	int score;
	int life;
	int array[4];
	int level;

	Player(float xp, float yp, float w, float h, float s) :Cube(xp, yp, w, h, s)
	{
		level = 0;
		score = 0;
		life = 3;
		for (int i = 0; i < 4; i++)
		{
			array[i] = 0;
		}
	}

	void draw()
	{
		glColor3f(1.0, 0.25, 1.0);

		glBegin(GL_POLYGON);
		glVertex3f(x, y, 0.0);
		glVertex3f(x + width, y, 0.0);
		glVertex3f(x + width, y + height, 0.0);
		glVertex3f(x, y + height, 0.0);
		glEnd();
		glFlush();
	}

	float moveIn(float coordinate, float s)
	{
		return coordinate + s;
	}

	void move()
	{
		for (int i = 0; i < 4; i++)
		{
			if (array[i] == 1)
			{
				array[i] = 0;
				
				if (i == 0)
				{
					x = moveIn(x, -speed);
				}

				if (i == 1)
				{
					x = moveIn(x, speed);
				}

				if (i == 2)
				{
					y = moveIn(y, -speed);
				}

				if (i == 3)
				{
					y = moveIn(y, speed);
				}

				if (x <= 0)
				{
					x = 0;
				}

				if (y <= 0)
				{
					y = 0;
				}

				if (x + width >= 480)
				{
					x = 480 - width;
				}

				if (y + height >= 640)
				{
					x = 215;
					y = 95;
					score++;
					level++;
				}
			}
		}
	}

	void collision(float xc, float w, float yc, float h)
	{
		if ((xc >= x && xc <= x + width) || (xc + w >= x && xc + w <= x + width))
		{
			if ((yc >= y && yc <= y + height) || (yc + h >= y && yc + h <= y + height))
			{
				life--;
				x = 215;
				y = 95;

				if (life <= 0)
				{
					exit(0);
				}
			}
		}
	}
};

shared_ptr<Player> myCube;
vector<shared_ptr<Enemy>> myEnemies;

void myInit()
{
	glClearColor(0.0, 1.0, 1.0, 0.0);
	
	glColor3f(1.0f, 0.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);

	glOrtho(0.0, 480.0, 0.0, 640.0, -1.0, 1.0);
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 97)
	{
		myCube->array[0] = 1;
	}

	if (key == 100)
	{
		myCube->array[1] = 1;
	}

	if (key == 115)
	{
		myCube->array[2] = 1;
	}

	if (key == 119)
	{
		myCube->array[3] = 1;
	}

	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.30, 0.55, 1.0);

	glBegin(GL_POLYGON);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(480.0, 0.0, 0.0);
	glVertex3f(480.0, 240.0, 0.0);
	glVertex3f(0.0, 240.0, 0.0);
	glEnd();
	glFlush();

	myCube->move();

	if (myCube->level == 1)
	{
		myCube->level = 0;
		for (int i = 0; i < myEnemies.size(); i++)
		{
			myEnemies[i]->levelUp();
		}
	}

	for (int i = 0; i < myEnemies.size(); i++)
	{
		myEnemies[i]->move();
		myEnemies[i]->draw();
		myCube->collision(myEnemies[i]->x, myEnemies[i]->width, myEnemies[i]->y, myEnemies[i]->height);
	}

	myCube->draw();

	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2f(0, 230);
	char pointText[5] = {'P','O','I','N','T'};
	
	for (int i = 0; i < 5; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, pointText[i]);
	}

	char point = myCube->score + '0';
	glColor3f(1.0, 0.0, 0.0);
	glRasterPos2f(50, 230);
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, point);

	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2f(0, 210);
	char lifeText[4] = { 'L','I','F','E'};

	for (int i = 0; i < 4; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, lifeText[i]);
	}

	char life = myCube->life + '0';
	glColor3f(1.0, 0.0, 0.0);
	glRasterPos2f(50, 210);
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, life);

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	myCube = make_shared<Player>(215, 95, 50, 50, 10);

	myEnemies.push_back(make_shared<Enemy>(0, 250, 50, 50, 0.2, 0.0, 0.0, 0.0));
	myEnemies.push_back(make_shared<Enemy>(430, 320, 50, 50, 0.3, 1.0, 1.0, 0.0));
	myEnemies.push_back(make_shared<Enemy>(430, 400, 50, 50, 0.4, 0.0, 1.0, 0.0));
	myEnemies.push_back(make_shared<Enemy>(0, 470, 50, 50, 0.5, 1.0, 0.0, 0.0));

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(480, 640);

	glutInitWindowPosition(0, 0);

	glutCreateWindow("Cube Game");

	myInit();

	glutKeyboardFunc(keyboard);

	glutDisplayFunc(render);

	Timer(0);

	glutMainLoop();

	return 0;
}