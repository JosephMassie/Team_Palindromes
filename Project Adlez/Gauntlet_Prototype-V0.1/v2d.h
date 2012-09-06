#pragma once

#include <math.h>

// drawing and math methods that require the libraries above
#define V2D_SIN		sin
#define V2D_COS		cos
// modify this if changing V2D's data types
#define V2D_GLVERTEX_METHOD	glVertex2fv

// PI defined to reduce dependency on other math libraries
#define V2D_PI			(3.14159265358979323846)
#define V2D_2PI			(V2D_PI*2)
#define V2D_HALFPI		(V2D_PI/2)
#define V2D_QUARTERPI	(V2D_PI/4)

// standard for V2D floats
#define __VTYPE float
// standard V2D using floats
#define V2DF	V2D<__VTYPE>
#define V2DI	V2D<int>

/**
 * a two dimensional vector class, which includes methods to handle 2D 
 * geometric calculation, including rotation in various circumstances.
 * @author mvaganov@hotmail.com April 2010
 */
template<typename VTYPE>
class V2D
{
protected:
public:
	/** position in 2 Dimensional space */
	VTYPE x, y;
	/** how many dimensions a V2D<VTYPE> keeps track of (sizeof(V2D)/sizeof(VTYPE)) */
	static const int NUM_DIMENSIONS = 2;

	/** to access this class's type later */
	typedef VTYPE type;
	/** the first (initial) dimension */
	static const int _X = 0;
	/** the second dimension */
	static const int _Y = 1;

	/** access X */
	inline const VTYPE & getX()const{return x;}
	/** access Y */
	inline const VTYPE & getY()const{return y;}
	/** mutate X */
	inline void setX(const VTYPE & a_value){x=a_value;}
	/** mutate Y */
	inline void setY(const VTYPE & a_value){y=a_value;}
	/** mutate X */
	inline void addX(const VTYPE & a_value){x+=a_value;}
	/** mutate Y */
	inline void addY(const VTYPE & a_value){y+=a_value;}
	/** @return array containing points of this vector ([V2D::X], [V2D::Y]) */
	inline const VTYPE * getDimensions()const{return &x;}
	/** @return getDimensions()[a_dimensionField] */
	inline VTYPE getField(const int & a_dimensionField)const{return getDimensions()[a_dimensionField];}
	/** getDimensions()[a_dimensionField]=a_value; */
	inline void setField(const int & a_dimensionField, const VTYPE & a_value){(&x)[a_dimensionField]=a_value;}
	/** @param a_axis {X, Y} */
	inline void flipAxis(const int & a_axis){(&x)[a_axis]*=-1;}

	/** resets the value of this vector */
	inline void set(const VTYPE & a_x, const VTYPE & a_y){x = a_x;	y = a_y;}
	/** copy another vector */
	inline void set(const V2D<VTYPE> & a_v2d){set(a_v2d.x, a_v2d.y);}

	/** default constructor */
	V2D():x(0.0),y(0.0){}
	/** complete constructor */
	V2D(VTYPE a_x, VTYPE a_y):x(a_x),y(a_y){}
	/** turns a pi-radians angle into a vector */
	V2D(VTYPE a_piRadians):x(V2D_COS(a_piRadians)), y(V2D_SIN(a_piRadians)){}
	/** copy constructor */
	V2D(const V2D<VTYPE> & v):x(v.x),y(v.y){}

	/** sets x and y to zero */
	inline void zero(){x=y=0;}

	/**
	 * declares a "global" variable in a function, which is OK in a template! 
	 * (can't declare globals in headers otherwise)
	 */
	inline static const V2D<VTYPE> & ZERO()			{static V2D<VTYPE> ZERO(0,0);			return ZERO;}
	inline static const V2D<VTYPE> & ZERO_DEGREES()	{static V2D<VTYPE> ZERODEGREES(1,0);	return ZERODEGREES;}

	/**
	 * @return true if both x and y are zero
	 * @note function is not allowed to modify members in V2D
	 */
	inline bool isZero() const {return x == 0 && y == 0;}

