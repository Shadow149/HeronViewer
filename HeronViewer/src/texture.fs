#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

in vec2 texOffsets[9];

// texture sampler
uniform sampler2D texture1;
uniform float bright;

uniform float[4] high;
uniform float[4] mid;
uniform float[4] low;
uniform float[4] exp;
uniform float[4] contrast;
uniform float[4] whites;

uniform bool clip;
uniform bool b4;
uniform bool bw;

uniform float sat;
uniform float wb;

uniform float[25] sharp_kernel;
uniform float[9] sharp_kernel33;

uniform float high_thresh;
uniform float shad_thresh;
uniform float high_incr;
uniform float shad_incr;

uniform float shad_var;
uniform float var_mult;

vec3 kelvin_table[19] = {
    vec3(255,56,0),
    vec3(255,109,0),
    vec3(255,137,18),
    vec3(255,161,72),
    vec3(255,180,107),
    vec3(255,196,137),
    vec3(255,209,163),
    vec3(255,219,186),
    vec3(255,228,206),
    vec3(255,236,224),
    vec3(255,243,239),
    vec3(255,249,253),
    vec3(245,243,255),
    vec3(235,238,255),
    vec3(227,233,255),
    vec3(220,229,255),
    vec3(214,225,255),
    vec3(208,222,255),
    vec3(204,219,255)
};


/*
vec4 edit(vec4 v) {
	vec4 result = vec4(0);
	float t = exp * ((v[0] + v[1] + v[2])/3);
	float val = (4 * pow((1-t),3) * t * (0.25 + low - contrast)) + (6 * pow((1-t),2) * pow(t,2) * (0.5 + mid)) + (4 * (1-t) * pow(t,3) * (0.75 + high + contrast)) + (pow(t,4) * 1);
	result = vec4(vec3(val),1);

	if (clip) {
		if (val <= 0)
			result = vec4(1.0, 1.0, 0.0, 1);
		else if (val >= 1.0)
			result = vec4(1.0, 0, 1.0, 1);
	}
	return result;
}
*/

float edit_channel(float v, int chr) {
	vec4 result = vec4(0);
	float t = pow(2,exp[chr]) * v * (1 + whites[chr]);
	float val = (4 * pow((1-t),3) * t * (0.25 + low[chr] - contrast[chr])) + (6 * pow((1-t),2) * pow(t,2) * (0.5 + mid[chr])) + (4 * (1-t) * pow(t,3) * (0.75 + high[chr] + contrast[chr])) + (pow(t,4) * 1);
	return val;
}

float edit_channel_lum(float v) {
	return edit_channel(v, 0);
}

vec4 edit_channels(vec4 pix) {
	vec3 res = vec3(edit_channel(pix[2], 1),edit_channel(pix[1], 2),edit_channel(pix[0], 3));
	return vec4(edit_channel_lum(res[0]),edit_channel_lum(res[1]),edit_channel_lum(res[2]), 1);
}


vec4 convolution(float kernel[9]) {
	vec4 result = vec4(0);
	for (int i = 0; i < 3; i ++) {
		for (int j = 0; j < 3; j ++) {
			result += kernel[(3*i) + j] * texture(texture1, texOffsets[(3*i) + j]) * texture(texture1, texOffsets[(3*i) + j]);
		}
	}
	return result;
}


float calcLum(vec4 v) {
	return (0.2126 * v.x + 0.7152 * v.y + 0.0722 * v.z);
}

float norm_dist(float x, float s, float m) {
	// s = standard dev
	// m = mean
	// Approx
	if (x <= m)
		return 1.0;
	return ( (1/(s * 2.5)) * pow(2, -0.5 * pow( (x - m)/s, 2 )) ) / (1/(s * 2.5));
	//return min(- s * log(x - m),1);
}

float foo(float x) {
	return -pow(0.8, (7.3 * x) - 1) + (1/0.8);
}

