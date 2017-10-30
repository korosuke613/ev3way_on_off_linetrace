/**
 ******************************************************************************
 **	�t�@�C���� : balancer_private.h
 **
 ** ���f���֘A���:
 **   ���f����   : balancer.mdl
 **   �o�[�W���� : 1.893
 **   ����       : y_yama - Tue Sep 25 11:37:09 2007
 **                takashic - Sun Sep 28 17:50:53 2008
 **
 ** Copyright (c) 2009-2016 MathWorks, Inc.
 ** All rights reserved.
 ******************************************************************************
 **/

/* Imported (extern) block parameters */
extern float A_D;                   /* Variable: A_D
                                        * Referenced by blocks:
                                        * '<S11>/Constant1'
                                        * '<S11>/Gain2'
                                        * ���[�p�X�t�B���^�W��(���E�ԗւ̕��ω�]�p�x�p)
                                        */
extern float A_R;                   /* Variable: A_R
                                        * Referenced by blocks:
                                        * '<S8>/Constant1'
                                        * '<S8>/Gain2'
                                        * ���[�p�X�t�B���^�W��(���E�ԗւ̖ڕW���ω�]�p�x�p)
                                        */
extern float K_F[4];                /* Variable: K_F
                                        * '<S1>/FeedbackGain'
                                        * �T�[�{����p��ԃt�B�[�h�o�b�N�W��
                                        */
extern float K_I;                   /* Variable: K_I
                                        * '<S1>/IntegralGain'
                                        * �T�[�{����p�ϕ��W��
                                        */
extern float K_PHIDOT;              /* Variable: K_PHIDOT
                                        * '<S3>/Gain2'
                                        * �ԑ̖̂ڕW���ʉ�]���x(d��/dt)�W��
                                        */
extern float K_THETADOT;            /* Variable: K_THETADOT
                                        * '<S3>/Gain1'
                                        * ���E�ԗւ̕��ω�]���x(d��/dt)�W��
                                        */
extern const float BATTERY_GAIN;    /* PWM�o�͎Z�o�p�o�b�e���d���␳�W�� */
extern const float BATTERY_OFFSET;  /* PWM�o�͎Z�o�p�o�b�e���d���␳�I�t�Z�b�g */

/*======================== TOOL VERSION INFORMATION ==========================*
 * MATLAB 7.7 (R2008b)30-Jun-2008                                             *
 * Simulink 7.2 (R2008b)30-Jun-2008                                           *
 * Real-Time Workshop 7.2 (R2008b)30-Jun-2008                                 *
 * Real-Time Workshop Embedded Coder 5.2 (R2008b)30-Jun-2008                  *
 * Stateflow 7.2 (R2008b)30-Jun-2008                                          *
 * Stateflow Coder 7.2 (R2008b)30-Jun-2008                                    *
 * Simulink Fixed Point 6.0 (R2008b)30-Jun-2008                               *
 *============================================================================*/

/*======================= LICENSE IN USE INFORMATION =========================*
 * matlab                                                                     *
 * real-time_workshop                                                         *
 * rtw_embedded_coder                                                         *
 * simulink                                                                   *
 *============================================================================*/
/******************************** END OF FILE ********************************/
