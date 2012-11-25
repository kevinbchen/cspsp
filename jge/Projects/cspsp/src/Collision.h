#ifndef _COLLISION_H_
#define _COLLISION_H_


#include "JGE.h"
#include "Person.h"
#include <vector>

#ifdef WIN32
#include <math.h>
#else
#include <fastmath.h>
#endif

#define max2(a,b)            (((a) > (b)) ? (a) : (b))
#define min2(a,b)            (((a) < (b)) ? (a) : (b))

#define EPSILON 0.000001f  

struct Line
{
	float x1;
	float y1;
	float x2;
	float y2;

	Line(float _x1, float _y1, float _x2, float _y2): x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
	Line(Vector2D _p1, Vector2D _p2): x1(_p1.x), y1(_p1.y), x2(_p2.x), y2(_p2.y) {}
};

struct Circle
{
public:
	float x;
	float y;
	float radius;

	Circle(float _x, float _y, float _radius): x(_x), y(_y), radius(_radius) {}
	Circle(Vector2D _p, float _radius): x(_p.x), y(_p.y), radius(_radius) {}
};

struct Shape
{
public:
	std::vector<Vector2D> points;

	Shape(std::vector<Vector2D> _points): points(_points) {}
};



void ClosestPointLine(float px, float py, Line &line, Vector2D &point); //http://www.gamedev.net/community/forums/topic.asp?topic_id=398748
bool LineLineIntersect(Line &line1, Line &line2, Vector2D &intersectpoint, bool returni = false); //http://www.faqs.org/faqs/graphics/algorithms-faq/  -  http://www.gamedev.net/community/forums/topic.asp?topic_id=324883
bool LineCircleIntersect(Line &line, Circle &circle, Vector2D &d, float &l, bool returndl = false); //somewhere on gamedev
bool LineCircleIntersect2(Line &line, Circle &circle, Vector2D &d, float &t);
/*void PersonConstraintColl(Person* person, Constraint* c, float dt, bool changeTheta = true);
void PersonBodyColl(Person* person, Body* body, float dt, bool changeTheta = true);
void BodyBodyColl(Body* body1, Body* body2, float dt);
void ParticleBodyColl(Particle* p, std::vector<Constraint*> constraints, bool considerMass = true, float relaxation1 = 0.8f, float relaxation2 = 0.8f);*/

#endif
