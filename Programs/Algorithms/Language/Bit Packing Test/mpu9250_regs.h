/***********************************************************************************************//**
 * \file   mpu9250_regs.h
 * \brief  MPU9250 registers and defaults
 ***************************************************************************************************
 *      Author: Matthew Fonken
 **************************************************************************************************/


#ifndef mpu9250_regs_h
#define mpu9250_regs_h

#include <stdint.h>
#include <stdbool.h>

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup imu
 * @{
 **************************************************************************************************/

/***************************************************************************************************
 Device Addresses
 **************************************************************************************************/
#define IMU_ADDR                0x68
#define MAG_ADDR                0x0C

/***************************************************************************************************
 Accel and Gyro Registers
 **************************************************************************************************/
#define SELF_TEST_X_GYRO        0x00
#define SELF_TEST_Y_GYRO        0x01
#define SELF_TEST_Z_GYRO        0x02
#define SELF_TEST_X_ACCEL       0x0D
#define SELF_TEST_Y_ACCEL       0x0E
#define SELF_TEST_Z_ACCEL       0x0F
#define XG_OFFSET_H             0x13
#define XG_OFFSET_L             0x14
#define YG_OFFSET_H             0x15
#define YG_OFFSET_L             0x16
#define ZG_OFFSET_H             0x17
#define ZG_OFFSET_L             0x18
#define SMPLRT_DIV              0x19
#define CONFIG                  0x1A
#define GYRO_CONFIG             0x1B
#define ACCEL_CONFIG            0x1C
#define ACCEL_CONFIG2           0x1D
#define LP_ACCEL_ODR            0x1E
#define WOM_THR                 0x1F
#define FIFO_EN                 0x23
#define I2C_MST_CTRL            0x24
#define I2C_SLV0_ADDR           0x25
#define I2C_SLV0_REG            0x26
#define I2C_SLV0_CTRL           0x27
#define I2C_SLV1_ADDR           0x28
#define I2C_SLV1_REG            0x29
#define I2C_SLV1_CTRL           0x2A
#define I2C_SLV2_ADDR           0x2B
#define I2C_SLV2_REG            0x2C
#define I2C_SLV2_CTRL           0x2D
#define I2C_SLV3_ADDR           0x2E
#define I2C_SLV3_REG            0x2F
#define I2C_SLV3_CTRL           0x30
#define I2C_SLV4_ADDR           0x31
#define I2C_SLV4_REG            0x32
#define I2C_SLV4_DO             0x33
#define I2C_SLV4_CTRL           0x34
#define I2C_SLV4_DI             0x35
#define I2C_MST_STATUS          0x36
#define INT_PIN_CFG             0x37
#define INT_ENABLE              0x38
#define INT_STATUS              0x3A
#define ACCEL_XOUT_H            0x3B
#define ACCEL_XOUT_L            0x3C
#define ACCEL_YOUT_H            0x3D
#define ACCEL_YOUT_L            0x3E
#define ACCEL_ZOUT_H            0x3F
#define ACCEL_ZOUT_L            0x40
#define TEMP_OUT_H              0x41
#define TEMP_OUT_L              0x42
#define GYRO_XOUT_H             0x43
#define GYRO_XOUT_L             0x44
#define GYRO_YOUT_H             0x45
#define GYRO_YOUT_L             0x46
#define GYRO_ZOUT_H             0x47
#define GYRO_ZOUT_L             0x48
#define EXT_SENS_DATA_00        0x49
#define EXT_SENS_DATA_01        0x4A
#define EXT_SENS_DATA_02        0x4B
#define EXT_SENS_DATA_03        0x4C
#define EXT_SENS_DATA_04        0x4D
#define EXT_SENS_DATA_05        0x4E
#define EXT_SENS_DATA_06        0x4F
#define EXT_SENS_DATA_07        0x50
#define EXT_SENS_DATA_08        0x51
#define EXT_SENS_DATA_09        0x52
#define EXT_SENS_DATA_10        0x53
#define EXT_SENS_DATA_11        0x54
#define EXT_SENS_DATA_12        0x55
#define EXT_SENS_DATA_13        0x56
#define EXT_SENS_DATA_14        0x57
#define EXT_SENS_DATA_15        0x58
#define EXT_SENS_DATA_16        0x59
#define EXT_SENS_DATA_17        0x5A
#define EXT_SENS_DATA_18        0x5B
#define EXT_SENS_DATA_19        0x5C
#define EXT_SENS_DATA_20        0x5D
#define EXT_SENS_DATA_21        0x5E
#define EXT_SENS_DATA_22        0x5F
#define EXT_SENS_DATA_23        0x60
#define I2C_SLV0_DO             0x63
#define I2C_SLV1_DO             0x64
#define I2C_SLV2_DO             0x65
#define I2C_SLV3_DO             0x66
#define I2C_MST_DELAY_CTRL      0x67
#define SIGNAL_PATH_RESET       0x68
#define MOT_DETECT_CTRL         0x69
#define USER_CTRL               0x6A
#define PWR_MGMT_1              0x6B
#define PWR_MGMT_2              0x6C
#define FIFO_COUNTH             0x72
#define FIFO_COUNTL             0x73
#define FIFO_R_W                0x74
#define WHO_AM_I                0x75
#define XA_OFFSET_H             0x77
#define XA_OFFSET_L             0x78
#define YA_OFFSET_H             0x7A
#define YA_OFFSET_L             0x7B
#define ZA_OFFSET_H             0x7D
#define ZA_OFFSET_L             0x7E


/***************************************************************************************************
 Magnometer Registers
 **************************************************************************************************/
#define MAG_WIA                 0x00
#define MAG_INFO                0x01
#define MAG_ST1                 0x02
#define MAG_XOUT_L              0x03
#define MAG_XOUT_H              0x04
#define MAG_YOUT_L              0x05
#define MAG_YOUT_H              0x06
#define MAG_ZOUT_L              0x07
#define MAG_ZOUT_H              0x08
#define MAG_ST2                 0x09
#define MAG_CNTL                0x0A
#define MAG_RSV                 0x0B
#define MAG_ASTC                0x0C
#define MAG_TS1                 0x0D
#define MAG_TS2                 0x0E
#define MAG_I2CDIS              0x0F
#define MAG_ASAX                0x10
#define MAG_ASAY                0x11
#define MAG_ASAZ                0x12

#define MAG_BIAS_X               470
#define MAG_BIAS_Y               120
#define MAG_BIAS_Z               125

//Magnetometer register masks
#define WIA_MASK 0x48

/***************************************************************************************************
 Local Types and Definitions
 **************************************************************************************************/

/* System Value */
#define IMU_ID                  0x71

/* Biases */
#define ACCEL_BIAS_X               0
#define ACCEL_BIAS_Y               0
#define ACCEL_BIAS_Z               0
#define GYRO_BIAS_X                0
#define GYRO_BIAS_Y                0
#define GYRO_BIAS_Z                0
#define MAG_BIAS_X               470
#define MAG_BIAS_Y               120
#define MAG_BIAS_Z               125

/* FIFO Modes */
#define FIFO_MODE_NO_OVWR       1
#define FIFO_MODE_OVWR          0

/* External FSYNC Modes */
#define EXT_FSYNC_MODE_DIS      0
#define EXT_FSYNC_MODE_TEMP     1
#define EXT_FSYNC_MODE_GYRO_X   2
#define EXT_FSYNC_MODE_GYRO_Y   3
#define EXT_FSYNC_MODE_GYRO_Z   4
#define EXT_FSYNC_MODE_ACCEL_X  5
#define EXT_FSYNC_MODE_ACCEL_Y  6
#define EXT_FSYNC_MODE_ACCEL_Z  7

/* Universal Register Commands */
#define ENABLE  1
#define DISABLE 0
#define LOW             1
#define HIGH            0

/* Register Commands */
#define OPEN_DRAIN      1
#define PUSH_PULL       0
#define UNTIL_STATUS    1
#define PULSE_50US      0
#define CLEAR_ON_RD     1
#define CLEAR_ON_ST_REG 0
#define FSYNC_INT_EN    1
#define FSYNC_INT_DIS   0

/* Clock Select Options */
#define CLKSEL_INTERNAL_20MHZ    0
#define CLKSEL_BEST_AVAILABLE    1
#define CLKSEL_STOP              7

/***************************************************************************************************
 Gyro Types and Definitions
 **************************************************************************************************/
/* General Config Type */
typedef struct
{
    uint8_t dlpfConfig  :3;                 /**< Digital low pass filter configuration */
    uint8_t fsyncMode   :3;                 /**< FSYNC latch mode                      */
    uint8_t fifoMode    :1;                 /**< FIFO write mode configuration         */
    uint8_t RESERVED    :1;
} mpu9250_gen_config_t;

/* Gyro fchoice option to enable normal fs bandwidth options */
#define GYRO_BW_ENABLE  0x00

/* Gyro fchoice mask to disable dlpf_config options */
#define GYRO_BW_MASK    0x03

/* Gyro bandwidth options set through its DLPF */
enum mpu9250_gyro_bandwidth
{
    GYRO_BANDWIDTH_8800HZ_HIGH_FS   = 0xFD, /**< 8.8kHz Bandwidth with 32kHs Fs */
    GYRO_BANDWIDTH_3600HZ_HIGH_FS   = 0xFE, /**< 3.6kHz Bandwidth with 32kHs Fs */
    GYRO_BANDWIDTH_3600HZ           = 0x07, /**< 3.6kHz Bandwidth with 8kHs Fs  */
    GYRO_BANDWIDTH_250HZ            = 0x00, /**<  250Hz Bandwidth with 1kHs Fs  */
    GYRO_BANDWIDTH_184HZ            = 0x01, /**<  184Hz Bandwidth with 1kHs Fs  */
    GYRO_BANDWIDTH_92HZ             = 0x02, /**<   92Hz Bandwidth with 1kHs Fs  */
    GYRO_BANDWIDTH_41HZ             = 0x03, /**<   41Hz Bandwidth with 1kHs Fs  */
    GYRO_BANDWIDTH_20HZ             = 0x04, /**<   20Hz Bandwidth with 1kHs Fs  */
    GYRO_BANDWIDTH_10HZ             = 0x05, /**<   10Hz Bandwidth with 1kHs Fs  */
    GYRO_BANDWIDTH_5HZ              = 0x06, /**<    5Hz Bandwidth with 1kHs Fs  */
};

/* Enables all X(0x01), Y(0x02), and Z(0x04) gyros */
#define GYRO_EN     7

/* Gyro Config Type */
typedef struct
{
    uint8_t fchoice     :2;                 /**< Fchoice config for gyro         */
    uint8_t RESERVED    :1;
    uint8_t fscale      :2;                 /**< Fscale config for gyro          */
    uint8_t enable      :3;                 /**< Enable config for gyro channels */
} mpu9250_gyro_config_t;

/* Gyro Scales */
#define GYRO_FULL_SCALE_250DPS  0
#define GYRO_FULL_SCALE_500DPS  1
#define GYRO_FULL_SCALE_1000DPS 2
#define GYRO_FULL_SCALE_2000DPS 3

/***************************************************************************************************
 Accelerometer Types and Definitions
 **************************************************************************************************/

/* Accel bandwidth options set through its DLPF */
enum mpu9250_accel_bandwidth
{
    ACCEL_BANDWIDTH_113KHZ          = 0xFF, /**< 113kHz Bandwith with 4kHs Rate */
    ACCEL_BANDWIDTH_460HZ           = 0x00, /**<  460Hz Bandwith with 1kHs Rate */
    ACCEL_BANDWIDTH_184HZ           = 0x01, /**<  184Hz Bandwith with 1kHs Rate */
    ACCEL_BANDWIDTH_92HZ            = 0x02, /**<   92Hz Bandwith with 1kHs Rate */
    ACCEL_BANDWIDTH_41HZ            = 0x03, /**<   41Hz Bandwith with 1kHs Rate */
    ACCEL_BANDWIDTH_20HZ            = 0x04, /**<   20Hz Bandwith with 1kHs Rate */
    ACCEL_BANDWIDTH_10HZ            = 0x05, /**<   10Hz Bandwith with 1kHs Rate */
    ACCEL_BANDWIDTH_5HZ             = 0x06, /**<    5Hz Bandwith with 1kHs Rate */
};

/* Enables all X(0x01), Y(0x02), and Z(0x04) accelerometers */
#define ACCEL_EN    7

/* Accel Config Type */
typedef struct
{
    uint16_t dlpfConfig  :3;                 /**< Digital low pass filter configuration */
    uint16_t fchoice     :1;                 /**< Fchoice config for accel              */
    uint16_t RESERVED    :7;
    uint16_t fscale      :2;                 /**< Fscale config for accel               */
    uint16_t enable      :3;                 /**< Enable config for accel channels      */
} mpu9250_accel_config_t;

/* Accel Scales */
#define ACCEL_FULL_SCALE_2G     0
#define ACCEL_FULL_SCALE_4G     1
#define ACCEL_FULL_SCALE_8G     2
#define ACCEL_FULL_SCALE_16G    3
#define ACCEL_INTEL_EN_LOC      7
#define ACCEL_INTEL_MODE_LOC    6

/* Accel Motion Interrupt Config */
#define ACCEL_INTEL_EN   DISABLE
#define ACCEL_INTEL_MODE DISABLE
#define ACCEL_INTEL ( ACCEL_INTEL_EN   << ACCEL_INTEL_EN_LOC   || \
                      ACCEL_INTEL_MODE << ACCEL_INTEL_MODE_LOC )
