#include "Collision.h"

void ClosestPointLine(float px, float py, Line &line, Vector2D &point) {

	/*Vector2D closestpoint;
	float lineSegmentx = line.x1 - line.x2;
	float lineSegmenty = line.y1 - line.y2;
	float proj = ((px - line.x2)*(lineSegmentx)+(py - line.y2)*(lineSegmenty)) / (lineSegmentx*lineSegmentx+lineSegmenty*lineSegmenty);

	if(proj <= 0.0f)    // closest point on the line segment is the tailPoint
	{
		closestpoint.x = line.x2;
		closestpoint.y = line.y2;
	}
	else if(proj >= 1.0f)    // closest point on the line segment is the headPoint
	{
		closestpoint.x = line.x1;
		closestpoint.y = line.y1;
	}
	else    // closest point lies on the line segment itself
	{
		closestpoint.x = line.x2 + lineSegmentx*proj;
		closestpoint.y = line.y2 + lineSegmenty*proj;

		//distance = (closestPoint - point).length();
	}
	return closestpoint;*/

	Vector2D p(px,py);
	Vector2D p1(line.x1,line.y1);
	Vector2D p2(line.x2,line.y2);

    Vector2D diff = p-p1;
	/*float diffx = px-line.x1;
	float diffy = py-line.y1;*/
	if (fabs(diff.x) < EPSILON && fabs(diff.y) < EPSILON) {
		//return p;
		//return Vector2D(px, py);
		point.x = p.x;
		point.y = p.y;
		return;
	}

    Vector2D dir = p2-p1;
	/*float dirx = line.x2-line.x1;
	float diry = line.y2-line.y1;*/
	if (fabs(dir.x) < EPSILON && fabs(dir.y) < EPSILON) {
		//return p1;
		//return Vector2D(line.x1, line.y1);
		point.x = p1.x;
		point.y = p1.y;
		return;
	}

    float dot1 = diff.Dot(dir);
    if (dot1 <= 0.0f) {
        //return p1;
		//return Vector2D(line.x1, line.y1);
		point.x = p1.x;
		point.y = p1.y;
		return;
    }    
    float dot2 = dir.Dot(dir);
    if (dot2 <= dot1) {
        //return p2;
		//return Vector2D(line.x2, line.y2);
		point.x = p2.x;
		point.y = p2.y;
		return;
    }
    float t = dot1/dot2;
    //return p1 + t * dir;
	//return Vector2D(line.x1+t*dirx, line.y1+t*diry);
	p1 += t*dir;
	point.x = p1.x;
	point.y = p1.y;
	return;
}

bool LineLineIntersect(Line &line1, Line &line2, Vector2D &intersectpoint, bool returni) {

	float p1x = line1.x1;
	float p1y = line1.y1;
	float p2x = line1.x2;
	float p2y = line1.y2;
	float p3x = line2.x1;
	float p3y = line2.y1;
	float p4x = line2.x2;
	float p4y = line2.y2;
	float divisor = (p2x-p1x)*(p4y-p3y)-(p2y-p1y)*(p4x-p3x);
	if (fabs(divisor) >= 0.000001f) {
		float invdiv = 1.0f/divisor;
		float r = ((p1y-p3y)*(p4x-p3x)-(p1x-p3x)*(p4y-p3y))*invdiv;
		float s = ((p1y-p3y)*(p2x-p1x)-(p1x-p3x)*(p2y-p1y))*invdiv;
		if ((0.0f <= r) && (r <= 1.0f) && (0.0f <= s) && (s <= 1.0f)) {		
			if (returni) {
				intersectpoint.x = p1x+r*(p2x-p1x);
				intersectpoint.y = p1y+r*(p2y-p1y);
			}
			return true;
		}
	}
	return false;
	/*Vector2D a2(line1.x1,line1.y1);
	Vector2D b2(line1.x2,line1.y2);
	Vector2D a1(line2.x1,line2.y1);
	Vector2D b1(line2.x2,line2.y2);
	Vector2D u = b1 - a1;
	Vector2D v = b2 - a2;
	float D = u.x * v.y - u.y * v.x;

	if (fabs(D) < EPSILON) return false; //parallel test

	Vector2D w = a1 - a2;

	float s = v.x * w.y - v.y * w.x;
	if (s < 0 || s > D) return false;

	float t = u.x * w.y - u.y * w.x;
	if (t < 0 || t > D) return false;
	
	if (returni) {
		intersectpoint = a1 + u * (s/D); //or b + v * (t/d);
	}
	return true;*/

}

bool LineCircleIntersect(Line &line, Circle &circle, Vector2D &d, float &l, bool returnd) {

	/*Vector2D O(line.x1,line.y1);
	Vector2D p2(line.x2,line.y2);
	if (O == p2) return false;
	Vector2D D = p2-O;*/
	Vector2D C(circle.x,circle.y);

	/*float t = D.Dot(C-O)/D.Dot(D);
	if (t < 0) {
		t = 0;
	}
	else if (t > 1) {
		t = 1;
	}

	Vector2D closest = O+t*D;
	d = C - closest;
	float ll = d.Dot(d);*/
	Vector2D closest;
	ClosestPointLine(circle.x, circle.y, line, closest);
	d = C - closest;
	float dist = d.LengthSquared();
	float r = circle.radius;
	if (dist < (r * r)) { //(ll < (r * r))
		if (returnd) {
			l = sqrtf(dist); //l = sqrtf(ll);
			if (l >= EPSILON) {
				d /= l;
			}
		}
		/*

		C += d * (r - l);*/
		return true;
	}
	return false;
}

bool LineCircleIntersect2(Line &line, Circle &circle, Vector2D& d, float &t) {
	Vector2D C(circle.x,circle.y);

	float x21 = line.x2-line.x1;
	float y21 = line.y2-line.y1;

	float x1c = circle.x-line.x1;
	float y1c = circle.y-line.y1;

	float a = x21*x21+y21*y21;
	float b = -2*(x21*x1c+y21*y1c);
	float c = x1c*x1c+y1c*y1c-circle.radius*circle.radius;


	float det = b*b-4*a*c;
	if (det == 0.0f) {
		t = -b/(2*a);
		if (t >= 0.0f && t <= 1.0f) {
			d.x = line.x1+x21*t;
			d.y = line.y1+y21*t;
			return true;
		}
	}
	else if (det > 0.0f) {
		det = sqrtf(det);
		
		t = (-b-det)/(2*a);
		if (t >= 0.0f && t <= 1.0f) {
			d.x = line.x1+x21*t;
			d.y = line.y1+y21*t;
			return true;
		}

		t = (-b+det)/(2*a);
		if (t >= 0.0f && t <= 1.0f) {
			d.x = line.x1+x21*t;
			d.y = line.y1+y21*t;
			return true;
		}
	}
	else {
	}
	return false;
}
