// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#ifndef BALL_MATHS_ANALYTICALGEOMETRY_H
#define BALL_MATHS_ANALYTICALGEOMETRY_H

#ifndef BALL_COMMON_EXCEPTION_H
# include <BALL/COMMON/exception.h>
#endif

#ifndef BALL_MATHS_ANGLE_H
#	include <BALL/MATHS/angle.h>
#endif

#ifndef BALL_MATHS_CIRCLE3_H
#	include <BALL/MATHS/circle3.h>
#endif

#ifndef BALL_MATHS_LINE3_H
#	include <BALL/MATHS/line3.h>
#endif

#ifndef BALL_MATHS_PLANE3_H
#	include <BALL/MATHS/plane3.h>
#endif

#ifndef BALL_MATHS_SPHERE3_H
#	include <BALL/MATHS/sphere3.h>
#endif

#ifndef BALL_MATHS_VECTOR3_H
#	include <BALL/MATHS/vector3.h>
#endif

#define BALL_MATRIX_CELL(m, dim, row, col)   *((m) + (row) * (dim) + (col))
#define BALL_CELL(x, y)                      *((m) + (y) * (dim) + (x))

namespace BALL 
{

	/**	\defgroup AnalyticalGeometry Analytical Geometry
			representation of analytical geometry functions,
			using the classes: TAngle, TCircle3, TLine3, TPlane3, TSphere3, TVector3.
	  \ingroup Mathematics
	*/
	//@{

	/**	Subroutine to get the determinant of any matrix.
			Direct usage of this function should be avoided.
			Instead use <tt>T getDeterminant(const T* m, Size dim) </tt>
			@param	m pointer to matrix
			@param	dim dimension of the matrix
	*/
	template <typename T>
	BALL_INLINE
	T getDeterminant_(const T* m, Size dim)
	{
		T determinant = 0;
		Index dim1 = dim - 1;

		if (dim > 1)
		{
            auto submatrix = new T[dim1 * dim1];

			for (Index i = 0; i < (Index)dim; ++i) 
			{
				for (Index j = 0; j < dim1; ++j) 
				{
					for (Index k = 0; k < dim1; ++k) 
					{
						*(submatrix + j * dim1 + k) = *(m + (j + 1) * dim + (k < i ? k : k + 1));
					}
				}
				determinant += *(m + i) * (i / 2.0 == i / 2 ? 1 : -1) * getDeterminant_(submatrix, dim1);
			}

			delete [] submatrix;
		} 
		else 
		{
			determinant = *m;
		}

		return determinant;
	}

	/**	Get the determinant of any matrix.
			@param	m pointer to matrix
			@param	dim dimension of the matrix
	*/
	template <typename T>
	T getDeterminant(const T* m, Size dim)
	{
		if (dim == 2)
		{
			return (BALL_CELL(0,0) * BALL_CELL(1,1) - BALL_CELL(0,1) * BALL_CELL(1,0));
		} 
		else if (dim == 3)
		{
			return (  BALL_CELL(0,0) * BALL_CELL(1,1) * BALL_CELL(2,2) 
							+ BALL_CELL(0,1) * BALL_CELL(1,2) * BALL_CELL(2,0) 
							+ BALL_CELL(0,2) * BALL_CELL(1,0) * BALL_CELL(2,1) 
							- BALL_CELL(0,2) * BALL_CELL(1,1) * BALL_CELL(2,0) 
							- BALL_CELL(0,0) * BALL_CELL(1,2) * BALL_CELL(2,1) 
							- BALL_CELL(0,1) * BALL_CELL(1,0) * BALL_CELL(2,2)); 
		} 
		else 
		{
			return getDeterminant_(m, dim);
		}
	}

	/**	Get the determinant of an 2x2 matrix.
			@param	m pointer to matrix
	*/
	template <typename T>
	BALL_INLINE 
	T getDeterminant2(const T* m)
	{
		Size dim = 2;
		return (BALL_CELL(0,0) * BALL_CELL(1,1) - BALL_CELL(0,1) * BALL_CELL(1,0));
	}

	/**	Get the determinant of an 2x2 matrix.
			@param	m00 first value of the matrix
			@param	m01 second value of the matrix
			@param	m10 third value of the matrix
			@param	m11 fourth value of the matrix
	*/
	template <typename T>
	BALL_INLINE 
	T getDeterminant2(const T& m00, const T& m01, const T& m10, const T& m11)
	{
		return (m00 * m11 - m01 * m10);
	}

	/**	Get the determinant of an 3x3 matrix.
			@param	m pointer to matrix
	*/
	template <typename T>
	BALL_INLINE 
	T getDeterminant3(const T *m)
	{
		Size dim = 3;
		return (  BALL_CELL(0,0) * BALL_CELL(1,1) * BALL_CELL(2,2) 
						+ BALL_CELL(0,1) * BALL_CELL(1,2) * BALL_CELL(2,0) 
						+ BALL_CELL(0,2) * BALL_CELL(1,0) * BALL_CELL(2,1) 
						- BALL_CELL(0,2) * BALL_CELL(1,1) * BALL_CELL(2,0) 
						- BALL_CELL(0,0) * BALL_CELL(1,2) * BALL_CELL(2,1) 
						- BALL_CELL(0,1) * BALL_CELL(1,0) * BALL_CELL(2,2)); 
	}