/* Accel Motion Register Type */
typedef struct
{
    uint8_t RESERVED    :6;
    uint8_t intel_mode  :1;                 /**< Accel motion interrupt mode   */
    uint8_t intel_en    :1;                 /**< Accel motion interrupt enable */
} mpu9250_accel_interrupt_t;

/* Low-Power Accel Output Data Rate Options */
enum mpu9250_accel_lp_odr
{
    ACCEL_LP_ODR__240HZ             =  0,   /**<  0.24Hz low-power accel output frequency */
    ACCEL_LP_ODR__490HZ             =  1,   /**<  0.49Hz low-power accel output frequency */
    ACCEL_LP_ODR__980HZ             =  2,   /**<  0.98Hz low-power accel output frequency */
    ACCEL_LP_ODR_1_950HZ            =  3,   /**<  1.95Hz low-power accel output frequency */
    ACCEL_LP_ODR_3_910HZ            =  4,   /**<  3.91Hz low-power accel output frequency */
    ACCEL_LP_ODR_7_810HZ            =  5,   /**<  7.81Hz low-power accel output frequency */
    ACCEL_LP_ODR_15_630HZ           =  6,   /**< 15.63Hz low-power accel output frequency */
    ACCEL_LP_ODR_31_250HZ           =  7,   /**< 31.25Hz low-power accel output frequency */
    ACCEL_LP_ODR_62_500HZ           =  8,   /**< 62.50Hz low-power accel output frequency */
    ACCEL_LP_ODR_125HZ              =  9,   /**<   125Hz low-power accel output frequency */
    ACCEL_LP_ODR_250HZ              = 10,   /**<   250Hz low-power accel output frequency */
    ACCEL_LP_ODR_500HZ              = 11,   /**<   500Hz low-power accel output frequency */
};