	/** @return if both x and y are less than the given x and y */
	inline bool isLessThan(const V2D<VTYPE> & p)const{return x<p.x&&y<p.y;}
	/** @return if both x and y are greaterthan or equl to the given x and y */
	inline bool isGreaterThanOrEqualTo(const V2D<VTYPE> & p)const{return x>=p.x&&y>=p.y;}

	/** @return the squared length of the vector (avoids sqrt) "quadrance" */
	inline VTYPE lengthSq() const{return (VTYPE)(x*x+y*y);}

	/** @return the length of the vector (uses sqrt) */
	inline VTYPE length() const{return sqrt((VTYPE)lengthSq());}

	/** @return dot product of this and v2 */
	inline VTYPE dot(const V2D<VTYPE> & v2) const
	{
		return (VTYPE)(x * v2.x + y * v2.y);
	}

	/**
	 * @return positive if v2 is clockwise of this vector 
	 * (assume Y points down, X to right)
	 */
	inline VTYPE sign(const V2D<VTYPE> & v2) const{return (x*v2.y)-(y*v2.x);}

	/** @return the vector perpendicular to this one */
	inline V2D<VTYPE> perp() const{return V2D(-y, x);}

	/** @param a_length what to set this vector's magnitude to */
	inline void setLength(const VTYPE & a_length)
	{
		VTYPE l = length(ll);
		// "x = (x * a_maxLength) / l" is more precise than "x *= (a_maxLength/l)"
		x = (x * a_maxLength) / l;
		y = (y * a_maxLength) / l;
	}

	/**
	 * @param a_maxLength x and y adjusts so that the length does not exceed this
	 */
	inline void truncate(const VTYPE & a_maxLength)
	{
		VTYPE max2 = a_maxLength*a_maxLength;
		VTYPE ll = lengthSq();
		if(ll > max2)
		{
			VTYPE l = sqrt(ll);
			// "x = (x * a_maxLength) / l" is more precise than "x *= (a_maxLength/l)"
			x = (x * a_maxLength) / l;
			y = (y * a_maxLength) / l;
		}
	}

	/** @return the distance square between this vector and v2 (avoids sqrt) */
	inline VTYPE distanceSq(const V2D<VTYPE> & v) const
	{
		VTYPE dx = x-v.x, dy = y-v.y;
		return dx*dx+dy*dy;
	}

	/** @return the distance between this vector and v2 */
	inline VTYPE distance(const V2D<VTYPE> & v) const{return sqrt(distanceSq(v));}

	/** @return the vector that is the reverse of this vector */
	inline V2D<VTYPE> getReverse() const
	{
		return V2D<VTYPE>(-x, -y);
	}

	/** @return a new V2D<VTYPE> that is the sum of this V2D<VTYPE> and v */
	inline V2D<VTYPE> sum(const V2D<VTYPE> & v) const
	{
		return V2D<VTYPE>(x+v.x, y+v.y);
	}

	/** @return a new V2D<VTYPE> that is the difference of this V2D<VTYPE> and v */
	inline V2D<VTYPE> difference(const V2D<VTYPE> & v) const
	{
		return V2D<VTYPE>(x-v.x, y-v.y);
	}

	/** @return a new V2D<VTYPE> that is the product of this V2D<VTYPE> and v */
	inline V2D<VTYPE> product(const V2D<VTYPE> & v) const
	{
		return V2D<VTYPE>(x*v.x, y*v.y);
	}

	/** @return a new V2D<VTYPE> that is the product of this V2D<VTYPE> and v */
	inline V2D<VTYPE> product(const VTYPE & a_value) const
	{
		return V2D<VTYPE>(x*a_value, y*a_value);
	}

	/** @return a new V2D<VTYPE> that is the quotient of this V2D<VTYPE> and the given V2D*/
	inline V2D<VTYPE> quotient(const VTYPE & a_value) const
	{
		return V2D<VTYPE>(x/a_value, y/a_value);
	}

