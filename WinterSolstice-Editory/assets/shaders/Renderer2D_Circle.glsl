//--------------------------
// - Hazel 2D -
// Renderer2D Circle Shader
// --------------------------

#type vertex
#version 450 core

layout(location = 0) in vec3 a_WorldPosition;
layout(location = 1) in vec3 a_LocalPosition;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_Thickness;
layout(location = 4) in float a_Fade;
layout(location = 5) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
    vec3 Normal; // Add Normal here
	vec3 WorldPosition;
};

layout (location = 0) out VertexOutput Output;
layout (location = 6) out flat int v_EntityID;

void main()
{
	Output.LocalPosition = a_LocalPosition;
	Output.Color = a_Color;
	Output.Thickness = a_Thickness;
	Output.Fade = a_Fade;

	v_EntityID = a_EntityID;
	Output.Normal = normalize(mat3(u_ViewProjection) * vec3(0.0,0.0,1.0));
	vec4 position = u_ViewProjection * vec4(a_WorldPosition, 1.0);
	Output.WorldPosition = position.xyz;
	gl_Position = position;
}

#type fragment
#version 450 core

layout (location = 0) out vec4 o_Color;
layout (location = 1) out int o_EntityID;
layout (location = 2) out vec4 o_Bloom;

layout (location = 3) out vec4 o_Position;
layout (location = 4) out vec4 o_Normal;

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
    vec3 Normal; // Add Normal here
	vec3 WorldPosition;
};

layout (location = 0) in VertexOutput Input;
layout (location = 6) in flat int v_EntityID;

void main()
{
    // Calculate distance and fill circle with white
    float distance = 1.0 - length(Input.LocalPosition);
    float circle = smoothstep(0.0, Input.Fade, distance);
    circle *= smoothstep(Input.Thickness + Input.Fade, Input.Thickness, distance);

	if (circle == 0.0)
		discard;
	o_Bloom = vec4(1.0);
	o_Position = vec4(Input.WorldPosition,1.0);
	o_Normal = vec4(Input.Normal,1.0);
    // Set output color
    o_Color = Input.Color;
	o_Color.a *= circle;

	o_EntityID = v_EntityID;
}
