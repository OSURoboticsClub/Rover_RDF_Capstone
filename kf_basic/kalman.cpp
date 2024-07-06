#include "kalman.hpp"
#include <Arduino.h>

using namespace Eigen;

KF::KF(int x, int z, int u){
    //set dimensions to parameters
    this->dim_x = x;
    this->dim_z = z;
    this->dim_u = u;

    //initialize identity matrix
    this->I.setIdentity(this->dim_x, this->dim_x);

    //Initialize system matricies
    this->x.Zero(this->dim_x, 1);
    this->P.setIdentity(this->dim_x, this->dim_x);
    this->Q.setIdentity(this->dim_x, this->dim_x);
    this->F.setIdentity(this->dim_x, this->dim_x);
    this->R.setIdentity(this->dim_z, this->dim_z);
    
    this->B.Zero(this->dim_x, this->dim_u);
    this->z.Zero(this->dim_z, 1);
    this->y.Zero(this->dim_z, 1);
    this->K.Zero(this->dim_x, this->dim_z);
    this->S.Zero(this->dim_z, this->dim_z);
    this->SI.Zero(this->dim_z, this->dim_z);
    this->u.Zero(this->dim_u, 1);
}

KF::KF(){}

KF::~KF(){}

void KF::update(const MatrixXf z){
    //calculate the residual
    y = z - H * x;

    //S = HPH' + R -> Update the system uncertainty
    S = H*P*H.transpose() + R;

    //get inverse of S for kalman gain
    SI = S.inverse();

    //calculate the kalman gain
    K = P*H.transpose()*SI;

    //calculate new x
    x = x + K*y;

    //P = (I-KH)P(I-KH)' + KRK' <- this is more numerically stable than just I - (KH)P
    (I-K*H)*P*(I-K*H).transpose() + K*R*K.transpose();

}

void KF::predict(){
    //calculate the prior x
    x = F*x + B*u;

    //calculate the prior P
    P = F*P*F.transpose() + Q;
}


//The below function comes from the bolder flight eigen example by RandomVibe
// PRINT MATRIX (float type)
// By: randomvibe
//-----------------------------
void KF::print_mtxf_arduino(const Eigen::MatrixXf& X)  
{
   int i, j, nrow, ncol;
   nrow = X.rows();
   ncol = X.cols();
   Serial.print("nrow: "); Serial.println(nrow);
   Serial.print("ncol: "); Serial.println(ncol);       
   Serial.println();
   for (i=0; i<nrow; i++)
   {
       for (j=0; j<ncol; j++)
       {
           Serial.print(X(i,j), 6);   // print 6 decimal places
           Serial.print(", ");
       }
       Serial.println();
   }
   Serial.println();
}

