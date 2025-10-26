#version 430

// #define M_PI 3.1415926535897932384626433832795

vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

layout (local_size_x = 16, local_size_y = 16) in; // Workgroup size

struct RenderPrimitive {
	int type;          
    int texUnit;
	float z;           // z height
    float id;
	vec2 a, b, c; // Points on the triangle. c unused if type 1
    vec2 uv_a, uv_b, uv_c; // uv coords. unused by type 1
	vec2 center_ab, center_bc, center_ca; // centers of each line as a Euclidean circle. bc and ca unused if type 1
	double r_ab, r_bc, r_ca; // radii of each line as a Euclidean circle. 
            //If type 1, r_bc is line weight and r_ca is +/- 1 signifying euclidean or hyperbolic metric for the line weight
};

layout(std430, binding = 0) buffer PrimitiveBuffer {
    RenderPrimitive primitives[];
};

layout(std430, binding = 1) buffer BitBuffer {
    uint collision_bits[]; // reinterpret as bits manually
};

uniform int primitiveCount;
uniform int WIDTH;
uniform int HEIGHT;

// Texture output
layout(rgba32f, binding = 0) writeonly uniform image2D outputTexture;

// Texture input
uniform sampler2D textures[16]; // Look up docs to see how big this array can be

vec2 Conj(vec2 z) {
    return vec2(z.x, -z.y);
}

vec2 CMult(vec2 u, vec2 v) {
    return vec2(u.x*v.x - u.y*v.y, u.x*v.y + u.y*v.x);
}

float CNormSquared(vec2 z) {
    return z.x*z.x + z.y*z.y;
}

vec2 CDiv(vec2 u, vec2 v) {
    return CMult(u, Conj(v)) / CNormSquared(v);
}
double CNorm(vec2 z) {
    return length(z);
}

double CDistance(vec2 u, vec2 v) {
    return CNorm(u - v);
}


vec2 OPlus(vec2 a, vec2 b) {
    return CDiv(a + b, vec2(1, 0) + CMult(Conj(a), b));
}

vec2 OTimes(float r, vec2 a) {
	if (a.x == 0 && a.y ==0) {
		return a;
	}
	float mag = length(a);
	return tanh(r * atanh(mag)) * a / mag;
}

vec2 ConvertToEinstein(vec2 a) {
    return OTimes(2.0, a);
}

float Gamma(vec2 a) {
    return 1.0 / sqrt(1 - CNormSquared(a));
}

vec2 EPlus(vec2 u, vec2 v) {
    float g = Gamma(u);
    return (1.0/(1 + dot(u, v))) * (
    u + (1.0/g)*v + (g/(1.0+g))*dot(u, v)*u
    );
}

double GNorm(vec2 a) {
		return 2*atanh(length(a)); //WARN: Magic numbers. Assumes MobiusGyrovector::UNIT_LENGTH == tanh(0.5);
}

double GDistance(vec2 a, vec2 b) {
    return GNorm(OPlus(-a, b));
}

// true if point b is clockwise of point a, false if b is counterclockwise of a (or they're collinear with the origin)
bool isClockwise(vec2 a, vec2 b) {
    // find the k component of the cross product (a x b).
	// If it's positive, a->b is CCW around the origin. If it's negative, a->b is CW around the origin
	return a.x * b.y - a.y * b.x < 0;
}

bool isPointOutsideCircle(vec2 point, vec2 center, double radius) {
    return length(point - center) > radius;
}

bool isPointOnRightSideOfLine(vec2 point, vec2 a, vec2 b, vec2 center, double radius) {
    // whether we want to be inside or outside a euclidean circle depends on whether the origin is inside that triangle
    // Since triangle vertices are ALWAYS ORDERED COUNTERCLOCKWISE, we can check that using isClockwise.
    // If ab is CCW, we're inside that line when we're outside the circle
    // If ab is CW, we're inside that line when we're inside the circle

    // However, first we should check if the line passes through the origin (radius is infinite)
    if (isinf(radius) || isnan(radius) || abs(radius) > 100000) {
        if (a.x == b.x) {
            return a.y > b.y ? point.x >= a.x : point.x <= a.x;
        }
        float slope = (b.y - a.y)/(b.x - a.x);
        return a.x > b.x ? point.y <= slope*(point.x - a.x) + a.y : point.y >= slope*(point.x - a.x) + a.y;
    }
    
    return isClockwise(a, b) ? !isPointOutsideCircle(point, center, radius) : isPointOutsideCircle(point, center, radius);
}

bool isPointInTriangle(vec2 point, RenderPrimitive triangle) {
    return true
        && isPointOnRightSideOfLine(point, triangle.a, triangle.b, triangle.center_ab, triangle.r_ab)
        && isPointOnRightSideOfLine(point, triangle.b, triangle.c, triangle.center_bc, triangle.r_bc)
        && isPointOnRightSideOfLine(point, triangle.c, triangle.a, triangle.center_ca, triangle.r_ca);
        ;
}