/* Low-Power Accel Output Data Rate Register Type */
typedef struct
{
    uint8_t lposc_clsel :4;                 /**< Low-power oscillator clock select */
    uint8_t RESERVED    :4;
} mpu9250_accel_lp_odr_t;

/***************************************************************************************************
 FIFO Types and Definitions
 **************************************************************************************************/

/* FIFO Enable Type */
typedef struct
{
    uint8_t slv_0       :1;                 /**< FIFO Enable I2C slave 0 */
    uint8_t slv_1       :1;                 /**< FIFO Enable I2C slave 1 */
    uint8_t slv_2       :1;                 /**< FIFO Enable I2C slave 2 */
    uint8_t accel       :1;                 /**< FIFO Enable Accel       */
    uint8_t gyro_z      :1;                 /**< FIFO Enable Gyro Z      */
    uint8_t gyro_y      :1;                 /**< FIFO Enable Gyro Y      */
    uint8_t gyro_x      :1;                 /**< FIFO Enable Gyro X      */
    uint8_t temp        :1;                 /**< FIFO Enable Temperature */
} mpu9250_fifo_enable_t;

/* FIFO Enable Register Locations */
#define FIFO_ENABLE_TEMP_LOC    7
#define FIFO_ENABLE_GYRO_X_LOC  6
#define FIFO_ENABLE_GYRO_Y_LOC  5
#define FIFO_ENABLE_GYRO_Z_LOC  4
#define FIFO_ENABLE_ACCEL_LOC   3
#define FIFO_ENABLE_SLV_2_LOC   2
#define FIFO_ENABLE_SLV_1_LOC   1
#define FIFO_ENABLE_SLV_0_LOC   0

