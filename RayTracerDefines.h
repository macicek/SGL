#ifndef __RAY_TRACER_DEFINES_H__
#define __RAY_TRACER_DEFINES_H__

const float EPSILON = 1e-1f;
const uint32 MAX_RAY_DEPTH = 8;
const uint32 AREA_LIGHT_SAMPLES = 16;

const rgb WHITE( 1.0f, 1.0f, 1.0f );
const rgb BLACK( 0.0f, 0.0f, 0.0f );
const rgb RED( 1.0f, 0.0f, 0.0f );
const rgb GREEN( 0.0f, 1.0f, 0.0f );
const rgb BLUE( 0.0f, 0.0f, 1.0f );

// #define _TRIANGLE_CULLING

struct material
{
	public:
		material()
		{}
		material( material const& m )
			: _color(m.color()), _diffuse(m.diffuse()), _specular(m.specular()), _shine(m.shine()), _transmittence(m.transmittence()), _refraction(m.refraction())
		{}
		material( rgb const& color, float const& diffuse, float const& specular, float const& shine, float const& transmittence, float const& refraction ) 
			: _color(color), _diffuse(diffuse), _specular(specular), _shine(shine), _transmittence(transmittence), _refraction(refraction)
		{}
		material( float const& red, float const& green, float const& blue, float const& diffuse, float const& specular, float const& shine, float const& transmittence, float const& refraction ) 
			: _color( rgb(red, green, blue) ), _diffuse(diffuse), _specular(specular), _shine(shine), _transmittence(transmittence), _refraction(refraction)
		{} 

		rgb			color() const { return _color; }
		float		diffuse() const { return _diffuse; }
		float		specular() const { return _specular; }
		float		shine() const { return _shine; }
		float		transmittence() const { return _transmittence; }
		float		refraction() const { return _refraction; }

	private:
		rgb			_color;
		float		_diffuse;
		float		_specular;
		float		_shine;
		float		_transmittence;
		float		_refraction;
};

struct emissiveMaterial
{
	public: 
		emissiveMaterial()
		{ }

		emissiveMaterial( rgb const& color, float c0, float c1, float c2 )
			: _color(color), _c0(c0), _c1(c1), _c2(c2)
		{ }

		rgb color() const
		{ return _color; }

		float c0() const
		{ return _c0; }

		float c1() const
		{ return _c1; }

		float c2() const
		{ return _c2; }

	private:
		rgb _color;
		float _c0, _c1, _c2;
};

#endif
