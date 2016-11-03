//norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
//norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
//double theta    = atan2( norm_c.y, norm_c.x );
//double r_n      = sqrt( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) );
//double r        = ( sqrt( - ( RAD_NORM_F * RAD_NORM_F ) * ( r_n * r_n ) + ( 2 * RAD_NORM_F) * r_n + 1 ) + 2 * r_n - 1 ) / 2;
//deproj_c.x      = r * cos( theta );
//deproj_c.y      = r * sin( theta );
//final_c.x       = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
//final_c.y       = ( ( deproj_c.y + 1 ) * image_height ) / 2;
//img_c->x        = final_c.x;
//img_c->y        = final_c.y;

//coord norm_c;
//coord deproj_c;
//coord final_c;
//
//norm_c.x        = ( ( 2 * img_c->x ) / image_width  ) - 1;
//norm_c.y        = ( ( 2 * img_c->y ) / image_height ) - 1;
//double d_std    = 2 * ( ( norm_c.x * norm_c.x ) + ( norm_c.y * norm_c.y ) );
//double d_inv    =  ( d_std == 0 ) ? 0.00001 : ( 1 / d_std );
//double d_srt    = sqrt( d_std );
//double deproj   = sqrt( d_inv + sqrt( d_inv - ( 2 * d_std ) + ( ( SQRT_32 * d_srt ) - 4 ) ) );
//deproj_c.x      = norm_c.x * deproj;
//deproj_c.y      = norm_c.y * deproj;
//final_c.x       = ( ( deproj_c.x + 1 ) * image_width  ) / 2;
//final_c.y       = ( ( deproj_c.y + 1 ) * image_height ) / 2;
//img_c->x        = final_c.x;
//img_c->y        = final_c.y;