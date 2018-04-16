/** Uncertainty of value */
float VALUE_UNCERTAINTY   = 0.001;
    
/** Uncertainty of bias */
float BIAS_UNCERTAINTY    = 0.003;
    
/** Uncertainty of sensor */
float SENSOR_UNCERTAINTY  = 0.03;

class kalman_t
{
    float      K[];               /**< Gain matrix */
    float      P_k[][];          /**< Error covariance matrix */
    float      rate;               /**< Rate */
    float      bias;               /**< Bias */
    float      value;              /**< Value */
    int         timestamp;          /**< Timestamp */
    
    
    kalman_t()
    {
        this.K = new float[2];
        this.P_k = new float[2][2];
        this.rate        = 0;
        this.bias        = 0;
        this.value       = 0;
    }
    kalman_t( float    v )
    {
        this.K = new float[2];
        this.P_k = new float[2][2];
        this.rate        = 0;
        this.bias        = 0;
        this.value       = v;
    }
    
    void update(float    value_new,
                float    rate_new )
    {
      /* =-----= PREDICT =-----= */
      /* Predict values */
      float delta_time = millis() - this.timestamp;
      this.rate       = rate_new - this.bias;
      this.value     += delta_time * this.rate;
      
      /* Predict error covariance */
      float P_k_diag = delta_time * this.P_k[1][1];
      this.P_k[0][0] +=   delta_time * ( ( delta_time * this.P_k[1][1] ) -
                                       this.P_k[0][1] -
                                       this.P_k[1][0] )
                        + VALUE_UNCERTAINTY;
      this.P_k[0][1] -=   P_k_diag;
      this.P_k[1][0] -=   P_k_diag;
      this.P_k[1][1] +=   BIAS_UNCERTAINTY * delta_time;
      
      /* =-----= UPDATE =-----= */
      /* Update values */
      float S      = this.P_k[0][0] + SENSOR_UNCERTAINTY;
      this.K[0]       = this.P_k[0][0] / S;
      this.K[1]       = this.P_k[1][0] / S;
      float delta_value = value_new - this.value;
      this.value     += this.K[0] * delta_value;
      this.bias      += this.K[1] * delta_value;
      
      /* Update error covariance */
      this.P_k[0][0] -= this.K[0] * this.P_k[0][0];
      this.P_k[0][1] -= this.K[0] * this.P_k[0][1];
      this.P_k[1][0] -= this.K[1] * this.P_k[0][0];
      this.P_k[1][1] -= this.K[1] * this.P_k[0][1];
      
      this.timestamp = millis();
    }
};