vec4 shad_convolution(vec4 pix) {
	//pix = vec4(pix.z, pix.y, pix.x, 1);
	//float kernel[9] = {};
	float mean = 0.0f;
	for (int i = 0; i < 3; i ++) {
		for (int j = 0; j < 3; j ++) {
			mean += calcLum(texture(texture1, texOffsets[(3*i) + j]));
		}
	}
	mean /= 9.0f;

	float var = 0;
	for (int i = 0; i < 3; i ++) {
		for (int j = 0; j < 3; j ++) {
			var += pow(calcLum(texture(texture1, texOffsets[(3*i) + j])) - mean, 2.0f);
			//var += calcLum(texture(texture1, texOffsets[(3*i) + j])) - mean;
		}
	}
	//var /= 8.0f;
	var *= var_mult;

	//vec4 add = vec4(0,0,0, 1);
	float add = 0;
	vec4 result = vec4(1);

	for (int i = 0; i < 3; i ++) {
		float lum = pix[i];
		//float lum = calcLum( pix );
		//if (var > 0.05)
		//	return result;

		//if (mean < shad_thresh) {
		add = norm_dist(mean, shad_var, shad_thresh) - var;
		//  pow(2,shad_incr) * 
		// - (shad_incr * (1 - norm_dist(lum, shad_var, shad_thresh) ) )
		//}
		//if (mean > high_thresh) {
		//add *= (1 + high_incr) * norm_dist(lum, 0.07, high_thresh);
		//}
		//result[i] = min((pix[i] * add) , 1);
		result[i] = min((pix[i] * add) , 1);
		//result[3-i] = max(  , lum);
	}
	
	result = vec4(result.x * shad_incr, result.y * shad_incr, result.z * shad_incr, 1);
	
	//return max(add, result) ;
	//return vec4(vec3( max((lum * foo(add)), lum) ), 1);
	//return vec4(vec3( max( min((lum * add) * pow(2,shad_incr) , 1) , lum)  ), 1);
	if (b4)
		return result;

	if (calcLum(result) > calcLum(pix))
		return result;
	else
		return pix;
	//return vec4(vec3(max(result, pix)),1);
	//return vec4(vec3((lum * add)),1);
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 rgb2xyz(vec3 v) {
	mat3 m = mat3(vec3(0.412453,0.212671,0.019334), vec3(0.357580,0.715160,0.119193), vec3(0.180423,0.072169,0.950227));
	return m*v;
}

vec3 xyz2rgb(vec3 v) {
	mat3 m = mat3(vec3(3.240479,-0.969256,0.055648), vec3(-1.537150,1.875992,-0.204043), vec3(-0.498535,0.041556,1.057311));
	return m*v;
}

vec3 xyY2xyz(vec3 v) {
	// v = xyY
	vec3 r = vec3(0);
	r.x = (v.z / v.y) * v.x;
	r.y = v.z;
	r.z = (v.z / v.y) * (1-v.x-v.y);
	return r;
}

vec3 kelvinToRGB(float T) {
	float x = 0;
	float y = 0;
	if (T >= 1667 && T < 4000) 
		x = -0.2661239 * (pow(10,9) / pow(T, 3)) -0.2342589 * (pow(10,6) / pow(T, 2)) + 0.8776956 * (pow(10,3) / T) + 0.179910;
	else if (T >= 4000 && T < 25000) 
		x = -3.0258469 * (pow(10,9) / pow(T, 3)) +2.1070379 * (pow(10,6) / pow(T, 2)) + 0.2226347 * (pow(10,3) / T) + 0.240390;

	if (T >= 1667 && T < 2222)
		y = -1.1063814 * pow(x, 3) -1.34811020 * pow(x, 2) +2.18555832 * x - 0.20219683;
	else if (T >= 2222 && T < 4000)
		y = -0.9549476 * pow(x, 3) -1.37418593 * pow(x, 2) +2.09137015 * x - 0.16748867;
	else if (T >= 4000 && T < 25000)
		y =  3.0817580 * pow(x, 3) -5.87338670 * pow(x, 2) +3.75112997 * x - 0.37001483;

	return xyz2rgb(xyY2xyz(vec3(x,y,1)));
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float norm_dist_high(float x, float s, float m) {
	// s = standard dev
	// m = mean
	// Approx
	if (x >= m)
		return 1.0;
	return ( (1/(s * 2.5)) * pow(2, -0.5 * pow( (x - m)/s, 2 )) ) / (1/(s * 2.5)) * shad_thresh;
	//return min(- s * log(x - m),1);
}


float norm_dist_high_nomax(float x, float s, float m) {
	// s = standard dev
	// m = mean
	// Approx
	return ( (1/(s * 2.5)) * pow(2, -0.5 * pow( (x - m)/s, 2 )) ) / (1/(s * 2.5));
	//return min(- s * log(x - m),1);
}

vec4 highlight_recover(vec4 pix) {
	float mean = 0.0f;
	for (int i = 0; i < 3; i ++) {
		for (int j = 0; j < 3; j ++) {
			mean += calcLum(texture(texture1, texOffsets[(3*i) + j]));
		}
	}
	mean /= 9.0f;

	float var = 0;
	for (int i = 0; i < 3; i ++) {
		for (int j = 0; j < 3; j ++) {
			var += pow(calcLum(texture(texture1, texOffsets[(3*i) + j])) - mean, 2.0f);
		}
	}
	var *= var_mult;

	float add = 0;
	int zero_count = 0;
	vec4 result = vec4(1);

	for (int i = 0; i < 3; i ++) {
		float lum = pix[i];
		add = norm_dist_high(mean, shad_var, high_thresh) - var;
		//result[i] = min((pix[i] * add) , 1);
		if (add > 0) {
			add = 1; 
			zero_count = zero_count + 1;
		}
		result[i] = pix[i] * add;
		if (result[i] > 1)
			result[i] = 1;
	}

	//result += result * norm_dist_high(mean, shad_var, high_thresh) * pow(2,high_incr);
	/*
	for (int i = 0; i < 3; i ++) {
		result[i] *= max(1 + (high_incr * norm_dist_high(mean, shad_var, high_thresh)), 0);
	}
	*/

	vec3 res_hsv = rgb2hsv(vec3(result.x, result.y, result.z));
//	result = vec4(hsv2rgb(vec3(res_hsv.x, res_hsv.y, res_hsv.z + (high_incr * norm_dist_high(mean, shad_var, high_thresh)))), 1);

	result = vec4(hsv2rgb(vec3(res_hsv.x, res_hsv.y, res_hsv.z + (high_incr * (norm_dist_high(mean, shad_incr, high_thresh))))), 1);

	if (zero_count < 3)
		return pix;
	else 
		return result;
	
}

float expose_chr(float v) {
	return min( v * pow(2,exp[0]), 1 );
}

vec4 expose(vec4 v) {
	 return vec4(expose_chr(v.x),expose_chr(v.y),expose_chr(v.z),1);
}

vec4 apply_sat(vec4 v) {
	vec3 hsv = rgb2hsv(v.xyz);
	hsv.y *= (1 + sat);
	vec3 rgb = hsv2rgb(hsv);
	return vec4(rgb, 1);
}

vec4 apply_wb(vec4 v) {
	//return v + vec4(wb, 0, -wb, 0);
	//int index = int(floor(wb));
	//vec3 temp = kelvin_table[index] / 255;
	return v * vec4(kelvinToRGB(wb),1);
}

vec4 clamp_pix(vec4 v) {
	if (calcLum(v) <= 0.0) {
		return vec4(0.0, 0.0, 0.0, 1);
	}
	if (calcLum(v) >= 1.0) {
		return vec4(1.0, 1.0, 1.0, 1);
	}
	return v;
}

void main()
{
	vec4 pix = texture(texture1, TexCoord) * texture(texture1, TexCoord);	
	//vec4 pix = expose(texture(texture1, TexCoord));	
	//FragColor = edit(convolution(sharp_kernel));
	
	
	if (b4) {
		vec4 result = vec4(pix.z, pix.y, pix.x, 1);
		if (bw) {
			result = vec4(vec3(calcLum(result)), 1);
		}
		FragColor = result;
		return;
	}
	

	vec4 result = clamp_pix(edit_channels(convolution(sharp_kernel33)));
	result = clamp_pix(highlight_recover(result));
	result = clamp_pix(apply_wb(result));
	result = clamp_pix(apply_sat(result));

	if (bw) {
		result = vec4(vec3(calcLum(result)), 1);
	}

	if (clip) {
		if (calcLum(result) <= 0.0) {
			result = vec4(1.0, 1.0, 0.0, 1);
		}
		if (calcLum(result) >= 1.0) {
			result = vec4(1.0, 0, 1.0, 1);
		}
	} 
	
	FragColor = result;
	//FragColor = edit_channels(pix);
}