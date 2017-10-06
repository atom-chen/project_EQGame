/******************************************************************************
* 作者: Y3
* 时间: 2010-09-15
* 描述: 二维矢量, 拷贝ogre
******************************************************************************/
#ifndef SSystem_Vector2f_H
#define SSystem_Vector2f_H

#include "SSystemCommon.h"
#include "math.h"
#include <limits>

namespace SearchSystem
{
	class Vector2f
	{
	public:
		float x, y;

	public:
		inline Vector2f()
		{
		}

		inline Vector2f(const float fX, const float fY )
			: x( fX ), y( fY )
		{
		}

		inline explicit Vector2f( const float scaler )
			: x( scaler), y( scaler )
		{
		}

		inline explicit Vector2f( const float afCoordinate[2] )
			: x( afCoordinate[0] ),
			y( afCoordinate[1] )
		{
		}

		inline explicit Vector2f( const int afCoordinate[2] )
		{
			x = (float)afCoordinate[0];
			y = (float)afCoordinate[1];
		}

		inline explicit Vector2f( float* const r )
			: x( r[0] ), y( r[1] )
		{
		}

		inline float operator [] ( const size_t i ) const
		{
			assert( i < 2 );

			return *(&x+i);
		}

		inline float& operator [] ( const size_t i )
		{
			assert( i < 2 );

			return *(&x+i);
		}

		/// Pointer accessor for direct copying
		inline float* ptr()
		{
			return &x;
		}
		/// Pointer accessor for direct copying
		inline const float* ptr() const
		{
			return &x;
		}

		/** Assigns the value of the other vector.
		@param
		rkVector The other vector
		*/
		inline Vector2f& operator = ( const Vector2f& rkVector )
		{
			x = rkVector.x;
			y = rkVector.y;

			return *this;
		}

		inline Vector2f& operator = ( const float fScalar)
		{
			x = fScalar;
			y = fScalar;

			return *this;
		}

		inline bool operator == ( const Vector2f& rkVector ) const
		{
			return ( x == rkVector.x && y == rkVector.y );
		}

		inline bool operator != ( const Vector2f& rkVector ) const
		{
			return ( x != rkVector.x || y != rkVector.y  );
		}

		// arithmetic operations
		inline Vector2f operator + ( const Vector2f& rkVector ) const
		{
			return Vector2f(
				x + rkVector.x,
				y + rkVector.y);
		}

		inline Vector2f operator - ( const Vector2f& rkVector ) const
		{
			return Vector2f(
				x - rkVector.x,
				y - rkVector.y);
		}

		inline Vector2f operator * ( const float fScalar ) const
		{
			return Vector2f(
				x * fScalar,
				y * fScalar);
		}

		inline Vector2f operator * ( const Vector2f& rhs) const
		{
			return Vector2f(
				x * rhs.x,
				y * rhs.y);
		}

		inline Vector2f operator / ( const float fScalar ) const
		{
			assert( fScalar != 0.0 );

			float fInv = 1.0f / fScalar;

			return Vector2f(
				x * fInv,
				y * fInv);
		}

		inline Vector2f operator / ( const Vector2f& rhs) const
		{
			return Vector2f(
				x / rhs.x,
				y / rhs.y);
		}

		inline const Vector2f& operator + () const
		{
			return *this;
		}

		inline Vector2f operator - () const
		{
			return Vector2f(-x, -y);
		}

		// overloaded operators to help Vector2f
		inline friend Vector2f operator * ( const float fScalar, const Vector2f& rkVector )
		{
			return Vector2f(
				fScalar * rkVector.x,
				fScalar * rkVector.y);
		}

		inline friend Vector2f operator / ( const float fScalar, const Vector2f& rkVector )
		{
			return Vector2f(
				fScalar / rkVector.x,
				fScalar / rkVector.y);
		}

		inline friend Vector2f operator + (const Vector2f& lhs, const float rhs)
		{
			return Vector2f(
				lhs.x + rhs,
				lhs.y + rhs);
		}

		inline friend Vector2f operator + (const float lhs, const Vector2f& rhs)
		{
			return Vector2f(
				lhs + rhs.x,
				lhs + rhs.y);
		}

		inline friend Vector2f operator - (const Vector2f& lhs, const float rhs)
		{
			return Vector2f(
				lhs.x - rhs,
				lhs.y - rhs);
		}

		inline friend Vector2f operator - (const float lhs, const Vector2f& rhs)
		{
			return Vector2f(
				lhs - rhs.x,
				lhs - rhs.y);
		}
		// arithmetic updates
		inline Vector2f& operator += ( const Vector2f& rkVector )
		{
			x += rkVector.x;
			y += rkVector.y;

			return *this;
		}

		inline Vector2f& operator += ( const float fScaler )
		{
			x += fScaler;
			y += fScaler;

			return *this;
		}

		inline Vector2f& operator -= ( const Vector2f& rkVector )
		{
			x -= rkVector.x;
			y -= rkVector.y;

			return *this;
		}

		inline Vector2f& operator -= ( const float fScaler )
		{
			x -= fScaler;
			y -= fScaler;

			return *this;
		}

		inline Vector2f& operator *= ( const float fScalar )
		{
			x *= fScalar;
			y *= fScalar;

			return *this;
		}

		inline Vector2f& operator *= ( const Vector2f& rkVector )
		{
			x *= rkVector.x;
			y *= rkVector.y;

			return *this;
		}

		inline Vector2f& operator /= ( const float fScalar )
		{
			assert( fScalar != 0.0 );

			float fInv = 1.0f / fScalar;

			x *= fInv;
			y *= fInv;

			return *this;
		}

