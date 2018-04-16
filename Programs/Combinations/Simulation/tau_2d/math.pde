
/******************************************************************************
 * Rotation Calculation
 *****************************************************************************/
/* See - http://www.nxp.com/files/sensors/doc/app_note/AN3461.pdf and
 - http://www.nxp.com/assets/documents/data/en/application-notes/AN4248.pdf
 */

void normalizeValues( )
{
  normalizeAccel();
  normalizeMag();
}

void normalizeAccel()
{
  float n = sqrt( Accel[0] * Accel[0] + Accel[1] * Accel[1] + Accel[2] * Accel[2] );
  Accel[0] /= n;
  Accel[1] /= n;
  Accel[2] /= n;
}

void normalizeMag()
{
  float n = sqrt( Mag[0] * Mag[0] + Mag[1] * Mag[1] + Mag[2] * Mag[2] );
  Mag[0] /= n;
  Mag[1] /= n;
  Mag[2] /= n;
}

void calculateAngles( )
{
  calculateRoll();
  calculatePitch();
  calculateYaw();
}

void filterAngles( )
{
  pitch.value = RwEst[0];
  roll.value = RwEst[1];
  yaw.value = RwEst[2];
  
   //pitch.update(RwEst[0], Gyro[0]/1000);
   //roll.update(RwEst[1], Gyro[1]/1000);
   //yaw.update(RwEst[2], Gyro[2]/1000);
}
/**************************************************************************//**
 * \brief Calculate roll angle (phi) from accelerometer data
 * \param[out] Return roll
 *****************************************************************************/
void calculateRoll( )
{
  /* AN4248: Eq. 13 */
  RwEst[0] = atan2( Accel[1], Accel[2] );

  /* AN3461: Eq. 37 */
  //    double den = sqrt( ( ( this.imu.accel[1] * this.imu.accel[1] ) + ( this.imu.accel[2] * this.imu.accel[2] ) ) );
  //    this.imu.roll = atan2( -this.imu.accel[0], den );
}

/**************************************************************************//**
 * \brief Calculate pitch angle (theta) from accelerometer data
 * \param[out] Return pitch
 *****************************************************************************/
void calculatePitch( )
{
  /* AN4248: Eq. 14 */
  float den = ( ( Accel[1] * sin( RwEst[0] ) )  + ( Accel[2] * cos( RwEst[0] ) ) );
  RwEst[1] = atan2( -Accel[0], den );

  /* AN3461: Eq. 38 */
  //int s = Accel[2] > 0 ? 1:-1;
  //float MU = 0.01;
  //float den = s * sqrt( ( ( Accel[2] * Accel[2] ) + ( MU * ( Accel[0] * Accel[0] ) ) ) );
  //RwEst[1] = atan2( Accel[1], den );
}

/**************************************************************************//**
 * \brief Calculate yaw angle (psi) from magnetometer data, pitch, and roll
 * \param[out] Return yaw
 *****************************************************************************/
void calculateYaw( )
{
  /* AN4248: Eq. 22 */
  float sin_phi   = sin( RwEst[0] );
  float sin_theta = sin( RwEst[1] );
  float cos_phi   = cos( RwEst[0] );
  float cos_theta = cos( RwEst[1] );
  float num = ( Mag[2] * sin_phi ) - ( Mag[1] * cos_phi );
  float den = ( Mag[0] * cos_theta ) + ( Mag[1] * ( sin_theta * sin_phi ) ) + ( Mag[2] * ( sin_theta * cos_phi ) );
  RwEst[2] = atan2( -num, den );
  RwEst[2] *= -1;
}