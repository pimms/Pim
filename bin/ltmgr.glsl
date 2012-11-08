// This perticular shader leaves some nasty artifacts, and should be rewritten
// or replaced entirely.

// FRAGMENT SHADER																		\n\
uniform float sigma;																	\n\
uniform float blurSize; 																\n\
uniform float lalpha;																	\n\
uniform vec4 ulcolor;																	\n\
uniform sampler2D blurSampler; 															\n\
																						\n\
const float pi = 3.14159265;															\n\
const float numBlurPixelsPerSide = 20.0;												\n\
const vec2  blurMultiplyVec      = vec2(0.0, 1.0);										\n\
																						\n\
float length(vec4 v)																	\n\
{																						\n\
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z );											\n\
}																						\n\
																						\n\
void main() {																			\n\
  vec3 incrementalGaussian;																\n\
  incrementalGaussian.x = 1.0 / (sqrt(2.0 * pi) * sigma);								\n\
  incrementalGaussian.y = exp(-0.5 / (sigma * sigma));									\n\
  incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;				\n\
  vec4 avgValue = vec4(0.0);															\n\
  float coefficientSum = 0.0;															\n\
  avgValue += texture2D(blurSampler, gl_TexCoord[0].xy) * incrementalGaussian.x;		\n\
  coefficientSum += incrementalGaussian.x;												\n\
  incrementalGaussian.xy *= incrementalGaussian.yz;										\n\
  for (int i=0; i<3; i++)																\n\
  for (float i = 1.0; i <= numBlurPixelsPerSide; i++) { 								\n\
	avgValue += texture2D(blurSampler, gl_TexCoord[0].xy - i * blurSize *  				\n\
					vec2(1.0,0.0)) * incrementalGaussian.x;         					\n\
	avgValue += texture2D(blurSampler, gl_TexCoord[0].xy + i * blurSize *  				\n\
					vec2(1.0,0.0)) * incrementalGaussian.x;         					\n\
	avgValue += texture2D(blurSampler, gl_TexCoord[0].xy - i * blurSize *  				\n\
					vec2(0.0,1.0)) * incrementalGaussian.x;         					\n\
	avgValue += texture2D(blurSampler, gl_TexCoord[0].xy + i * blurSize *  				\n\
					vec2(0.0,1.0)) * incrementalGaussian.x;         					\n\
	coefficientSum += 3.0 * incrementalGaussian.x;										\n\
	incrementalGaussian.xy *= incrementalGaussian.yz;									\n\
  }																						\n\
  vec4 color = avgValue / coefficientSum;												\n\
  color.a -= (length(color) - length(ulcolor)) * lalpha * 3.0;							\n\
  gl_FragColor = color;																	\n\
}

// VERTEX SHADER																			\n\
void main() {																				\n\
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;									\n\
    gl_TexCoord[0] = gl_MultiTexCoord0;														\n\
}





/*
		SHADER #2:
*/

// VERTEX SHADER 																		\n\
void main(void)																			\n\
{																						\n\
  gl_Position = ftransform();															\n\
  gl_TexCoord[0] = gl_MultiTexCoord0;													\n\
}																					

// FRAGMENT SHADER 																		\n\
#version120																				\n\
uniform sampler2D tex;																	\n\
uniform vec4 ulcolor;																	\n\
uniform float lalpha;																	\n\
																						\n\
uniform float rt_w; // render target width												\n\
uniform float rt_h; // render target height												\n\
																						\n\
float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );							\n\
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );					\n\
																						\n\
float length(vec4 v)																	\n\
{																						\n\
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z );											\n\
}																						\n\
																						\n\
void main() 																			\n\
{ 																						\n\
	vec4 tc = vec4(1.0, 0.0, 0.0, 0.0);													\n\
	vec2 uv = gl_TexCoord[0].xy;														\n\
	tc = texture2D(tex, uv) * weight[0];												\n\
																						\n\
	for (int i=1; i<3; i++) 															\n\
	{																					\n\
		tc += texture2D(tex, uv + vec2(0.0, offset[i])/rt_h)							\n\
		      * weight[i];																\n\
		tc += texture2D(tex, uv - vec2(0.0, offset[i])/rt_h)							\n\
		     * weight[i];																\n\
	}																					\n\
	tc.a -= (length(tc) - length(ulcolor)) * lalpha * 3.0;								\n\
	gl_FragColor = tc;																	\n\
}