	/**	Get the determinant of an 3x3 matrix.
			@param	m00, m01, m02, m10, m11, m12, m20, m21, m22 the elements of the matrix
	*/
	template <typename T>
	BALL_INLINE T 
	getDeterminant3(const T& m00, const T& m01, const T& m02, const T& m10, const T& m11, const T& m12, const T& m20, const T& m21, const T& m22)
	{
		return (  m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m02 * m11 * m20 - m00 * m12 * m21 - m01 * m10 * m22); 
	}

	/**	Solve a system of linear equations.
		  Given a system of linear equations \par
			 \par
			\f$
				\begin{array}{ccccccccc}
				 a_{1,1} x_1 & + & a_{1,2} x_2 & + & \ldots & + & a_{1,n} x_n & = & a_{1,(n+1)} \\
				 a_{2,1} x_1 & + & a_{2,2} x_2 & + & \ldots & + & a_{2,n} x_n & = & a_{2,(n+1)} \\
				   \vdots    &   &   \vdots    &   & \ddots &   &   \vdots    &   &   \vdots \\
				 a_{n,1} x_1 & + & a_{n,2} x_2 & + & \ldots & + & a_{n,n} x_n & = & a_{n,(n+1)} \\
				\end{array}
			\f$
			 \par
			in matrix form, identify the solution \f$x = (x_1, x_2,\ldots x_N)\f$. \par
			<tt>m</tt> should point to a C-style array containing the \f$n\times(n+1)\f$ matrix <b>A</b>. \par
			The elements of <b>A</b> are row-ordered, i.e., they are ordered like this: \par
			\f$
				a_{1,1}, a_{1,2}, \cdot, a_{1,(n+1)}, a_{2,1}, \ldots a_{n,(n+1)}
			\f$ \par
			<tt>x</tt> points to a C-style array that will contain the solution vector <b>x</b>
			upon successful termination of the function. \par
			If there is no solution or the system is under-determined, return <b>false</b>.
			@param	m pointer to the factors in the equations
			@param	x pointer in which the results are stored
			@param  dim the dimension of the equation system (number of variables)
			@return bool <tt>true</tt> if a solution is found
	*/
	template <typename T>
	bool SolveSystem(const T* m, T* x, const Size dim)
	{
		T pivot;
		Index i, j, k, p;
		// the column dimension of the matrix
		const Size col_dim = dim + 1;
        auto matrix = new T[dim * (dim + 1)];
		const T* source = m;
		T* target = (T*)matrix;
		T* end = (T*)&BALL_MATRIX_CELL(matrix, col_dim, dim - 1, dim);

		while (target <= end)
		{
			*target++ = *source++;
		}

		for (i = 0; i < (Index)dim; ++i)
		{
			pivot = BALL_MATRIX_CELL(matrix, col_dim, i, i);
			p = i;
			for (j = i + 1; j < (Index)dim; ++j)
			{
				if (Maths::isLess(pivot, BALL_MATRIX_CELL(matrix, col_dim, j, i)))
				{
					pivot = BALL_MATRIX_CELL(matrix, col_dim, j, i);
					p = j;
				}
			}

			if (p != i)
			{
				T tmp;

				for (k = i; k < (Index)dim + 1; ++k)
				{
					tmp = BALL_MATRIX_CELL(matrix, dim, i, k);
					BALL_MATRIX_CELL(matrix, col_dim, i, k) = BALL_MATRIX_CELL(matrix, col_dim, p, k);
					BALL_MATRIX_CELL(matrix, col_dim, p, k) = tmp;
				}
			}
			else if (Maths::isZero(pivot) || Maths::isNan(pivot))
			{ 
				// invariant: matrix m is singular
				delete [] matrix;
				
				return false;
			}

			for (j = dim; j >= i; --j) 
			{
				BALL_MATRIX_CELL(matrix, col_dim, i, j) /= pivot;
			}

			for (j = i + 1; j < (Index)dim; ++j)
			{
				pivot = BALL_MATRIX_CELL(matrix, col_dim, j, i);

				for (k = dim; k>= i; --k) 
				{
					BALL_MATRIX_CELL(matrix, col_dim, j, k) -= pivot * BALL_MATRIX_CELL(matrix, col_dim, i, k);
				}
			}
		}

		x[dim - 1] = BALL_MATRIX_CELL(matrix, col_dim, dim - 1, dim);

		for (i = dim - 2; i >= 0; --i)
		{
			x[i] = BALL_MATRIX_CELL(matrix, col_dim, i, dim);

			for (j = i + 1; j < (Index)dim; ++j) 
			{
				x[i] -= BALL_MATRIX_CELL(matrix, col_dim, i, j) * x[j];	
			}
		}

		delete [] matrix;
		
		return true;
	}

#undef BALL_CELL
#undef BALL_MATRIX_CELL

	/**	Solve a system of two equations of the form
		  \f$a_1 x_1 + b_1 x_2 = c_1\f$ and 
		  \f$a_2 x_1 + b_2 x_2 = c_2\f$.
			@param	a1, b1, c1, a2, b2, c2 constants of the system
			@param x1 the first solution
			@param x2 the second solution
			@return bool <tt>true</tt> if a solution is found
	*/
	template <typename T>
	BALL_INLINE 
	bool SolveSystem2(const T& a1, const T& b1, const T& c1, const T& a2, const T& b2, const T& c2, T& x1, T& x2)
	{
		T quot = (a1 * b2 - a2 * b1);

		if (Maths::isZero(quot))
		{
			return false;
		}

		x1 = (c1 * b2 - c2 * b1) / quot;
		x2 = (a1 * c2 - a2 * c1) / quot;

		return true;
	}

