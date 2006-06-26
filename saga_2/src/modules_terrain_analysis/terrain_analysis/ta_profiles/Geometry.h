
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Compute the dot product AB ? BC
float dot(float *A, float *B, float *C){
    
	float *AB = new float[2];
    float *BC = new float[2];
    AB[0] = B[0]-A[0];
    AB[1] = B[1]-A[1];
    BC[0] = C[0]-B[0];
    BC[1] = C[1]-B[1];
    float dot = AB[0] * BC[0] + AB[1] * BC[1];
    
	return dot;
}
//Compute the cross product AB x AC
float cross(float *A, float *B, float *C){
    
	float *AB = new float[2];
    float *AC = new float[2];
    AB[0] = B[0]-A[0];
    AB[1] = B[1]-A[1];
    AC[0] = C[0]-A[0];
    AC[1] = C[1]-A[1];
    float cross = AB[0] * AC[1] - AB[1] * AC[0];
    
	return cross;
}
//Compute the distance from A to B
float distance(float *A, float *B){
    float d1 = A[0] - B[0];
    float d2 = A[1] - B[1];
    return sqrt(d1*d1+d2*d2);
}
//Compute the distance from AB to C

float linePointDist(float *A, float *B, float *C){
    float dist = cross(A,B,C) / distance(A,B);

    return fabs(dist);
}
