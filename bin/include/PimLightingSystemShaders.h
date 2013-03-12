#define PIM_LS_LOWQ_FRAG	"\
uniform sampler2D tex;									\n\
uniform float lalpha;									\n\
uniform vec4 ulcolor;									\n\
float length(vec4 v)									\n\
{														\n\
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);			\n\
}														\n\
void main()												\n\
{														\n\
	vec4 src = texture2D(tex, gl_TexCoord[0].xy);		\n\
	src.a -= (length(src)-length(ulcolor))*lalpha*3.0;	\n\
	gl_FragColor = src;									\n\
}"

#define PIM_LS_LOWQ_VERT "\
void main()												\n\
{														\n\
	gl_Position = ftransform();							\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;					\n\
}"


// This shader is a modified version of Callum Hay's implementation:
// http://callumhay.blogspot.no/2010/09/gaussian-blur-shader-glsl.html	
#define PIM_LS_HIGHQ_FRAG "\
uniform float sigma;																	\n\
uniform float blurSize; 																\n\
uniform float lalpha;																	\n\
uniform float pixels;																	\n\
uniform vec4 ulcolor;																	\n\
uniform vec2 direction;																	\n\
uniform sampler2D blurSampler; 															\n\
																						\n\
const float pi = 3.14159265;															\n\
																						\n\
float length(vec4 v)																	\n\
{																						\n\
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z );											\n\
}																						\n\
																						\n\
void main() {																			\n\
	vec3 incrementalGaussian;															\n\
	incrementalGaussian.x = 1.0 / (sqrt(2.0 * pi) * sigma);								\n\
	incrementalGaussian.y = exp(-0.5 / (sigma * sigma));								\n\
	incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;				\n\
	vec4 avgValue = vec4(0.0);															\n\
	float coefficientSum = 0.0;															\n\
	avgValue += texture2D(blurSampler, gl_TexCoord[0].xy) * incrementalGaussian.x;		\n\
	coefficientSum += incrementalGaussian.x;											\n\
	incrementalGaussian.xy *= incrementalGaussian.yz;									\n\
	for (float i = 1.0; i <= pixels; i++) { 											\n\
	avgValue += texture2D(blurSampler, gl_TexCoord[0].xy - i * blurSize *  				\n\
					direction) * incrementalGaussian.x;         						\n\
	avgValue += texture2D(blurSampler, gl_TexCoord[0].xy + i * blurSize *  				\n\
					direction) * incrementalGaussian.x;         						\n\
	coefficientSum += 2.0 * incrementalGaussian.x;										\n\
	incrementalGaussian.xy *= incrementalGaussian.yz;									\n\
	}																					\n\
	vec4 color = avgValue / coefficientSum;												\n\
	color.a -= (length(color) - length(ulcolor)) * lalpha * 2.0;						\n\
	gl_FragColor = color;																\n\
}"

#define PIM_LS_HIGHQ_VERT "\
void main() {																			\n\
	gl_Position = ftransform();															\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;													\n\
}"