	/**	Solve a quadratic equation of the form
			a \f$x^2 + b x + c = 0\f$.
			@param	a
			@param	b 
			@param	c 
			@param x1 the first solution
			@param x2 the second solution
			@return short the number of solutions (0 - 2)
	*/
	template <typename T>
	short	SolveQuadraticEquation(const T& a, const T& b, const T &c, T &x1, T &x2)
	{
		if (a == 0)
		{
			if (b == 0)
			{
				return 0;
			}
			x1 = x2 = c / b;
			return 1;
		}
		T discriminant = b * b - 4 * a * c;

		if (Maths::isLess(discriminant, 0))
		{
			return 0;
		}

		T sqrt_discriminant = sqrt(discriminant);
		if (Maths::isZero(sqrt_discriminant))
		{
			x1 = x2 = -b / (2 * a);

			return 1;
		} 
		else 
		{
			x1 = (-b + sqrt_discriminant) / (2 * a);
			x2 = (-b - sqrt_discriminant) / (2 * a);

			return 2;
		}
	}

	/**	Get the partition of two vectors.
			@param	a the first vector
			@param	b the second vector
			@return TVector3 the partition
	*/
	template <typename T>
	BALL_INLINE 
	TVector3<T> GetPartition(const TVector3<T>& a, const TVector3<T>& b)
	{
		return TVector3<T>((b.x + a.x) / 2, (b.y + a.y) / 2, (b.z + a.z) / 2);
	}

	/**	Get the partition of two vectors, calculated
			with two ratio factors.
			@param	a the first vector
			@param	b the second vector
			@param  r the ratio factor of the first vector
			@param  s the ratio factor of the second vector
			@return TVector3 the partition
			@throw  Exception::DivisionByZero if r+s == 0
	*/
	template <typename T>
	BALL_INLINE 
	TVector3<T> GetPartition(const TVector3<T>& a, const TVector3<T>& b, const T& r, const T& s)
	{
		T sum = r + s;
		if (sum == (T)0)
		{
			throw Exception::DivisionByZero(__FILE__, __LINE__);
		}		
		return TVector3<T>
			((s * a.x + r * b.x) / sum,
			 (s * a.y + r * b.y) / sum,
			 (s * a.z + r * b.z) / sum);
	}

	/**	Get the distance between two points.
			@param	a the first point
			@param	b the second point
			@return T the distance
	*/
	template <typename T>
	BALL_INLINE 
	T GetDistance(const TVector3<T>& a, const TVector3<T>& b)
	{
		T dx = a.x - b.x;
		T dy = a.y - b.y;
		T dz = a.z - b.z;
		
		return sqrt(dx * dx + dy * dy + dz * dz); 
	}

	/**	Get the distance between a line and a point.
			@param	line the line
			@param	point the point
			@return T the distance
			@throw  Exception::DivisionByZero if the line has length 0
	*/
	template <typename T>
	BALL_INLINE 
	T GetDistance(const TLine3<T>& line, const TVector3<T>& point)
	{
		if (line.d.getLength() == (T)0)
		{
			throw Exception::DivisionByZero(__FILE__, __LINE__);
		}		
		return ((line.d % (point - line.p)).getLength() / line.d.getLength());
	}

	/**	Get the distance between a point and a line.
			@param	point the point
			@param	line the line
			@return T the distance
			@throw  Exception::DivisionByZero if the line has length 0
	*/
	template <typename T>
	BALL_INLINE 
	T GetDistance(const TVector3<T>& point, const TLine3<T>& line)
	{
		return GetDistance(line, point);
	}

	/**	Get the distance between two lines.
			@param	a the first line
			@param	b the second line
			@return T the distance
			@throw  Exception::DivisionByZero if the lines are parallel and a has length 0
	*/
	template <typename T>
	T GetDistance(const TLine3<T>& a, const TLine3<T>& b)
	{
		T cross_product_length = (a.d % b.d).getLength();
		
		if (Maths::isZero(cross_product_length))
		{ // invariant: parallel lines
			if (a.d.getLength() == (T)0)
			{
				throw Exception::DivisionByZero(__FILE__, __LINE__);
			}					
			return ((a.d % (b.p - a.p)).getLength() / a.d.getLength());
		} 
		else 
		{
			T spat_product = TVector3<T>::getTripleProduct(a.d, b.d, b.p - a.p);

			if (Maths::isNotZero(spat_product))
			{ // invariant: windschiefe lines
				
				return (Maths::abs(spat_product) / cross_product_length);
			} 
			else 
			{ 
				// invariant: intersecting lines
				return 0;
			}
		}
	}

	/**	Get the distance between a point and a plane.
			@param	point the point
			@param	plane the plane
			@return T the distance
			@throw  Exception::DivisionByZero if the normal vector of plane has zero length
	*/
	template <typename T>
	BALL_INLINE 
	T GetDistance(const TVector3<T>& point, const TPlane3<T>& plane)
	{
		T length = plane.n.getLength();

		if (length == (T)0)
		{
			throw Exception::DivisionByZero(__FILE__, __LINE__);
		}
		return (Maths::abs(plane.n * (point - plane.p)) / length);
	}

	/**	Get the distance between a plane and a point.
			@param	plane the plane
			@param	point the point
			@return T the distance
			@throw  Exception::DivisionByZero if the normal vector of plane has zero length
	*/
	template <typename T>
	BALL_INLINE 
	T GetDistance(const TPlane3<T>& plane, const TVector3<T>& point)
	{
		return GetDistance(point, plane);
	}

