#include "Algorithm_filter.h"



/*====================================================================================================*/
/*====================================================================================================*
** ��������: IIR_I_Filter
** ��������: IIRֱ��I���˲���
** ��    ��: InData Ϊ��ǰ����
**           *x     ����δ�˲�������
**           *y     �����˲��������
**           *b     ����ϵ��b
**           *a     ����ϵ��a
**           nb     ����*b�ĳ���
**           na     ����*a�ĳ���
**           LpfFactor
** ��    ��: OutData         
** ˵    ��: ��
** ����ԭ��: y(n) = b0*x(n) + b1*x(n-1) + b2*x(n-2) -
                    a1*y(n-1) - a2*y(n-2)
**====================================================================================================*/
/*====================================================================================================*/
double IIR_I_Filter(double InData, double *x, double *y, double *b, short nb, double *a, short na)
{
  double z1,z2;
  short i;
  double OutData;
  
  for(i=nb-1; i>0; i--)
  {
    x[i]=x[i-1];
  }
  
  x[0] = InData;
  
  for(z1=0,i=0; i<nb; i++)
  {
    z1 += x[i]*b[i];
  }
  
  for(i=na-1; i>0; i--)
  {
    y[i]=y[i-1];
  }
  
  for(z2=0,i=1; i<na; i++)
  {
    z2 += y[i]*a[i];
  }
  
  y[0] = z1 - z2; 
  OutData = y[0];
    
  return OutData;
}

/*====================================================================================================*/
/*====================================================================================================*
**���� : KalmanFilter
**���� : �������˲�
**���� :  
**ݔ�� : None
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
double KalmanFilter(const double ResrcData,double ProcessNiose_Q,double MeasureNoise_R,double x_last,double p_last)
{
   double R = MeasureNoise_R;
   double Q = ProcessNiose_Q;
   double x_mid = x_last;
   double x_now;
   double p_mid ;
   double p_now;
   double kg;        

   x_mid=x_last;          //x_last=x(k-1|k-1),x_mid=x(k|k-1)
   p_mid=p_last+Q;        //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
   kg=p_mid/(p_mid+R);    //kgΪkalman filter��RΪ����
   x_now=x_mid+kg*(ResrcData-x_mid);//���Ƴ�������ֵ
                
   p_now=(1-kg)*p_mid;   //����ֵ��Ӧ��covariance       
   p_last = p_now;       //����covarianceֵ
   x_last = x_now;       //����ϵͳ״ֵ̬
   return x_now;                
}



/*====================================================================================================*/
/*====================================================================================================*
**���� : LPF_1st
**���� : һ�׵�ͨ�˲�
**���� :  
**ݔ�� : None
**��ע : None
**====================================================================================================*/
/*====================================================================================================*/
float LPF_1st(float oldData, float newData, float lpf_factor)
{
	return oldData * (1 - lpf_factor) + newData * lpf_factor;
}
#define new_weight 0.5f
#define old_weight 0.5f
float value;
float LPF( float oldData,float newData)
{
//		static float lastacc;
	 value = (float) newData * new_weight +oldData * old_weight ;
		return value ;
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/