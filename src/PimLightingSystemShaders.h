#define PIM_LS_LOWQ_FRAG																 "\
uniform sampler2D tex;																	\n\
uniform float lalpha;																	\n\
uniform vec4 ulcolor;																	\n\
float length(vec4 v)																	\n\
{																						\n\
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);											\n\
}																						\n\
void main()																				\n\
{																						\n\
	vec4 src = texture2D(tex, gl_TexCoord[0].xy);										\n\
	src.a -= (length(src)-length(ulcolor))*lalpha*3.0;									\n\
	gl_FragColor = src;																	\n\
}"

#define PIM_LS_LOWQ_VERT																 "\
void main()																				\n\
{																						\n\
	gl_Position = ftransform();															\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;													\n\
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

#define PIM_LS_HIGHQ_VERT																 "\
void main() {																			\n\
	gl_Position = ftransform();															\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;													\n\
}"

#define PIM_LS_NORMALMAP_FRAG															 "\
uniform sampler2D 	tex0; 		// The sprite texture									\n\
uniform sampler2D 	tex1;		// The normal texture									\n\
uniform vec2 		dims;		// The dimension of the texture							\n\
uniform vec2 		wpos;		// The world position of the sprite						\n\
uniform vec2 		anchor;		// The anchor of the Sprite								\n\
uniform int		numLights;		// The amount of lights									\n\
uniform vec2 		lpos[10];	// The light positions									\n\
uniform float 		lrad[10];	// The light radius'									\n\
																						\n\
void main() {																			\n\
	vec4 finalColor = vec4(0.0, 0.0, 0.0, 0.0);											\n\
																						\n\
	/* Iterate over all the colors */													\n\
 	for (int i=0; i<numLights; i++) {													\n\
																						\n\
		/* Get the normal of the normal-map */											\n\
		vec4 color = texture2D(tex1, gl_TexCoord[0].xy);								\n\
		vec3 norm = normalize((color.xyz * 2.0) + vec3(-1.0));							\n\
																						\n\
		/* Find the relative position of the light */									\n\
		vec2 nlpos = lpos[i] - wpos + (anchor * dims) - gl_TexCoord[0].xy * dims;		\n\
																						\n\
		/* Set up a distance-factor */													\n\
		vec2 diff = nlpos - gl_TexCoord[0].xy;											\n\
		float len = length(diff);														\n\
		len /=  400.0;																	\n\
		if (len > 1.0) len = 1.0;														\n\
		if (len < 0.0) len = 0.0;														\n\
		len = (1.0 - len);																\n\
																						\n\
		/* Get the color of the image texture */										\n\
		color = texture2D(tex0, gl_TexCoord[0].xy);										\n\
																						\n\
		/* Calculate the final color */													\n\
		float fac = dot(norm, normalize(vec3(nlpos, 100.0)));							\n\
		fac *= len;																		\n\
		color.rgb *= fac;																\n\
		finalColor += color;															\n\
	}																					\n\
	gl_FragColor = finalColor;															\n\
}"