	/**	Get the distance between a line and a plane.
			@param	line the line
			@param	plane the plane
			@return T the distance
			@throw  Exception::DivisionByZero if the normal vector of plane has zero length
	*/
	template <typename T>
	BALL_INLINE 
	T GetDistance(const TLine3<T>& line, const TPlane3<T>& plane)
	{
		T length = plane.n.getLength();
		if (length == (T)0)
		{
			throw Exception::DivisionByZero(__FILE__, __LINE__);
		}
		return (Maths::abs(plane.n * (line.p - plane.p)) / length);
	}

	/**	Get the distance between a plane and a line.
			@param	plane the plane
			@param	line the line
			@return T the distance
			@throw  Exception::DivisionByZero if the normal vector of plane has zero length
	*/
	template <typename T>
	BALL_INLINE 
	T GetDistance(const TPlane3<T>& plane, const TLine3<T>& line)
	{
		return GetDistance(line, plane);
	}

	/**	Get the distance between two planes.
			@param	a the first plane
			@param	b the second plane
			@return T the distance
			@throw  Exception::DivisionByZero if the normal vector of a has zero length
	*/
	template <typename T>
	BALL_INLINE 
	T GetDistance(const TPlane3<T>& a, const TPlane3<T>& b)
	{
		T length = a.n.getLength();
		if (length == (T)0)
		{
			throw Exception::DivisionByZero(__FILE__, __LINE__);
		}		
		return (Maths::abs(a.n * (a.p - b.p)) / length);
	}

	/**	Get the angle between two Vector3.
			@param	a the first vector
			@param	b the second vector
			@param	intersection_angle the resulting angle
			@return bool, always true
	*/
	template <typename T>
	BALL_INLINE 
	bool GetAngle(const TVector3<T>& a, const TVector3<T>& b,	TAngle<T> &intersection_angle)
	{
		T length_product = a.getSquareLength() *  b.getSquareLength();
		if(Maths::isZero(length_product))
		{
			return false;
		}
		intersection_angle = a.getAngle(b);
		return true;
	}

	/**	Get the angle between two lines.
			@param	a the first line
			@param	b the second line
			@param	intersection_angle the resulting angle
			@return bool, true if an angle can be calculated, otherwise false
	*/
	template <typename T>
	BALL_INLINE 
	bool GetAngle(const TLine3<T>& a, const TLine3<T>& b, TAngle<T>& intersection_angle)
	{
		T length_product = a.d.getSquareLength() *  b.d.getSquareLength();

		if(Maths::isZero(length_product))
		{
			return false;
		}
		intersection_angle = acos(Maths::abs(a.d * b.d) / sqrt(length_product));
		return true;
	}

	/**	Get the angle between a plane and a Vector3.
			@param	plane the plane
			@param	vector the Vector3 
			@param	intersection_angle the resulting angle
			@return bool, true if an angle can be calculated, otherwise false
	*/
	template <typename T>
	BALL_INLINE 
	bool GetAngle(const TPlane3<T>& plane, const TVector3<T>& vector, TAngle<T>& intersection_angle)
	{
		T length_product = plane.n.getSquareLength() * vector.getSquareLength();
		
		if (Maths::isZero(length_product))
		{
			return false;
		} 
		else 
		{
			intersection_angle = asin(Maths::abs(plane.n * vector) / sqrt(length_product));
			return true;
		}
	}

	/**	Get the angle between a vector3 and a plane.
			@param	vector the vector3
			@param	plane the plane
			@param	intersection_angle the resulting angle
			@return bool, true if an angle can be calculated, otherwise false
	*/
	template <typename T>
	BALL_INLINE 
	bool GetAngle(const TVector3<T>& vector ,const TPlane3<T>& plane, TAngle<T> &intersection_angle)
	{
		return GetAngle(plane, vector, intersection_angle);
	}

	/**	Get the angle between a plane and a line.
			@param	plane the plane
			@param	line the line
			@param	intersection_angle the resulting angle
			@return bool, true if an angle can be calculated, otherwise false
	*/
	template <typename T>
	BALL_INLINE 
	bool GetAngle(const TPlane3<T>& plane,const TLine3<T>& line, TAngle<T>& intersection_angle)
	{
		T length_product = plane.n.getSquareLength() * line.d.getSquareLength();
		
		if (Maths::isZero(length_product))
		{
			return false;
		} 

		intersection_angle = asin(Maths::abs(plane.n * line.d) / sqrt(length_product));
		return true;
	}

	/**	Get the angle between a line and a plane.
			@param	line the line
			@param	plane the plane
			@param	intersection_angle the resulting angle
			@return bool, true if an angle can be calculated, otherwise false
	*/
	template <typename T>
	BALL_INLINE 
	bool GetAngle(const TLine3<T>& line, const TPlane3<T>& plane, TAngle<T>& intersection_angle)
	{
		return GetAngle(plane, line, intersection_angle);
	}


	/**	Get the angle between two planes.
			@param	a the first plane
			@param	b the second plane
			@param	intersection_angle the resulting angle
			@return bool, true if an angle can be calculated, otherwise false
	*/
	template <typename T>
	BALL_INLINE 
	bool GetAngle(const TPlane3<T>& a, const TPlane3<T>& b, TAngle<T>& intersection_angle)
	{
		T length_product = a.n.getSquareLength() * b.n.getSquareLength();

		if(Maths::isZero(length_product))
		{
			return false;
		}

		intersection_angle = acos(Maths::abs(a.n * b.n) / sqrt(length_product));
		return true;
	}

