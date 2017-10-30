/**
 *******************************************************************************
 **	�t�@�C���� : balancer_param.c
 **
 **	�T�v       : �|���U�q����p�����[�^
 **
 ** ���L       : �|���U�q����p�����[�^�͐�������ɑ傫�ȉe����^���܂��B
 **
 *******************************************************************************
 **/

/*============================================================================
 * �f�[�^��`
 *===========================================================================*/
float A_D = 0.8F;	/* ���[�p�X�t�B���^�W��(���E�ԗւ̕��ω�]�p�x�p) */
float A_R = 0.996F;	/* ���[�p�X�t�B���^�W��(���E�ԗւ̖ڕW���ω�]�p�x�p) */

/* ��ԃt�B�[�h�o�b�N�W��
 * K_F[0]: �ԗ։�]�p�x�W��
 * K_F[1]: �ԑ̌X�Ίp�x�W��
 * K_F[2]: �ԗ։�]�p���x�W��
 * K_F[3]: �ԑ̌X�Ίp���x�W��
 */
float K_F[4] = {-0.870303F, -31.9978F, -1.1566F*0.6, -2.78873F};
float K_I = -0.44721F;   /* �T�[�{����p�ϕ��t�B�[�h�o�b�N�W�� */

float K_PHIDOT = 25.0F*2.5F; /* �ԑ̖ڕW����p���x�W�� */
float K_THETADOT = 7.5F; /* ���[�^�ڕW��]�p���x�W�� */

const float BATTERY_GAIN = 0.001089F;	/* PWM�o�͎Z�o�p�o�b�e���d���␳�W�� */
const float BATTERY_OFFSET = 0.625F;	/* PWM�o�͎Z�o�p�o�b�e���d���␳�I�t�Z�b�g */

/******************************** END OF FILE ********************************/