vec2 NearestPointOnLine(vec2 point, vec2 a, vec2 b, vec2 center, double radius) {
    if (isinf(radius) || isnan(radius) || abs(radius) > 100000) {
        // just project onto the euclidean line abs
        return a + (dot(point-a, b-a)/dot(b-a, b-a))*(b-a);
    }
    vec2 delta = point - center;
    return center + float(radius) * delta / length(delta);
}

bool isPointNearLine(vec2 point, RenderPrimitive line) {
    vec2 proj = NearestPointOnLine(point, line.a, line.b, line.center_ab, line.r_ab);
    double dist = line.r_ca > 0 ? CDistance(point, proj) : GDistance(point, proj);
    return dist < line.r_bc/2.0;
}

// returns angle from a to b clockwise about the origin. Range=[-pi, pi]
float Angle(vec2 a, vec2 b) {
	// get the abs value of the angle in [0, pi]
	// clamping to [-1, 1] avoids floating point errors
	float dotp = float((a.x*b.x + a.y*b.y) / (CNorm(a) * CNorm(b)));
	float angle = acos(min(max(dotp, -1.0), 1.0));
	return isClockwise(a, b) ? -angle : angle;
}

float Angle(vec2 a, vec2 b, vec2 about) {
	return Angle(OPlus(-about, a), OPlus(-about, b));
}




vec4 barycentric(vec2 ndc, RenderPrimitive tri) {
    vec2 A = ConvertToEinstein(tri.a), B = ConvertToEinstein(tri.b), 
        C = ConvertToEinstein(tri.c), P = ConvertToEinstein(ndc);

    vec2 AP = EPlus(-A, P), AB = EPlus(-A, B), AC = EPlus(-A, C);
    vec2 gAB = Gamma(AB)*AB, gAC = Gamma(AC)*AC;
    // write AP as a linear combination of gAB and gAC
    mat2 changeOfBasis = mat2(gAB, gAC);
    vec2 basisChanged = inverse(changeOfBasis)*AP;
    float mB = basisChanged.x, mC = basisChanged.y, mA = 1 - mB*Gamma(AB) - mC*Gamma(AC);
    // renormalize weights
    float total = mA + mB + mC;
    mA /= total; mB /= total; mC /= total;

    vec2 uv = mA*tri.uv_a + mB*tri.uv_b + mC*tri.uv_c;
    return texture(textures[tri.texUnit], uv);
}

// Alpha blend "top over bottom" using straight alpha
vec4 alphaBlend(vec4 bottom , vec4 top) {
    // formula from wikipedia.
    vec3 C_a = top.xyz; float a_a = top.w;
    vec3 C_b = bottom.xyz; float a_b = bottom.w;
    float a_o = a_a + a_b*(1 - a_a);
    if (a_o < 1e-5) {
        // we have some floating point error. The resultant color should just be transparent.
        return vec4(0.0);
    }
    vec3 C_o = (C_a*a_a + C_b*a_b*(1-a_a))/a_o;
    return vec4(C_o, a_o);
}

///
/// debug
///
vec3 toBinary(int a) {
    vec3 rgb = vec3(0.0, 0.0, 0.0);
    if (a > 3) {
        rgb.r = 1.0;
        a -= 4;
    }
    if (a > 1) {
        rgb.g = 1.0;
        a -= 2;
    }
    if (a > 0) {
        rgb.b = 1.0;
    }
    return rgb;
}


void main() {
    //color = vec4((collision_bits[4*600*600-1] & (1 << 31)) >> 31, 0.0, 0.0, 1.0);
    // Get the coordinates of the pixel
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);

    // I know width and height should be passed in as uniforms. Ignore it; that's not the issue
    vec2 ndc = (coords - ivec2(WIDTH / 2, HEIGHT / 2)) / vec2(WIDTH/2.0, HEIGHT/2.0);
    

    if (length(ndc) > 1) { // ignore everything outside the unit disk
        color = vec4(0.4, 0.4, 0.4, 1.0);
        imageStore(outputTexture, coords, color);
        return;
    }
    
    for (int i = 0; i < primitiveCount; i++) {
        switch (primitives[i].type) {
            case 0:
                if (isPointInTriangle(ndc, primitives[i])) {
                    color = alphaBlend(color, barycentric(ndc, primitives[i]));
                }
                break;
            case 1:
                if (isPointNearLine(ndc, primitives[i])) {
                    color = vec4(float(coords.x) / 600.0, float(coords.y) / 600.0, 0.0, 1.0);
                }
                break;
            default:
                break;
        }
        if (color.w == 1.0) {
            break;
        }
    }
    

    // Write color to the texture
    imageStore(outputTexture, coords, color);
}
