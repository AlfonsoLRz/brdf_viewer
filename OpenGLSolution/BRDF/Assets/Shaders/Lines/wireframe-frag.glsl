#version 450

uniform vec3 vColor;

layout (location = 0) out vec4 fColor;

void main() {
	/**const vec2 viewportSize = vec2(1050, 650) * 4;
	const vec2 screenCoord = gl_FragCoord.xy / viewportSize;
	const vec2 maxDistance = vec2(.5f);
	const float clampDistance = 0.3f;
	const vec2 distCenter = abs(vec2(.5f) - screenCoord);
	vec2 alpha = vec2(1.0f);
	float addDistance = 0.1f;

	if (distCenter.x > clampDistance || distCenter.y > clampDistance)
	{
		alpha.x = pow(smoothstep(.0f, 1.0f, 1.0f - (distCenter.x - clampDistance) / (maxDistance.x - clampDistance)), 2);
		alpha.y = pow(smoothstep(.0f, 1.0f, 1.0f - (distCenter.y - clampDistance) / (maxDistance.y - clampDistance)), 2);
	}

	float globalDistance = distance(vec2(.5f), screenCoord);

	if (globalDistance < clampDistance) alpha.x = 1.0f;
	else if (globalDistance >= clampDistance + addDistance) alpha.x = .0f;
	else alpha.x = pow(smoothstep(.0f, 1.0f, 1.0f - (globalDistance - clampDistance) / (addDistance)), 3);**/

	fColor = vec4(vColor, 1.0f);
}