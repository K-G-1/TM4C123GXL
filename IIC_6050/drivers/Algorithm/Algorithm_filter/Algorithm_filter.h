#ifndef __Algorithm_filter_H
#define	__Algorithm_filter_H


double IIR_I_Filter(double InData, double *x, double *y, double *b, short nb, double *a, short na);
double KalmanFilter(const double ResrcData,double ProcessNiose_Q,double MeasureNoise_R,double x_last,double p_last);
float LPF_1st(float oldData, float newData, float lpf_factor);
float LPF( float oldData,float newData);
#endif /* __Algorithm_filter_H */
