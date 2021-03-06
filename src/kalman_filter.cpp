#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  VectorXd y = z - H_ * x_;
  MatrixXd Ht = H_.transpose();
  MatrixXd P_Ht = P_ * H_.transpose();
  MatrixXd S = H_ * P_Ht + R_;
  MatrixXd Si = S.inverse();
  MatrixXd K =  P_Ht * Si;

  //new state
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_;
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
    * update the state by using Extended Kalman Filter equations
  */
  const float px = x_(0,0);
  const float py = x_(1,0);
  const float vx = x_(2,0);
  const float vy = x_(3,0);
  const float eps = 1e-5;
  const float rho = sqrt(px * px + py * py);
  const float phi = atan2(py, px);
  const float rho_dot = (px * vx + py * vy) / (eps + rho);

  VectorXd z_pred(3);
  z_pred << rho, phi, rho_dot;
  // Update state by implementing EKF equations
  VectorXd y = z - z_pred;
  MatrixXd Ht = H_.transpose();
  MatrixXd P_Ht = P_ * H_.transpose();
  MatrixXd S = H_ * P_Ht + R_;
  MatrixXd Si = S.inverse();
  MatrixXd K = P_Ht * Si;

  // New estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_;
}