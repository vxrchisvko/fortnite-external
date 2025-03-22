#include <corecrt_math_defines.h>
#include <random>
#include <d3d9types.h>
#include <xmmintrin.h>
#include <immintrin.h>

class fvector2d
{
public:
	fvector2d() : x(0.f), y(0.f)
	{

	}

	fvector2d(double _x, double _y) : x(_x), y(_y)
	{

	}
	~fvector2d()
	{

	}

	fvector2d operator-(fvector2d v)
	{
		return fvector2d(x - v.x, y - v.y);
	}

	double x;
	double y;
};

class fvector
{
public:
	fvector() : x(0.f), y(0.f), z(0.f)
	{
	}

	fvector(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{
	}

	~fvector()
	{
	}

	double x;
	double y;
	double z;

	inline double dot(fvector v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline double distance(fvector v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	inline double length()
	{
		return sqrt(x * x + y * y + z * z);
	}

	fvector operator+(fvector v)
	{
		return fvector(x + v.x, y + v.y, z + v.z);
	}

	fvector operator-(fvector v)
	{
		return fvector(x - v.x, y - v.y, z - v.z);
	}

	fvector operator*(double flNum)
	{
		return fvector(x * flNum, y * flNum, z * flNum);
	}

	fvector operator/(double flNum)
	{
		return fvector(x / flNum, y / flNum, z / flNum);
	}

	fvector& operator+=(const fvector& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	fvector& operator-=(const fvector& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
};

struct fquat
{
	double x;
	double y;
	double z;
	double w;
};
struct frotator
{
	double Pitch;
	double Yaw;
	double Roll;
};

struct ftransform {
	fquat rotation;
	fvector translation;
	uint8_t pad[8];
	fvector scale;
	uint8_t pad2[8];

	D3DMATRIX to_matrix_with_scale() const {
		fvector scale_3d
		(
			(scale.x == 0.0) ? 1.0 : scale.x,
			(scale.y == 0.0) ? 1.0 : scale.y,
			(scale.z == 0.0) ? 1.0 : scale.z
		);

		double x2 = rotation.x + rotation.x;
		double y2 = rotation.y + rotation.y;
		double z2 = rotation.z + rotation.z;
		double xx2 = rotation.x * x2;
		double yy2 = rotation.y * y2;
		double zz2 = rotation.z * z2;
		double yz2 = rotation.y * z2;
		double wx2 = rotation.w * x2;
		double xy2 = rotation.x * y2;
		double wz2 = rotation.w * z2;
		double xz2 = rotation.x * z2;
		double wy2 = rotation.w * y2;

		D3DMATRIX m;

		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		m._11 = (1.0 - (yy2 + zz2)) * scale_3d.x;
		m._22 = (1.0 - (xx2 + zz2)) * scale_3d.y;
		m._33 = (1.0 - (xx2 + yy2)) * scale_3d.z;

		m._32 = (yz2 - wx2) * scale_3d.z;
		m._23 = (yz2 + wx2) * scale_3d.y;

		m._21 = (xy2 - wz2) * scale_3d.y;
		m._12 = (xy2 + wz2) * scale_3d.x;

		m._31 = (xz2 + wy2) * scale_3d.z;
		m._13 = (xz2 - wy2) * scale_3d.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

inline D3DMATRIX matrix_multiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX p_out;
	p_out._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	p_out._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	p_out._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	p_out._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	p_out._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	p_out._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	p_out._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	p_out._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	p_out._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	p_out._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	p_out._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	p_out._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	p_out._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	p_out._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	p_out._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	p_out._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return p_out;
}

#define m_pi 3.14159265358979323846f

struct _matrix {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
};

inline _matrix matrix(fvector Vec4, fvector origin = fvector(0, 0, 0))
{
	double rad_pitch = ( Vec4.x * double( m_pi ) / 180.f );
	double rad_yaw = ( Vec4.y * double( m_pi ) / 180.f );
	double rad_roll = ( Vec4.z * double( m_pi ) / 180.f );

	double sp = sinf( rad_pitch );
	double cp = cosf( rad_pitch );
	double sy = sinf( rad_yaw );
	double cy = cosf( rad_yaw );
	double sr = sinf( rad_roll );
	double cr = cosf( rad_roll );

	_matrix matrix;
	matrix.m[0][0] = cp * cy;
	matrix.m[0][1] = cp * sy;
	matrix.m[0][2] = sp;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = sr * sp * cy - cr * sy;
	matrix.m[1][1] = sr * sp * sy + cr * cy;
	matrix.m[1][2] = -sr * cp;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -( cr * sp * cy + sr * sy );
	matrix.m[2][1] = cy * sr - cr * sp * sy;
	matrix.m[2][2] = cr * cp;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

static float powf_( float _X, float _Y )
{
	return ( _mm_cvtss_f32(_mm_pow_ps( _mm_set_ss( _X ), _mm_set_ss( _Y ) ) ) );
}

static float sqrtf_( float _X ) {
	return ( _mm_cvtss_f32(_mm_sqrt_ps( _mm_set_ss( _X ) ) ) );
}

static double get_cross_distance( double x1, double y1, double x2, double y2 ) {
	return sqrtf( powf( ( x2 - x1 ), 2 ) + powf_( ( y2 - y1 ), 2 ) );
}

template<class T>
class tarray
{
public:
	tarray( ) : tData( ), iCount( ), iMaxCount( ) {}
	tarray( T* data, int count, int max_count ) :
		tData( data ), iCount( count ), iMaxCount( max_count ) {
	}

	auto Get( int idx ) -> T
	{
		return driver.read< T >(reinterpret_cast<__int64>( this->tData ) + ( idx * sizeof( T ) ) );
	}

	auto Size( ) -> std::uint32_t
	{
		return this->iCount;
	}

	bool IsValid( )
	{
		return this->iCount != 0;
	}

	T* tData;
	int iCount;
	int iMaxCount;
};

struct f_plane : public fvector
{
	double w;
};

struct f_matrix
{
	double m[4][4];


	f_plane xplane, yplane, zplane, wplane;

	f_matrix MatrixMultiplication(f_matrix other)
	{
		f_matrix copy = {};

		copy.wplane.x = this->wplane.x * other.xplane.x + this->wplane.y * other.yplane.x + this->wplane.z * other.zplane.x + this->wplane.w * other.wplane.x;
		copy.wplane.y = this->wplane.x * other.xplane.y + this->wplane.y * other.yplane.y + this->wplane.z * other.zplane.y + this->wplane.w * other.wplane.y;
		copy.wplane.z = this->wplane.x * other.xplane.z + this->wplane.y * other.yplane.z + this->wplane.z * other.zplane.z + this->wplane.w * other.wplane.z;
		copy.wplane.w = this->wplane.x * other.xplane.w + this->wplane.y * other.yplane.w + this->wplane.z * other.zplane.w + this->wplane.w * other.wplane.w;

		return copy;
	}

	static f_matrix RotatorToMatrix(fvector rotation)
	{
		auto return_value = f_matrix();

		auto pitch = (rotation.x * 3.14159265358979323846 / 180);
		auto yaw = (rotation.y * 3.14159265358979323846 / 180);
		auto roll = (rotation.z * 3.14159265358979323846 / 180);

		auto sp = sin(pitch);
		auto cp = cos(pitch);
		auto sy = sin(yaw);
		auto cy = cos(yaw);
		auto sr = sin(roll);
		auto cr = cos(roll);

		return_value.xplane.x = cp * cy;
		return_value.xplane.y = cp * sy;
		return_value.xplane.z = sp;

		return_value.yplane.x = sr * sp * cy - cr * sy;
		return_value.yplane.y = sr * sp * sy + cr * cy;
		return_value.yplane.z = -sr * cp;

		return_value.zplane.x = -(cr * sp * cy + sr * sy);
		return_value.zplane.y = cy * sr - cr * sp * sy;
		return_value.zplane.z = cr * cp;

		return_value.wplane.w = 0x1;

		return return_value;
	}
};