/* FIFO Enable Register Config */
#define FIFO_ENABLE_TEMP        false
#define FIFO_ENABLE_GYRO_X      false
#define FIFO_ENABLE_GYRO_Y      false
#define FIFO_ENABLE_GYRO_Z      false
#define FIFO_ENABLE_ACCEL       false
#define FIFO_ENABLE_SLV_2       false
#define FIFO_ENABLE_SLV_1       false
#define FIFO_ENABLE_SLV_0       false

/* FIFO Enable Register Define Config */
#define FIFO_ENABLE   ( FIFO_ENABLE_TEMP   << FIFO_ENABLE_TEMP_LOC   || \
                        FIFO_ENABLE_GYRO_X << FIFO_ENABLE_GYRO_X_LOC || \
                        FIFO_ENABLE_GYRO_Y << FIFO_ENABLE_GYRO_Y_LOC || \
                        FIFO_ENABLE_GYRO_Z << FIFO_ENABLE_GYRO_Z_LOC || \
                        FIFO_ENABLE_ACCEL  << FIFO_ENABLE_ACCEL_LOC  || \
                        FIFO_ENABLE_SLV_2  << FIFO_ENABLE_SLV_2_LOC  || \
                        FIFO_ENABLE_SLV_1  << FIFO_ENABLE_SLV_2_LOC  || \
                        FIFO_ENABLE_SLV_0  << FIFO_ENABLE_SLV_0_LOC  )

/***************************************************************************************************
 I2C Master Types and Definitions
 **************************************************************************************************/