	/**	Get the intersection point between two lines.
			@param	a the first line
			@param	b the second line
			@param	point the resulting intersection
			@return bool, true if an intersection can be calculated, otherwise false
	*/
	template <typename T>
	bool GetIntersection(const TLine3<T>& a, const TLine3<T>& b, TVector3<T>& point)
	{
		T c1, c2;
		if ((SolveSystem2(a.d.x, -b.d.x, b.p.x - a.p.x, a.d.y,  -b.d.y, b.p.y - a.p.y, c1, c2) == true && Maths::isEqual(a.p.z + a.d.z * c1, b.p.z + b.d.z * c2)) || (SolveSystem2(a.d.x, -b.d.x, b.p.x - a.p.x, a.d.z, -b.d.z, b.p.z - a.p.z, c1, c2) == true && Maths::isEqual(a.p.y + a.d.y * c1, b.p.y + b.d.y * c2)) || (SolveSystem2(a.d.y, -b.d.y, b.p.y - a.p.y, a.d.z, -b.d.z, b.p.z - a.p.z, c1, c2) == true && Maths::isEqual(a.p.x + a.d.x * c1, b.p.x + b.d.x * c2)))
		{
			point.set(a.p.x + a.d.x * c1, a.p.y + a.d.y * c1, a.p.z + a.d.z * c1);
			return true;
		} 

		return false;
	}

	/**	Get the intersection point between a plane and a line.
			@param	plane the plane
			@param	line the line
			@param	intersection_point the resulting intersection
			@return bool, true if an intersection can be calculated, otherwise false
	*/
	template <typename T>
	BALL_INLINE 
	bool GetIntersection(const TPlane3<T>& plane, const TLine3<T>& line, TVector3<T>& intersection_point)
	{
		T dot_product = plane.n * line.d;
		if (Maths::isZero(dot_product))
		{
			return false;
		} 
		intersection_point.set(line.p + (plane.n * (plane.p - line.p)) * line.d / dot_product);
		return true;
	}

	/**	Get the intersection point between a line and a plane.
			@param	line the line
			@param	plane the plane
			@param	intersection_point the resulting intersection
			@return bool, true if an intersection can be calculated, otherwise false
	*/
	template <typename T>
	BALL_INLINE 
	bool GetIntersection(const TLine3<T>& line, const TPlane3<T>& plane, TVector3<T>& intersection_point)
	{
		return GetIntersection(plane, line, intersection_point);
	}

	/**	Get the intersection line between two planes.
			@param	plane1 the first plane
			@param	plane2 the second plane
			@param	line the resulting intersection
			@return bool, true if an intersection can be calculated, otherwise false
	*/
	template <typename T>
	bool GetIntersection(const TPlane3<T>& plane1, const TPlane3<T>& plane2, TLine3<T>& line)
	{
		T u = plane1.p*plane1.n;
		T v = plane2.p*plane2.n;
		T det = plane1.n.x*plane2.n.y-plane1.n.y*plane2.n.x;
		if (Maths::isZero(det))
		{
			det = plane1.n.x*plane2.n.z-plane1.n.z*plane2.n.x;
			if (Maths::isZero(det))
			{
				det = plane1.n.y*plane2.n.z-plane1.n.z*plane2.n.y;
				if (Maths::isZero(det))
				{
					return false;
				}
				else
				{
					T a = plane2.n.z/det;
					T b = -plane1.n.z/det;
					T c = -plane2.n.y/det;
					T d = plane1.n.y/det;
					line.p.x = 0;
					line.p.y = a*u+b*v;
					line.p.z = c*u+d*v;
					line.d.x = -1;
					line.d.y = a*plane1.n.x+b*plane2.n.x;
					line.d.z = c*plane1.n.x+d*plane2.n.x;
				}
			}
			else
			{
				T a = plane2.n.z/det;
				T b = -plane1.n.z/det;
				T c = -plane2.n.x/det;
				T d = plane1.n.x/det;
				line.p.x = a*u+b*v;
				line.p.y = 0;
				line.p.z = c*u+d*v;
				line.d.x = a*plane1.n.y+b*plane2.n.y;
				line.d.y = -1;
				line.d.z = c*plane1.n.y+d*plane2.n.y;
			}
		}
		else
		{
			T a = plane2.n.y/det;
			T b = -plane1.n.y/det;
			T c = -plane2.n.x/det;
			T d = plane1.n.x/det;
			line.p.x = a*u+b*v;
			line.p.y = c*u+d*v;
			line.p.z = 0;
			line.d.x = a*plane1.n.z+b*plane2.n.z;
			line.d.y = c*plane1.n.z+d*plane2.n.z;
			line.d.z = -1;
		}
		return true;
	}

	/**	Get the intersection point between a sphere and a line.
			@param	sphere the sphere
			@param	line the line
			@param	intersection_point1 the first intersection point
			@param	intersection_point2 the second intersection point
			@return bool, true if an intersection can be calculated, otherwise false
	*/
	template <typename T>
	bool GetIntersection(const TSphere3<T>& sphere, const TLine3<T>& line, TVector3<T>& intersection_point1, TVector3<T>& intersection_point2)
	{
		T x1, x2;
		short number_of_solutions = SolveQuadraticEquation (line.d * line.d, (line.p - sphere.p) * line.d * 2, (line.p - sphere.p) * (line.p - sphere.p) - sphere.radius * sphere.radius, x1, x2);

		if (number_of_solutions == 0)
		{
			return false;
		}

		intersection_point1 = line.p + x1 * line.d;
		intersection_point2 = line.p + x2 * line.d;

		return true;
	}

