uniform float dummy;
uniform vec2 dim;
uniform vec2 fudge;
varying float debug;

void main(void)
{
	mat4 tTex = gl_TextureMatrix[0];
	gl_Position = ftransform();
	vec4 object = gl_Vertex;
	vec4 texCd = object * tTex;
	texCd.w /= 1.;
	debug = 1.;
	
	texCd.xy /= dim / 2.;
	texCd.y *= 2.0;
//	texCd.xy += fudge;
	texCd.xy += vec2(0.0, .22);
	texCd.xy += 1.; 
	texCd.xy /= 2.;
	texCd.z = 0.;
	texCd.xy *= dim;
	gl_TexCoord[0] = texCd;
}