/* I2C Master Clock Options */
enum mpu9250_i2c_master_clk
{
    I2C_MCLK_348KHZ                 =  0,   /**<  348kHz I2C master clock frequency */
    I2C_MCLK_333KHZ                 =  1,   /**<  333kHz I2C master clock frequency */
    I2C_MCLK_320KHZ                 =  2,   /**<  320kHz I2C master clock frequency */
    I2C_MCLK_308KHZ                 =  3,   /**<  308kHz I2C master clock frequency */
    I2C_MCLK_296KHZ                 =  4,   /**<  296kHz I2C master clock frequency */
    I2C_MCLK_286KHZ                 =  5,   /**<  286kHz I2C master clock frequency */
    I2C_MCLK_376KHZ                 =  6,   /**<  276kHz I2C master clock frequency */
    I2C_MCLK_267KHZ                 =  7,   /**<  267kHz I2C master clock frequency */
    I2C_MCLK_258KHZ                 =  8,   /**<  258kHz I2C master clock frequency */
    I2C_MCLK_500KHZ                 =  9,   /**<  500kHz I2C master clock frequency */
    I2C_MCLK_471KHZ                 = 10,   /**<  471kHz I2C master clock frequency */
    I2C_MCLK_444KHZ                 = 11,   /**<  444kHz I2C master clock frequency */
    I2C_MCLK_421KHZ                 = 12,   /**<  421kHz I2C master clock frequency */
    I2C_MCLK_400KHZ                 = 13,   /**<  400kHz I2C master clock frequency */
    I2C_MCLK_381KHZ                 = 14,   /**<  381kHz I2C master clock frequency */
    I2C_MCLK_364KHZ                 = 15,   /**<  364kHz I2C master clock frequency */
};

/* I2C Master Control Type */
typedef struct
{
    uint8_t m_clk       :4;             /**< I2C master clock frequency select */
    uint8_t m_p_nsr     :1;             /**< Transition between slave reads    */
    uint8_t s3_fifo_en  :1;             /**< Write SLV_3 registers to FIFO     */
    uint8_t wait_for_es :1;             /**< Wait for external sensor          */
    uint8_t mult_m_en   :1;             /**< Enable multi-master capability    */
} mpu9250_i2c_m_ctrl_t;

/***************************************************************************************************
 Interrupt Types and Definitions
 **************************************************************************************************/

/* INT Pin Config Type */
typedef struct
{
    uint8_t RESERVED    :1;
    uint8_t bypass_en   :1;             /**< Set I2C master pins to "bypass mode"   */
    uint8_t fsync_md_en :1;             /**< Enable FSYNC pin to be an interrupt    */
    uint8_t actl_fsync  :1;             /**< Acitve level of FSYNC pin as interrupt */
    uint8_t int_anyrd_c :1;             /**< Clear interrupt status option select   */
    uint8_t ltch_int_en :1;             /**< Latch INT pin option select            */
    uint8_t open        :1;             /**< Config INT pin as open or push-pull    */
    uint8_t actl        :1;             /**< Config INT pin as active low or high   */
} mpu9250_int_pin_config_t;

/* INT PIN Register Locations */
#define INT_PIN_ACTL_LOC     7
#define INT_PIN_OPEN_LOC     6
#define INT_PIN_LTCH_EN_LOC  5
#define INT_PIN_ANYRD_C_LOC  4
#define INT_PIN_ACTL_FS_LOC  3
#define INT_PIN_FSYNC_EN_LOC 2
#define INT_PIN_BYPS_EN_LOC  1

/* INT PIN Register Config */
#define INT_PIN_ACTL         HIGH
#define INT_PIN_OPEN         PUSH_PULL
#define INT_PIN_LTCH_EN      PULSE_50US
#define INT_PIN_ANYRD_C      CLEAR_ON_ST_REG
#define INT_PIN_ACTL_FS      HIGH
#define INT_PIN_FSYNC_EN     FSYNC_INT_DIS
#define INT_PIN_BYPS_EN      HIGH

/* INT PIN Register Define Config */
#define INT_PIN_CONFIG ( INT_PIN_ACTL     << INT_PIN_ACTL_LOC     || \
                         INT_PIN_OPEN     << INT_PIN_OPEN_LOC     || \
                         INT_PIN_LTCH_EN  << INT_PIN_LTCH_EN_LOC  || \
                         INT_PIN_ANYRD_C  << INT_PIN_ANYRD_C_LOC  || \
                         INT_PIN_ACTL_FS  << INT_PIN_ACTL_FS_LOC  || \
                         INT_PIN_FSYNC_EN << INT_PIN_FSYNC_EN_LOC || \
                         INT_PIN_BYPS_EN  << INT_PIN_BYPS_EN_LOC  )

/* Interrupt Enable Type */
typedef struct
{
    uint8_t raw_rdy_en  :1;             /**< Enable raw sensor data ready interrupt */
    uint8_t RESERVED1   :2;
    uint8_t fsync_int_en :1;            /**< Enable FSYNC interrupt                 */
    uint8_t fifo_ovfw_en :1;            /**< Enable FIFO overflow interrupt         */
    uint8_t RESERVED2   :1;
    uint8_t wom_en      :1;             /**< Enable wake on motion interrupt        */
    uint8_t RESERVED3   :1;
} mpu9250_int_enable_t;

