#include <math.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N) {
    float aux=0.0;
    for(int i=0; i<N;i++){
        aux += x[i]*x[i];
    }
    //float aux2=aux/N;
    float PdB=10*log10(aux/N); 
    return PdB;
}

float compute_am(const float *x, unsigned int N) {
    float suma=0.0;
    for(int i=0; i<N;i++){
        suma+=fabs(x[i]);
    }
    float am=suma/N;
    return am;
}

float compute_zcr(const float *x, unsigned int N, float fm) {
    int d=0;
    for(int i=1;i<N;i++){  
        if((x[i]>0 && x[i-1]>0)||(x[i]<0 && x[i-1]<0)){
            d += 0;
        }
        else d += 1;
    }
    float zcr=(fm/2)*d/(N-1);
    return zcr;
 }