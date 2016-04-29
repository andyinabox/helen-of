// fragment shader
#version 120

//https://www.opengl.org/registry/specs/EXT/texture_array.txt
//http://www.gamedev.net/topic/508125-glsl-array-of-textures/

// this is how we receive the texture
uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform sampler2DRect tex2;
uniform sampler2DRect tex3;
uniform sampler2DRect tex4;
uniform sampler2DRect tex5;
uniform sampler2DRect tex6;
uniform sampler2DRect tex7;
uniform sampler2DRect tex8;
uniform sampler2DRect tex9;

uniform float dMultiply;
uniform vec2 direction;

varying vec2 texCoordVarying;



vec2 updateSt(in vec2 st, in vec4 last, in vec2 dir, in float amount) {
    return(st + vec2( (last.x + dir.x) * amount, (last.z + dir.y) * amount));
}

void main()
{

    int n = 10;
    vec4 avg = vec4(0.0);
    vec2 st = texCoordVarying.st;
    
//    avg = mix(texture2DRect(tex0, texCoordVarying), avg, 1.0/n);
//    avg = mix(texture2DRect(tex1, texCoordVarying), avg, 1.0/n);
//    avg = mix(texture2DRect(tex2, texCoordVarying), avg, 1.0/n);
//    avg = mix(texture2DRect(tex3, texCoordVarying), avg, 1.0/n);
//    avg = mix(texture2DRect(tex4, texCoordVarying), avg, 1.0/n);
//    avg = mix(texture2DRect(tex5, texCoordVarying), avg, 1.0/n);
//    avg = mix(texture2DRect(tex6, texCoordVarying), avg, 1.0/n);
//    avg = mix(texture2DRect(tex7, texCoordVarying), avg, 1.0/n);
//    avg = mix(texture2DRect(tex8, texCoordVarying), avg, 1.0/n);
//    avg = mix(texture2DRect(tex9, texCoordVarying), avg, 1.0/n);

//    vec4 color = texture2DRect(tex0, st);
//    vec2 st2 = st + vec2( (color.x - 0.5) * 5.0, (color.z - 0.5) * 5.0);


    vec4 texel0 = texture2DRect(tex0, st);
    st = updateSt(st, texel0, direction, 1.0*dMultiply);
    vec4 texel1 = texture2DRect(tex1, st);
    st = updateSt(st, texel1, direction,2.0*dMultiply);
    vec4 texel2 = texture2DRect(tex2, st);
    st = updateSt(st, texel2, direction,3.0*dMultiply);
    vec4 texel3 = texture2DRect(tex3, st);
    st = updateSt(st, texel3, direction,4.0*dMultiply);
    vec4 texel4 = texture2DRect(tex4, st);
    st = updateSt(st, texel4, direction,5.0*dMultiply);
    vec4 texel5 = texture2DRect(tex5, st);
    st = updateSt(st, texel5, direction,6.0*dMultiply);
    vec4 texel6 = texture2DRect(tex6, st);
    st = updateSt(st, texel6, direction,7.0*dMultiply);
    vec4 texel7 = texture2DRect(tex7, st);
    st = updateSt(st, texel7, direction,8.0*dMultiply);
    vec4 texel8 = texture2DRect(tex8, st);
    st = updateSt(st, texel8, direction,9.0*dMultiply);
//    vec4 texel9 = texture2DRect(tex9, st);
    
    
    avg = texture2DRect(tex1, st);
    avg += texture2DRect(tex1, st);
    avg += texture2DRect(tex2, st);
    avg += texture2DRect(tex3, st);
    avg += texture2DRect(tex4, st);
    avg += texture2DRect(tex5, st);
    avg += texture2DRect(tex6, st);
    avg += texture2DRect(tex7, st);
    avg += texture2DRect(tex8, st);
    avg += texture2DRect(tex9, st);
    avg = avg / n;

    
    gl_FragColor = vec4(avg.rgb, 1.0);
}