/* INT Enable Register Locations */
#define INT_ENABLE_WOM_EN_LOC       6
#define INT_ENABLE_FIFO_OVFL_EN_LOC 5
#define INT_ENABLE_FSYNC_INT_EN_LOC 4
#define INT_ENABLE_RAW_RDY_EN_LOC   0

/* INT Enable Register Config */
#define INT_ENABLE_WOM_EN       DISABLE
#define INT_ENABLE_FIFO_OVFL_EN DISABLE
#define INT_ENABLE_FSYNC_INT_EN DISABLE
#define INT_ENABLE_RAW_RDY_EN   DISABLE

/* INT Enable Register Define Config */
#define INT_ENABLE_CONFIG ( INT_ENABLE_WOM_EN       << INT_ENABLE_WOM_EN_LOC       || \
                            INT_ENABLE_FIFO_OVFL_EN << INT_ENABLE_FIFO_OVFL_EN_LOC || \
                            INT_ENABLE_FSYNC_INT_EN << INT_ENABLE_FSYNC_INT_EN_LOC || \
                            INT_ENABLE_RAW_RDY_EN   << INT_ENABLE_RAW_RDY_EN_LOC   )

/* Interrupt Status Type */
typedef struct
{
    uint8_t raw_rdy_int :1;             /**< Raw sensor data ready interrupt */
    uint8_t RESERVED1   :2;
    uint8_t fsync_int_int :1;           /**< FSYNC interrupt                 */
    uint8_t fifo_ovfw_int :1;           /**< FIFO overflow interrupt         */
    uint8_t RESERVED2   :1;
    uint8_t wom_int     :1;             /**< Wake on motion interrupt        */
    uint8_t RESERVED3   :1;
} mpu9250_int_status_t;

/* INT Status Register Locations */
#define INT_STATUS_WOM_EN_LOC       6
#define INT_STATUS_FIFO_OVFL_EN_LOC 5
#define INT_STATUS_FSYNC_INT_EN_LOC 4
#define INT_STATUS_RAW_RDY_EN_LOC   0

/* INT Status Register Config */
#define INT_STATUS_WOM_EN       DISABLE
#define INT_STATUS_FIFO_OVFL_EN DISABLE
#define INT_STATUS_FSYNC_INT_EN DISABLE
#define INT_STATUS_RAW_RDY_EN   DISABLE

/* INT Status Register Define Config */
#define INT_STATUS_CONFIG ( INT_STATUS_WOM_EN       << INT_STATUS_WOM_EN_LOC       || \
                            INT_STATUS_FIFO_OVFL_EN << INT_STATUS_FIFO_OVFL_EN_LOC || \
                            INT_STATUS_FSYNC_INT_EN << INT_STATUS_FSYNC_INT_EN_LOC || \
                            INT_STATUS_RAW_RDY_EN   << INT_STATUS_RAW_RDY_EN_LOC   )

/***************************************************************************************************
 User Control Types and Definitions
 **************************************************************************************************/

/* User Control Type */
typedef struct
{
    uint8_t sig_cond_rst :1;            /**< Reset gyro, accel, and temp digital signal paths */
    uint8_t i2c_m_rst   :1;             /**< Reset I2C master module                          */
    uint8_t fifo_rst    :1;             /**< Reset FIFO module                                */
    uint8_t RESERVED1   :1;
    uint8_t i2c_if_dis  :1;             /**< Reset I2C slave module and enter SPI mode only   */
    uint8_t i2c_m_en    :1;             /**< Reset I2C master I/F module                      */
    uint8_t fifo_en     :1;             /**< Enable FIFO operation                            */
    uint8_t RESERVED2   :1;
} mpu9250_usr_control_t;

/* User Config Register Locations */
#define USER_CONFIG_FIFO_EN_LOC     6
#define USER_CONFIG_I2C_MST_EN_LOC  5
#define USER_CONFIG_I2C_IF_DIS_LOC  4
#define USER_CONFIG_FIFO_RST_LOC    2
#define USER_CONFIG_I2C_MST_RST_LOC 1
#define USER_CONFIG_SIGCOND_RST_LOC 0

/* User Config Register Config */
#define USER_CONFIG_FIFO_EN     DISABLE
#define USER_CONFIG_I2C_MST_EN  DISABLE
#define USER_CONFIG_I2C_IF_DIS  DISABLE
#define USER_CONFIG_FIFO_RST    DISABLE
#define USER_CONFIG_I2C_MST_RST DISABLE
#define USER_CONFIG_SIGCOND_RST DISABLE