	/**	Get the intersection point between a line and a sphere.
			@param	line the line
			@param	sphere the sphere
			@param	intersection_point1 the first intersection point
			@param	intersection_point2 the second intersection point
			@return bool, true if an intersection can be calculated, otherwise false
	*/
	template <typename T>
	BALL_INLINE 
	bool GetIntersection(const TLine3<T>& line, const TSphere3<T>& sphere, TVector3<T>& intersection_point1, TVector3<T>& intersection_point2)
	{
		return GetIntersection(sphere, line, intersection_point1, intersection_point2);
	}

	/**	Get the intersection circle between a sphere and a plane.
			@param	sphere the sphere
			@param	plane the plane
			@param	intersection_circle the intersection circle
			@return bool, true if an intersection can be calculated, otherwise false
	*/
	template <typename T>
	bool GetIntersection(const TSphere3<T>& sphere, const TPlane3<T>& plane, TCircle3<T>& intersection_circle)
	{
		T distance = GetDistance(sphere.p, plane);

		if (Maths::isGreater(distance, sphere.radius))
		{
			return false;
		}

		TVector3<T> Vector3(plane.n);
		Vector3.normalize();

		if (Maths::isEqual(distance, sphere.radius))
		{
			intersection_circle.set(sphere.p + sphere.radius * Vector3, plane.n, 0);
		} 
		else 
		{
			intersection_circle.set
				(sphere.p + distance * Vector3, plane.n,
				 sqrt(sphere.radius * sphere.radius - distance * distance));
		}

		return true;
	}

	/**	Get the intersection circle between a plane and a sphere.
			@param	plane the plane
			@param	sphere the sphere
			@param	intersection_circle the intersection circle
			@return bool, true if an intersection can be calculated, otherwise false
	*/
	template <typename T>
	BALL_INLINE bool
	GetIntersection(const TPlane3<T>& plane, const TSphere3<T>& sphere, TCircle3<T>& intersection_circle)
	{
		return GetIntersection(sphere, plane, intersection_circle);
	}
	 
	/**	Get the intersection circle between two spheres.
			This methods returns <b>false</b>, if the two spheres
			are identical, since then no intersection circle exists.
			@param	a the first sphere
			@param	b the second sphere
			@param	intersection_circle the intersection circle
			@return bool, <b>true</b> if an intersection can be calculated, otherwise <b>false</b>
	*/
	template <typename T>
	bool GetIntersection(const TSphere3<T>& a, const TSphere3<T>& b, TCircle3<T>& intersection_circle)
	{
		TVector3<T> norm = b.p - a.p;
		T square_dist = norm * norm;
		if (Maths::isZero(square_dist))
		{
			return false;
		}
		T dist = sqrt(square_dist);
		if (Maths::isLess(a.radius + b.radius, dist))
		{
			return false;
		}
		if (Maths::isGreaterOrEqual(Maths::abs(a.radius - b.radius), dist))
		{
			return false;
		}

		T radius1_square = a.radius * a.radius;
		T radius2_square = b.radius * b.radius;
		T u = radius1_square - radius2_square + square_dist;
		T length = u / (2 * square_dist);
		T square_radius = radius1_square - u * length / 2;
		if (square_radius < 0)
		{
			return false;
		}

		intersection_circle.p = a.p + (norm * length);
		intersection_circle.radius = sqrt(square_radius);
		intersection_circle.n = norm / dist;

		return true;
	}

	/**	Get the intersection points between three spheres.
			@param	s1 the first sphere
			@param	s2 the second sphere
			@param	s3 the third sphere
			@param	p1 the first intersection point
			@param	p2 the second intersection point
			@param	test
			@return bool, <b>true</b> if an intersection can be calculated, otherwise <b>false</b>
	*/
	template <class T>
	bool GetIntersection(const TSphere3<T>& s1, const TSphere3<T>& s2, const TSphere3<T>& s3, TVector3<T>& p1, TVector3<T>& p2, bool test = true)
	{
		T r1_square = s1.radius*s1.radius;
		T r2_square = s2.radius*s2.radius;
		T r3_square = s3.radius*s3.radius;
		T p1_square_length = s1.p*s1.p;
		T p2_square_length = s2.p*s2.p;
		T p3_square_length = s3.p*s3.p;
		T u = (r2_square-r1_square-p2_square_length+p1_square_length)/2;
		T v = (r3_square-r1_square-p3_square_length+p1_square_length)/2;
		TPlane3<T> plane1;
		TPlane3<T> plane2;
		try
		{
			plane1 = TPlane3<T>(s2.p.x-s1.p.x,s2.p.y-s1.p.y,s2.p.z-s1.p.z,u);
			plane2 = TPlane3<T>(s3.p.x-s1.p.x,s3.p.y-s1.p.y,s3.p.z-s1.p.z,v);
		}
		catch (Exception::DivisionByZero&)
		{
			return false;
		}
		TLine3<T> line;
		if (GetIntersection(plane1,plane2,line))
		{
			TVector3<T> diff(s1.p-line.p);
			T x1, x2;
			if (SolveQuadraticEquation(line.d*line.d, -diff*line.d*2, diff*diff-r1_square, x1,x2) > 0)
			{
				p1 = line.p+x1*line.d;
				p2 = line.p+x2*line.d;
				if (test)
				{
					TVector3<T> test = s1.p-p1;
					if (Maths::isNotEqual(test*test,r1_square))
					{
						return false;
					}
					test = s1.p-p2;
					if (Maths::isNotEqual(test*test,r1_square))
					{
						return false;
					}
					test = s2.p-p1;
					if (Maths::isNotEqual(test*test,r2_square))
					{
						return false;
					}
					test = s2.p-p2;
					if (Maths::isNotEqual(test*test,r2_square))
					{
						return false;
					}
					test = s3.p-p1;
					if (Maths::isNotEqual(test*test,r3_square))
					{
						return false;
					}
					test = s3.p-p2;
					if (Maths::isNotEqual(test*test,r3_square))
					{
						return false;
					}
				}
				return true;
			}
		}
		return false;
	}


