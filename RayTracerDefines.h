#ifndef __RAY_TRACER_DEFINES_H__
#define __RAY_TRACER_DEFINES_H__

struct material
{
	public:
		material()
		{}

		material( rgb<float> const& color, float const& diffuse, float const& specular, float const& shine, float const& transmittence, float const& refraction ) 
			: _color(color), _diffuse(diffuse), _specular(specular), _shine(shine), _transmittence(transmittence), _refraction(refraction)
		{}

		material( float const& red, float const& green, float const& blue, float const& diffuse, float const& specular, float const& shine, float const& transmittence, float const& refraction ) 
			: _color( rgb<float>(red, green, blue) ), _diffuse(diffuse), _specular(specular), _shine(shine), _transmittence(transmittence), _refraction(refraction)
		{} 

	private:
		rgb<float>	_color;
		float		_diffuse;
		float		_specular;
		float		_shine;
		float		_transmittence;
		float		_refraction;
};

#endif