	/** @return a new V2D<VTYPE> that is the quotient of this V2D<VTYPE> and the given V2D<VTYPE> */
	inline V2D<VTYPE> quotient(const V2D<VTYPE> & v) const
	{
		return V2D<VTYPE>(x/v.x, y/v.y);
	}

	/** @return this V2D<VTYPE> after adding v */
	inline const V2D<VTYPE> & add(const V2D<VTYPE> & v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	/** @return this V2D<VTYPE> after subtracting v */
	inline const V2D<VTYPE> & subtract(const V2D<VTYPE> & v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	/** @return this V2D<VTYPE> after multiplying v */
	inline V2D<VTYPE> & multiply(const VTYPE & a_value)
	{
		x *= a_value;
		y *= a_value;
		return *this;
	}

	/** @return this V2D<VTYPE> after multiplying v */
	inline V2D<VTYPE> & multiply(const V2D<VTYPE> & v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	/** @return this V2D<VTYPE> after dividing v */
	inline V2D<VTYPE> & divide(const VTYPE & a_value)
	{
		x /= a_value;
		y /= a_value;
		return *this;
	}

	/** @return this V2D<VTYPE> after dividing v */
	inline V2D<VTYPE> & divide(const V2D<VTYPE> & v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	/** @return if this V2D<VTYPE> is euqal to v */
	inline bool isEqual(const V2D<VTYPE> & v) const
	{
		return (x == v.x && y == v.y);
	}

	/** @return true if this point is within a_radius from a_point */
	inline bool isWithin(const VTYPE & a_radius, const V2D<VTYPE> & a_point) const
	{
		VTYPE rr = a_radius*a_radius;
		return this->distanceSq(a_point) <= rr;
	}

	/** @return if this point is between the rectangle inscribed by the given corners */
	inline bool isBetweenRect(const V2D<VTYPE> & a, const V2D<VTYPE> & b)
	{
		return((a.x <= b.x)?(x>=a.x&&x<=b.x):(x>=b.x&&x<=a.x))
			&&((a.y <= b.y)?(y>=a.y&&y<=b.y):(y>=b.y&&y<=a.y));
	}

	/** @return true if the given vector is equivalant to this one */
	inline bool equals(const V2D<VTYPE> & v)const
	{return x == v.x && y == v.y;}

	/** forces vector to have a length of 1 */
	inline V2D<VTYPE> & normalize()
	{
		divide(length());
		return *this;
	}

	/** a normalized verson of this vector */
	inline V2D<VTYPE> normal() const
	{
		V2D<VTYPE> norm(*this);
		return norm.normalize();
	}

	/** @return radians between these normalized vector is */
	inline VTYPE piRadians(const V2D<VTYPE> & v) const
	{
		return V2D_COS(dot(v));
	}

	/** @return radians that this normalized vector is */
	inline VTYPE piRadians() const
	{
		return (y<0?-1:1)*piRadians(ZERO_DEGREES());
	}
	
	/** @return how many degrees (standard 360 degree scale) this is */
	inline VTYPE degrees() const
	{
		// altered by ME
		// Math. is not needed here
		return ((y > 0)?-1:1) * acos(x) * 180/V2D_PI;
	}

	/** @param a_normal cos(theta),sin(theta) as x,y values */
	inline void rotate(const V2D<VTYPE> & a_normal)
	{
		VTYPE len = length();	// remember length data
		// normalize()	// turn vector into simple angle, lose length data
		divide(len);	// same as normalize, but one less function call
		// calculate turn in-place within a new data structure
		V2D<VTYPE> turned(
			// x_ = x*cos(theta) - y*sin(theta)
			x*a_normal.x - y*a_normal.y, 
			// y_ = x*sin(theta) + y*cos(theta)
			x*a_normal.y + y*a_normal.x);
		// memory copy of structure
		*this = turned;
		// put length data back into normalized vector
		multiply(len);
	}

	/** @param a_degreePiRadians in piRadians */
	inline void rotate(const VTYPE & a_degreePiRadians)
	{
		rotate(V2D<VTYPE>(V2D_COS(a_degreePiRadians), V2D_SIN(a_degreePiRadians)));
	}

	/**
	 * @param a_fwd rotate this point's x axis to match this vector
	 * @param a_side rotate this point's y axis to match this vector
	 */
	inline V2D<VTYPE> toWorldSpace(const V2D<VTYPE> & a_fwd, const V2D<VTYPE> & a_side, 
		const V2D<VTYPE> & a_pos) const
	{
		return V2D<VTYPE>(
			(a_fwd.x*x) + (a_side.x*y) + (a_pos.x),
			(a_fwd.y*x) + (a_side.y*y) + (a_pos.y));
	}

	/**
	 * @param a_fwd rotate this point's x axis to match this vector
	 * @param a_side rotate this point's y axis to match this vector
	 */
	inline V2D<VTYPE> toWorldSpace(const V2D<VTYPE> & a_fwd, const V2D<VTYPE> & a_side, 
		const V2D<VTYPE> & a_pos, const V2D<VTYPE> & a_scale) const
	{
		return V2D<VTYPE>(
			(a_scale.x*a_fwd.x*x) + (a_scale.y*a_side.x*y) + (a_pos.x),
			(a_scale.x*a_fwd.y*x) + (a_scale.y*a_side.y*y) + (a_pos.y));
	}

	/**
	 * @param a_fwd rotate this point's x axis to match this vector
	 * @param a_side rotate this point's y axis to match this vector
	 */
	inline V2D<VTYPE> toWorldSpace(const V2D<VTYPE> & a_fwd, const V2D<VTYPE> & a_side) const
	{
		return V2D<VTYPE>((a_fwd.x*x) + (a_side.x*y), (a_fwd.y*x) + (a_side.y*y));
	}

	/**
	 * @param a_fwd vector of translated x dimension
	 * @param a_side vector of translated y dimension
	 * @param a_origin origin to translate this point in relation to
	 */
	inline V2D<VTYPE> toLocalSpace(const V2D<VTYPE> & a_fwd, const V2D<VTYPE> & a_side, const V2D<VTYPE> & a_origin) const
	{
		VTYPE tx = -a_origin.dot(a_fwd);
		VTYPE ty = -a_origin.dot(a_side);
		return V2D<VTYPE>(
			(a_fwd.x*x)  + (a_fwd.y*y)  + (tx),
			(a_side.x*x) + (a_side.y*y) + (ty));
	}

	/**
	 * organizes a list of 2D vectors into a circular curve (arc)
	 * @param a_startVector what to start at (normalized vector)
	 * @param a_angle the angle to increment the arc by V2D(piRadians)
	 * @param a_list the list of 2D vectors to map along the arc
	 * @param a_arcs the number of elements in a_list
	 */
	static void arc(const V2D<VTYPE> & a_startVector, const V2D<VTYPE> & a_angle, V2D<VTYPE> * a_list, const int & a_arcs)
	{
		VTYPE len = a_startVector.length();	// remember length data
		a_list[0] = a_startVector;				// copy starting point for calculations
		a_list[0].divide(len);					// normalize starting point
		V2D<VTYPE> * lastPoint = &a_list[0];	// faster memory reference than a_list[i-1]
		// calculate all points in the arc as normals (faster: no division)
		for(int i = 1; i < a_arcs; ++i)
		{
			// calculate rotation in next point
			(lastPoint+1)->set(
				// x_ = x*cos(theta) - y*sin(theta)
				lastPoint->x*a_angle.x - lastPoint->y*a_angle.y, 
				// y_ = x*sin(theta) + y*cos(theta)
				lastPoint->x*a_angle.y + lastPoint->y*a_angle.x);
			++lastPoint;
		}
		if(len != 1)
		{
			// put length data back into normalized vector
			for(int i = 0; i < a_arcs; ++i)
			{
				// embarassingly parallel
				a_list[i].multiply(len);
			}
		}
	}

	/**
	 * ensures wraps this V2D's x/y values around the given rectangular range
	 * @param a_min the minimum x/y
	 * @param a_max the maximum x/y
	 */
	inline void wrapAround(const V2D<VTYPE> & a_min, const V2D<VTYPE> & a_max)
	{
		VTYPE width = a_max.x - a_min.x;
		VTYPE height= a_max.y - a_min.y;
		while(x < a_min.x){	x += width;	}
		while(x > a_max.x){	x -= width;	}
		while(y < a_min.y){	y +=height;	}
		while(y > a_max.y){	y -=height;	}
	}

	/** @return the position half-way between line a->b */
	inline static V2D<VTYPE> between(const V2D<VTYPE> & a, const V2D<VTYPE> & b)
	{
		V2D<VTYPE> average = b.sum(a);
		average.divide(2.0);
		return average;
	}

	/**
	 * @param A,B line 1
	 * @param C,D line 2
	 * @param point __OUT to the intersection of line AB and CD
	 * @param dist __OUT the distance along line AB to the intersection
	 * @return true if intersection occurs between the lines
	 */
	static inline bool lineIntersection(const V2D<VTYPE> & A, const V2D<VTYPE> & B, 
										const V2D<VTYPE> & C, const V2D<VTYPE> & D, 
										VTYPE & dist, V2D<VTYPE> & point)
	{
		VTYPE rTop = (A.y-C.y)*(D.x-C.x)-(A.x-C.x)*(D.y-C.y);
		VTYPE rBot = (B.x-A.x)*(D.y-C.y)-(B.y-A.y)*(D.x-C.x);
		VTYPE sTop = (A.y-C.y)*(B.x-A.x)-(A.x-C.x)*(B.y-A.y);
		VTYPE sBot = (B.x-A.x)*(D.y-C.y)-(B.y-A.y)*(D.x-C.x);
		if ( (rBot == 0) || (sBot == 0))
		{
			//lines are parallel
			return false;
		}
		VTYPE r = rTop/rBot;
		VTYPE s = sTop/sBot;
		dist = A.distance(B) * r;
		point = A.sum(B.difference(A).product(r));
		return ( (r > 0) && (r < 1) && (s > 0) && (s < 1) );
	}

	/**
	 * @param a_out_closestPoint will be closest point to a_point on line AB
	 * @return true if a_out_closestPoint is actually on line AB
	 */
	static bool closestPointOnLine(const V2D<VTYPE> & A, const V2D<VTYPE> & B, 
		const V2D<VTYPE> & a_point, V2D<VTYPE> & a_out_closestPoint)
	{
		V2D<VTYPE> r = B.difference(A).perp();
		V2D<VTYPE> d = r.product(2).sum(a_point);
		V2D<VTYPE> c = a_point.difference(r);
		VTYPE dist;
		bool intersected = lineIntersection(A, B, c, d, dist, a_out_closestPoint);
		return intersected;
	}

	/** @return if circle (a_point,a_radius) crosses line (A,B) */
	static bool lineCrossesCircle(const V2D<VTYPE> & A, const V2D<VTYPE> & B, 
		const V2D<VTYPE> & a_point, const VTYPE & a_radius, V2D<VTYPE> & a_out_closePoint)
	{
		bool connectionOnLine = closestPointOnLine(A, B, a_point, a_out_closePoint);
		return(connectionOnLine && a_out_closePoint.distance(a_point) <= a_radius)
			|| a_point.distance(A) < a_radius
			|| a_point.distance(B) < a_radius;
	}

	// overloaded operators... used sparingly to avoid confusion
	const V2D<VTYPE> & operator+=(const V2D<VTYPE> & rhs){return add(rhs);}
	const V2D<VTYPE> & operator-=(const V2D<VTYPE> & rhs){return subtract(rhs);}
	const V2D<VTYPE> & operator*=(const VTYPE & rhs){return multiply(rhs);}
	const V2D<VTYPE> & operator/=(const VTYPE & rhs){return divide(rhs);}
	bool operator==(const V2D<VTYPE> & rhs) const{return isEqual(rhs);}
	bool operator!=(const V2D<VTYPE> & rhs) const{return !isEqual(rhs);}
};