	/**	Test whether two vector3 are collinear
			@param	a the first vector3
			@param	b the second vector3
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isCollinear(const TVector3<T>& a, const TVector3<T>& b)
	{
		return (a % b).isZero();
	}

	/**	Test whether three vector3 are complanar
			@param	a the first vector3
			@param	b the second vector3
			@param	c the third vector3
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isComplanar(const TVector3<T>& a, const TVector3<T>& b, const TVector3<T>& c)
	{
		return Maths::isZero(TVector3<T>::getTripleProduct(a, b, c));
	}

	/**	Test whether four vector3 are complanar
			@param	a the first vector3
			@param	b the second vector3
			@param	c the third vector3
			@param	d the fourth vector3
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isComplanar(const TVector3<T>& a, const TVector3<T>& b, const TVector3<T>& c, const TVector3<T>& d)
	{
		return isComplanar(a - b, a - c, a - d);
	}

	/**	Test whether two vector3 are orthogonal
			@param	a the first vector3
			@param	b the second vector3
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isOrthogonal(const TVector3<T>& a, const TVector3<T>& b)
	{
		return Maths::isZero(a * b);
	}

	/**	Test whether a vector3 and a line are orthogonal
			@param	vector the vector
			@param	line the line
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isOrthogonal(const TVector3<T>& vector, const TLine3<T>& line)
	{
		return Maths::isZero(vector * line.d);
	}

	/**	Test whether a line and a vector3 are orthogonal
			@param	line the line
			@param	vector the vector
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isOrthogonal(const TLine3<T>& line, const TVector3<T>& vector)
	{
		return isOrthogonal(vector, line);
	}

	/**	Test whether two lines are orthogonal.
			@param	a the first line
			@param	b the second line
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isOrthogonal(const TLine3<T>& a, const TLine3<T>& b)
	{
		return Maths::isZero(a.d * b.d);
	}

	/**	Test whether a vector3 and a plane are orthogonal.
			@param	vector the vector3
			@param	plane the plane
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isOrthogonal(const TVector3<T>& vector, const TPlane3<T>& plane)
	{
		return isCollinear(vector, plane.n);
	}

	/**	Test whether a plane and a vector3 are orthogonal.
			@param	plane the plane
			@param	vector the vector3
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isOrthogonal(const TPlane3<T>& plane, const TVector3<T>& vector)
	{
		return isOrthogonal(vector, plane);
	}

	/**	Test whether two planes are orthogonal.
			@param	a the first plane
			@param	b the second plane
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isOrthogonal(const TPlane3<T>& a, const TPlane3<T>& b)
	{
		return Maths::isZero(a.n * b.n);
	}

	/**	Test whether a line is intersecting a point.
			@param	point the point
			@param	line the line
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isIntersecting(const TVector3<T>& point, const TLine3<T>& line)
	{
		return Maths::isZero(GetDistance(point, line));
	}

	/**	Test whether a line is intersecting a point.
			@param	line the line
			@param	point the point
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isIntersecting(const TLine3<T>& line, const TVector3<T>& point)
	{
		return isIntersecting(point, line);
	}

	/**	Test whether two lines are intersecting.
			@param	a the first line
			@param	b the second line
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isIntersecting(const TLine3<T>& a, const TLine3<T>& b)
	{
		return Maths::isZero(GetDistance(a, b));
	}

	/**	Test whether a point lies in a plane.
			@param	point the point
			@param	plane the plane
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isIntersecting(const TVector3<T>& point, const TPlane3<T>& plane)
	{
		return Maths::isZero(GetDistance(point, plane));
	}

	/**	Test whether a point lies in a plane.
			@param	plane the plane
			@param	point the point
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isIntersecting(const TPlane3<T>& plane, const TVector3<T>& point)
	{
		return isIntersecting(point, plane);
	}

	/**	Test whether a line is intersecting a plane.
			@param	line the line
			@param	plane the plane
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isIntersecting(const TLine3<T>& line, const TPlane3<T>& plane)
	{
		return Maths::isZero(GetDistance(line, plane));
	}

	/**	Test whether a plane is intersecting a line.
			@param	plane the plane
			@param	line the line
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isIntersecting(const TPlane3<T>& plane, const TLine3<T>& line)
	{
		return isIntersecting(line, plane);
	}

	/**	Test whether two planes are intersecting.
			@param	a the first plane
			@param	b the second plane
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isIntersecting(const TPlane3<T>& a, const TPlane3<T>& b)
	{
		return Maths::isZero(GetDistance(a, b));
	}

	/**	Test whether a line and a plane are parallel.
			@param	line the line
			@param	plane the plane
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isParallel(const TLine3<T>& line, const TPlane3<T>& plane)
	{
		return isOrthogonal(line.d, plane.n);
	}

	/**	Test whether a plane and a line are parallel.
			@param	plane the plane
			@param	line the line
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isParallel(const TPlane3<T>& plane, const TLine3<T>& line)
	{
		return isParallel(line, plane);
	}

	/**	Test whether two planes are parallel.
			@param	a the first plane
			@param	b the second plane
			@return bool, true or false
	*/
	template <typename T>
	BALL_INLINE 
	bool isParallel(const TPlane3<T>& a, const TPlane3<T>& b)
	{
		return isCollinear(a.n, b.n);
	}