/* User Config Define Config */
#define USER_CONFIG   ( USER_CONFIG_FIFO_EN     << USER_CONFIG_FIFO_EN_LOC     || \
                        USER_CONFIG_I2C_MST_EN  << USER_CONFIG_I2C_MST_EN_LOC  || \
                        USER_CONFIG_I2C_IF_DIS  << USER_CONFIG_I2C_IF_DIS_LOC  || \
                        USER_CONFIG_FIFO_RST    << USER_CONFIG_FIFO_RST_LOC    || \
                        USER_CONFIG_I2C_MST_RST << USER_CONFIG_I2C_MST_RST_LOC || \
                        USER_CONFIG_SIGCOND_RST << USER_CONFIG_SIGCOND_RST_LOC )

/***************************************************************************************************
 Power Management Types and Definitions
 **************************************************************************************************/

/* Power Management Type */
typedef struct
{
    uint16_t dis_zg      :1;            /**< Disble Z gyro  */
    uint16_t dis_yg      :1;            /**< Disble Y gyro  */
    uint16_t dis_xg      :1;            /**< Disble X gyro  */
    uint16_t dis_za      :1;            /**< Disble Z accel */
    uint16_t dis_ya      :1;            /**< Disble Y accel */
    uint16_t dis_xa      :1;            /**< Disble X accel */
    uint16_t RESERVED    :2;
    uint16_t clk_sel     :3;            /**< Clock source select                                    */
    uint16_t pd_ptat     :1;            /**< Power down internal PTAT voltage generator             */
    uint16_t gyro_stdby  :1;            /**< Enable gyro and pll circuitry, but disable sense paths */
    uint16_t cycle       :1;            /**< Cycle between sleep and sampling at LP_ACCEL_ODR rate  */
    uint16_t sleep       :1;            /**< Set sleep mode */
    uint16_t h_rst       :1;            /**< Hard reset     */
} mpu9250_pwr_mgmt_t;

/* Power Management Register Locations */
#define PWR_MGMT_H_RESET_LOC    15
#define PWR_MGMT_SLEEP_LOC      14
#define PWR_MGMT_CYCLE_LOC      13
#define PWR_MGMT_GYRO_STBY_LOC  12
#define PWR_MGMT_PD_PTAT_LOC    11
#define PWR_MGMT_CLKSEL_LOC      8
#define PWR_MGMT_DISABLE_XA_LOC  5
#define PWR_MGMT_DISABLE_YA_LOC  4
#define PWR_MGMT_DISABLE_ZA_LOC  3
#define PWR_MGMT_DISABLE_XG_LOC  2
#define PWR_MGMT_DISABLE_YG_LOC  1
#define PWR_MGMT_DISABLE_ZG_LOC  0

/* Power Management Register Config */
#define PWR_MGMT_H_RESET    DISABLE
#define PWR_MGMT_SLEEP      DISABLE
#define PWR_MGMT_CYCLE      DISABLE
#define PWR_MGMT_GYRO_STBY  DISABLE
#define PWR_MGMT_PD_PTAT    DISABLE
#define PWR_MGMT_CLKSEL     CLKSEL_INTERNAL_20MHZ
#define PWR_MGMT_DISABLE_XA DISABLE
#define PWR_MGMT_DISABLE_YA DISABLE
#define PWR_MGMT_DISABLE_ZA DISABLE
#define PWR_MGMT_DISABLE_XG DISABLE
#define PWR_MGMT_DISABLE_YG DISABLE
#define PWR_MGMT_DISABLE_ZG DISABLE

/* Power Management Register Define Config */
#define PWR_MGMT_CONFIG ( PWR_MGMT_H_RESET    << PWR_MGMT_H_RESET_LOC    || \
                          PWR_MGMT_SLEEP      << PWR_MGMT_SLEEP_LOC      || \
                          PWR_MGMT_CYCLE      << PWR_MGMT_CYCLE_LOC      || \
                          PWR_MGMT_GYRO_STBY  << PWR_MGMT_GYRO_STBY_LOC  || \
                          PWR_MGMT_PD_PTAT    << PWR_MGMT_PD_PTAT_LOC    || \
                          PWR_MGMT_CLKSEL     << PWR_MGMT_CLKSEL         || \
                          PWR_MGMT_DISABLE_XA << PWR_MGMT_DISABLE_XA_LOC || \
                          PWR_MGMT_DISABLE_YA << PWR_MGMT_DISABLE_YA_LOC || \
                          PWR_MGMT_DISABLE_ZA << PWR_MGMT_DISABLE_ZA_LOC || \
                          PWR_MGMT_DISABLE_XG << PWR_MGMT_DISABLE_XG_LOC || \
                          PWR_MGMT_DISABLE_YG << PWR_MGMT_DISABLE_YG_LOC || \
                          PWR_MGMT_DISABLE_ZG << PWR_MGMT_DISABLE_ZG_LOC )

