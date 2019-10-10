#version 330 core
uniform vec3 viewPos;
uniform vec3 lPos;


in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D texture0;
uniform sampler2D shadowMap;
out vec4 f_color;



vec3 CalLight(vec3 lightPos,vec3 lightColor,float ambient,float specular,float shadow)//漫反射系数，镜面反射系数，若不用阴影则设为-1
{
    //vec3 norm = calBumpedNormal();
    vec3 norm = fs_in.Normal;
    vec3 ambientLight = ambient*lightColor;

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);

    float diffuse = max(dot(norm, lightDir),0.0);

    vec3 diffuseLight = diffuse*lightColor;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 specularLight = specular * spec * lightColor;
	
    if(shadow < 0)
    return (ambientLight+diffuseLight+specularLight);
	else
	return (ambientLight + (1.0 - shadow) * (diffuseLight + specularLight)) * lightColor;  
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;
	//有锯齿会失真，采用一个偏移量来进行更正.深度图1024*1024分辨率会受限于解析度，所以在远处的片元有可能从深度贴图的同一个值采样
	//对于具有梯度的物体，如果相邻两片元从深度图获得同一深度d，但是事实上两者的currentDepth是不同的，就会造成一个在表面，一个在表面下（认为是阴影）
	//float bias = 0.005;用bias让每个片元的深度均在表面上。更加可靠的办法能够根据表面朝向光线的角度更改偏移量：使用点乘
	vec3 normal = fs_in.Normal;
	vec3 lightDir = normalize(lPos - fs_in.FragPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // 检查当前片元是否在阴影中
	//float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
	
	//进行光滑
	float shadow = 0.0;
	//这个textureSize返回一个给定采样器纹理的0级mipmap的vec2类型的宽和高。用1除以它返回一个单独纹理像素的大小，我们用以对纹理坐标进行偏移，确保每个新样本，来自不同的深度值。
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

    return shadow;
}
void main()
{

  	
	vec3 lightPos = lPos;
	vec3 color = texture(texture0, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
 	
 // 计算阴影

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace); 
    vec3 lighting = CalLight(lightPos,lightColor,0.3,0.2,shadow) ;

    f_color = vec4(lighting*color,1.0);
	
}