	/**	Return the oriented angle of two vectors with a normal vector.
	 *  @throw Exception::DivisionByZero if at least one vector is zero
	*/
	template <typename T>
	TAngle<T> getOrientedAngle
		(const T& ax, const T& ay, const T& az,
		 const T& bx, const T& by, const T& bz,
		 const T& nx, const T& ny, const T& nz)
	{
    // Calculate the length of the two normals
    T bl = (T) sqrt((double)ax * ax + ay * ay + az * az);
    T el = (T) sqrt((double)bx * bx + by * by + bz * bz);
    T bel = (T) (ax * bx + ay * by + az * bz);

    // if one or both planes are degenerated
    if (bl * el == 0)
    {
      throw Exception::DivisionByZero(__FILE__, __LINE__);
		}
    bel /= (bl * el);
    if (bel > 1.0)
    {
      bel = 1;
		}
    else if (bel < -1.0)
    {
      bel = -1;
		}

    T acosbel = (T) acos(bel);	// >= 0

    if (( nx * (az * by - ay * bz)
				 + ny * (ax * bz - az * bx)
				 + nz * (ay * bx - ax * by)) > 0)
    {
    	acosbel = Constants::PI+Constants::PI-acosbel;
		}

		return TAngle<T>(acosbel);
	}

	/**	Return the oriented angle of two vectors with a normal vector.
	 *  @throw Exception::DivisionByZero if at least one vector is zero
	*/
  template <typename T>
	BALL_INLINE 
  TAngle<T>getOrientedAngle(const TVector3<T>& a, const TVector3<T>& b, const TVector3<T>& normal)
  {
    return getOrientedAngle(a.x, a.y, a.z, b.x, b.y, b.z, normal.x, normal.y, normal.z);
	}
 
	/**	Return the torsion angle of four points to each other.
			@param  ax 1. vector x component
			@param  ay 1. vector y component
			@param  az 1. vector z component
			@param  bx 2. vector x component
			@param  by 2. vector y component
			@param  bz 2. vector z component
			@param  cx 3. vector x component
			@param  cy 3. vector y component
			@param  cz 3. vector z component
			@param  dx 4. vector x component
			@param  dy 4. vector y component
			@param  dz 4. vector z component
			@return TAngle the torsion angle
			@throw  Exception::DivisionByZero if one of the outer vectors is collinear with the middle one
	*/
	template <typename T>
	TAngle<T> getTorsionAngle
		(const T& ax, const T& ay, const T& az,
		 const T& bx, const T& by, const T& bz,
		 const T& cx, const T& cy, const T& cz, 
		 const T& dx, const T& dy, const T& dz)
	{
		T abx = ax - bx;
		T aby = ay - by;
		T abz = az - bz;

		T cbx = cx - bx;
		T cby = cy - by;
		T cbz = cz - bz;

		T cdx = cx - dx;
		T cdy = cy - dy;
		T cdz = cz - dz;

		// Calculate the normals to the two planes n1 and n2
		// this is given as the cross products:
		//		 AB x BC
		//		--------- = n1
		//		|AB x BC|
		//
		//		 BC x CD
		// 	  --------- = n2
		// 	  |BC x CD|

		// Normal to plane 1 
		T ndax = aby * cbz - abz * cby; 
		T nday = abz * cbx - abx * cbz;
		T ndaz = abx * cby - aby * cbx;

		// Normal to plane 2 
		T neax = cbz * cdy - cby * cdz; 
		T neay = cbx * cdz - cbz * cdx;
		T neaz = cby * cdx - cbx * cdy;

		// Calculate the length of the two normals 
		T bl = (T) sqrt((double)ndax * ndax + nday * nday + ndaz * ndaz);
		T el = (T) sqrt((double)neax * neax + neay * neay + neaz * neaz);
		T bel = (T) (ndax * neax + nday * neay + ndaz * neaz);
		
		// if one or both planes are degenerated
		if (bl * el == 0)
		{
			throw Exception::DivisionByZero(__FILE__, __LINE__);
		}
		bel /= (bl * el);
		if (bel > 1.0) 
		{
			bel = 1;
		} 
		else if (bel < -1.0) 
		{
			bel = -1;
		}

		T acosbel = (T) acos(bel);

		if ((cbx * (ndaz * neay - nday * neaz) 
				 + cby * (ndax * neaz - ndaz * neax) 
				 + cbz * (nday * neax - ndax * neay))
				< 0)
		{
			acosbel = -acosbel;
		}
		
		acosbel = (acosbel > 0.0) 
			? Constants::PI - acosbel 
			: -(Constants::PI + acosbel);
		
		return TAngle<T>(acosbel);
	}
	//@}
} // namespace BALL


#endif // BALL_MATHS_ANALYTICALGEOMETRY_H
