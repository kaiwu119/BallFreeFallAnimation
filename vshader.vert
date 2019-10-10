#version 330 core
uniform mat4 m_projection;
uniform mat4 m_view;
uniform mat4 m_model;

in vec3 a_position;
in vec3 a_normal;
in vec3 a_texcoord;



out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = m_projection*m_view*m_model*vec4(a_position,1.0);

   /* v_position = vec3(m_model*vec4(a_position,1.0));
    v_texcoord = a_texcoord;
    v_normal = vec3(normalize(m_model*vec4(a_normal,0.0)));*/
	
	vs_out.FragPos = vec3(m_model * vec4(a_position, 1.0));
    vs_out.Normal = transpose(inverse(mat3(m_model))) * a_normal; //model矩阵的逆矩阵的转置矩阵缩放操作对法线的影响
    vs_out.TexCoords = a_texcoord.xy;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

}