		inline Vector2f& operator /= ( const Vector2f& rkVector )
		{
			x /= rkVector.x;
			y /= rkVector.y;

			return *this;
		}

		/** Returns the length (magnitude) of the vector.
		@warning
		This operation requires a square root and is expensive in
		terms of CPU operations. If you don't need to know the exact
		length (e.g. for just comparing lengths) use squaredLength()
		instead.
		*/
		inline float length () const
		{
			return sqrt( x * x + y * y );
		}

		/** Returns the square of the length(magnitude) of the vector.
		@remarks
		This  method is for efficiency - calculating the actual
		length of a vector requires a square root, which is expensive
		in terms of the operations required. This method returns the
		square of the length of the vector, i.e. the same as the
		length but before the square root is taken. Use this if you
		want to find the longest / shortest vector without incurring
		the square root.
		*/
		inline float squaredLength () const
		{
			return x * x + y * y;
		}

		/** Calculates the dot (scalar) product of this vector with another.
		@remarks
		The dot product can be used to calculate the angle between 2
		vectors. If both are unit vectors, the dot product is the
		cosine of the angle; otherwise the dot product must be
		divided by the product of the lengths of both vectors to get
		the cosine of the angle. This result can further be used to
		calculate the distance of a point from a plane.
		@param
		vec Vector with which to calculate the dot product (together
		with this one).
		@returns
		A float representing the dot product value.
		*/
		inline float dotProduct(const Vector2f& vec) const
		{
			return x * vec.x + y * vec.y;
		}

		/** Normalises the vector.
		@remarks
		This method normalises the vector such that it's
		length / magnitude is 1. The result is called a unit vector.
		@note
		This function will not crash for zero-sized vectors, but there
		will be no changes made to their components.
		@returns The previous length of the vector.
		*/
		inline float normalise()
		{
			float fLength =sqrt( x * x + y * y);

			// Will also work for zero-sized vectors, but will change nothing
			if ( fLength > 1e-08 )
			{
				float fInvLength = 1.0f / fLength;
				x *= fInvLength;
				y *= fInvLength;
			}

			return fLength;
		}



		/** Returns a vector at a point half way between this and the passed
		in vector.
		*/
		inline Vector2f midPoint( const Vector2f& vec ) const
		{
			return Vector2f(
				( x + vec.x ) * 0.5f,
				( y + vec.y ) * 0.5f );
		}

		/** Returns true if the vector's scalar components are all greater
		that the ones of the vector it is compared against.
		*/
		inline bool operator < ( const Vector2f& rhs ) const
		{
			if( x < rhs.x && y < rhs.y )
				return true;
			return false;
		}

		/** Returns true if the vector's scalar components are all smaller
		that the ones of the vector it is compared against.
		*/
		inline bool operator > ( const Vector2f& rhs ) const
		{
			if( x > rhs.x && y > rhs.y )
				return true;
			return false;
		}

		/** Sets this vector's components to the minimum of its own and the
		ones of the passed in vector.
		@remarks
		'Minimum' in this case means the combination of the lowest
		value of x, y and z from both vectors. Lowest is taken just
		numerically, not magnitude, so -1 < 0.
		*/
		inline void makeFloor( const Vector2f& cmp )
		{
			if( cmp.x < x ) x = cmp.x;
			if( cmp.y < y ) y = cmp.y;
		}

		/** Sets this vector's components to the maximum of its own and the
		ones of the passed in vector.
		@remarks
		'Maximum' in this case means the combination of the highest
		value of x, y and z from both vectors. Highest is taken just
		numerically, not magnitude, so 1 > -3.
		*/
		inline void makeCeil( const Vector2f& cmp )
		{
			if( cmp.x > x ) x = cmp.x;
			if( cmp.y > y ) y = cmp.y;
		}

		/** Generates a vector perpendicular to this vector (eg an 'up' vector).
		@remarks
		This method will return a vector which is perpendicular to this
		vector. There are an infinite number of possibilities but this
		method will guarantee to generate one of them. If you need more
		control you should use the Quaternion class.
		*/
		inline Vector2f perpendicular(void) const
		{
			return Vector2f (-y, x);
		}
		/** Calculates the 2 dimensional cross-product of 2 vectors, which results
		in a single floating point value which is 2 times the area of the triangle.
		*/
		inline float crossProduct( const Vector2f& rkVector ) const
		{
			return x * rkVector.y - y * rkVector.x;
		}

		/** Returns true if this vector is zero length. */
		inline bool isZeroLength(void) const
		{
			float sqlen = (x * x) + (y * y);
			return (sqlen < (1e-06 * 1e-06));

		}

		/** As normalise, except that this vector is unaffected and the
		normalised vector is returned as a copy. */
		inline Vector2f normalisedCopy(void) const
		{
			Vector2f ret = *this;
			ret.normalise();
			return ret;
		}

		/** Calculates a reflection vector to the plane with the given normal .
		@remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
		*/
		inline Vector2f reflect(const Vector2f& normal) const
		{
			return Vector2f( *this - ( 2 * this->dotProduct(normal) * normal ) );
		}

		inline bool equals(const Vector2f& v, float epsilon = std::numeric_limits<float>::epsilon()) const
		{
			if (fabs(x-v.x) <= epsilon && fabs(y-v.y) <= epsilon)
				return true;
			else
				return false;
		}


		/** Function for writing to a stream.
		*/
		inline friend std::ostream& operator <<
			( std::ostream& o, const Vector2f& v )
		{
			o << "Vector2f(" << v.x << ", " << v.y <<  ")";
			return o;
		}

	};
}

#endif