/***************************************************************************************************
 Universal Types and Definitions
 **************************************************************************************************/

/* Config Type (6 bytes) */
typedef struct
{
    mpu9250_gen_config_t        general;        /**< General config           */
    mpu9250_gyro_config_t       gyro;           /**< Gyro config              */
    mpu9250_accel_config_t      accel;          /**< Accel config             */
    mpu9250_int_pin_config_t    interrupt;      /**< Interrupt config         */
    uint8_t                     PADDING;        /**< Memory Alignment         */
} mpu9250_config_t;

/* Control Type (8 bytes) */
typedef struct
{
    mpu9250_accel_lp_odr_t      accel_lp;       /**< Low-power accel control  */
    mpu9250_accel_interrupt_t   accel_int;      /**< Accel interrupt control  */
    mpu9250_fifo_enable_t       fifo_en;        /**< FIFO enable control      */
    mpu9250_int_enable_t        int_en;         /**< Interrupt enable control */
    mpu9250_int_status_t        int_status;     /**< Interrupt status         */
    mpu9250_usr_control_t       usr_ctrl;       /**< User control             */
    mpu9250_pwr_mgmt_t          pwr_mgmt;       /**< Power management         */
} mpu9250_control_t;

/* Global Type (14 byte) */
typedef struct
{
    mpu9250_config_t            config;         /**< Universal config         */
    mpu9250_control_t           control;        /**< Universal control        */
} mpu9250_global_t;

/* Default Configurations */
#define FIFO_MODE_DEFAULT       FIFO_MODE_OVWR          /**< FIFO mode default                */
#define EXT_FSYNC_MODE_DEFAULT  EXT_FSYNC_MODE_DIS      /**< External FSYNC mode default      */
#define GYRO_BW_DEFAULT         GYRO_BANDWIDTH_3600HZ   /**< Gyro bandwidth default           */
#define GYRO_FS_DEFAULT         GYRO_FULL_SCALE_250DPS  /**< Gyro full scale default          */
#define GYRO_EN_DEFAULT         GYRO_EN                 /**< Gyro enable default              */
#define ACCEL_BW_DEFAULT        ACCEL_BANDWIDTH_460HZ   /**< Accel bandwidth default          */
#define ACCEL_FS_DEFAULT        ACCEL_FULL_SCALE_2G     /**< Accel full scale default         */
#define ACCEL_LP_ODR_DEFAULT    ACCEL_LP_ODR_250HZ      /**< Low-power accel date rate default*/
#define ACCEL_EN_DEFAULT        ACCEL_EN                /**< Accel enable default             */
#define ACCEL_INTEL_DEFAULT     ACCEL_INTEL             /**< Accel interrupt default          */
#define FIFO_ENABLE_DEFAULT     FIFO_ENABLE             /**< FIFO enable default              */
#define INT_PIN_DEFAULT         INT_PIN_CONFIG          /**< INT pin default                  */
#define INT_ENABLE_DEFAULT      INT_ENABLE_CONFIG       /**< Interrupt enable default         */
#define INT_STATUS_DEFAULT      INT_STATUS_CONFIG       /**< Interrupt status default         */
#define USER_CONFIG_DEFAULT     USER_CONFIG             /**< User config default              */
#define PWR_MGMT_DEFAULT        PWR_MGMT_CONFIG         /**< Power Management default         */


/***************************************************************************************************
 Local Functions
 **************************************************************************************************/

/**************************************************************************//**
 * \brief Set register value on MPU9250
 * \param[in] reg Register to access
 * \param[in] val Value to set
 *****************************************************************************/
void mpu9250_SetRegister( uint8_t reg, uint8_t val );

/**************************************************************************//**
 * \brief Get register value from MPU9250
 * \param[out] Return value from register
 * \param[in] reg Register to access
 *****************************************************************************/
uint8_t mpu9250_GetRegister( uint8_t reg );

/**************************************************************************//**
 * \brief Set register value on MPU9250 Magnometer
 * \param[in] reg Register to access
 * \param[in] val Value to set
 *****************************************************************************/
void mpu9250_SetMagRegister( uint8_t reg, uint8_t val );

/**************************************************************************//**
 * \brief Get register value from MPU9250 Magnometer
 * \param[out] Return value from register
 * \param[in] reg Register to access
 *****************************************************************************/
uint8_t mpu9250_GetMagRegister( uint8_t reg );

/**************************************************************************//**
 * \brief Initialize MPU9250 with default values
 *****************************************************************************/
void mpu9250_defaultInit( mpu9250_global_t * );

/**************************************************************************//**
 * \brief Update MPU9250 registers with current global configuration
 *****************************************************************************/
void mpu9250_updateRegisters( mpu9250_global_t * );

#endif /* mpu9250_regs_h */
