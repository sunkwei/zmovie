#pragma once

#ifdef __cplusplus
extern "C" {
#endif // c++

typedef struct uv_angles
{
	double u_angle;
	double v_angle;
} Angles, *LpAngles;

typedef struct angles_f
{
	Angles als;
	double f;
} AnglesF;

typedef struct two_angles_f
{
	Angles first_als;
	Angles second_als;
	double f;
} TwoAnglesF;

typedef struct ptz_t ptz_t;

typedef struct PtzParams
{
    double f; // 焦距.
    double ccd_size_width, ccd_size_height;	// cmos 大小.
    double pan_min_angle, tilt_min_angle;	// 水平，竖直方向最小角.
	double pan_max_va, tilt_max_va;		// 水平，竖直方向最大视角（1倍时), 其他倍率，直接使用该值除以倍率即可 ..
} PtzParams;

/** 打开云台设备，指定串口名字，和云台地址.
	
        ptz_open("http://127.0.0.1:10003$teacher", 0); // 通过 http interface ...
 */
ptz_t *ptz_open(const char *serial_name, int addr);
void ptz_close(ptz_t *p);


/** 初始化云台，此时指向正前方，倍率为1
 */
int ptz_reset(ptz_t *p);
int ptz_reset2(ptz_t *p, int h, int v, int move_speed, int zoom_speed);

/** 控制云台方向，调用 ptz_stop() 停止转动.
	
        speed 为速度，1最慢，18最快.

 */
int ptz_left(ptz_t *p, int speed);
int ptz_right(ptz_t *p, int speed);
int ptz_up(ptz_t *p, int speed);
int ptz_down(ptz_t *p, int speed);
int ptz_upleft(ptz_t *p, int speed);
int ptz_upright(ptz_t *p, int speed);
int ptz_downleft(ptz_t *p, int speed);
int ptz_downright(ptz_t *p, int speed);
int ptz_stop(ptz_t *p);

int ptz_preset_call(ptz_t *p, int idx);
int ptz_preset_save(ptz_t *p, int idx);
int ptz_preset_clr(ptz_t *p, int idx);

/** 控制倍率.
 */
int ptz_zoom_set(ptz_t *p, int v);	// 设置倍率为 v
int ptz_zoom_set_blocked(ptz_t *p, int v);
int ptz_zoom_get(ptz_t *p);			// 返回当前倍率.
int ptz_zoom_tele(ptz_t *p, int speed);	// 拉近镜头，使用 speed 速度（0.. 7]
int ptz_zoom_wide(ptz_t *p, int speed);	// 推远镜，使用 speed 速度（0..7]
int ptz_zoom_stop(ptz_t *p);		// 停止 tele/wide 的动作.

/** 返回当前指向位置:

		h 为水平位置: -880 .. 880 
		v 为上下位置：-300 .. 300
 */
int ptz_get_current_position(ptz_t *p, int *h, int *v);

/** 设置当前相对位置:

		h, v

	使用 speed 指定的速度， [1..14]
 */
int ptz_set_relative_position(ptz_t *p, int h, int v, int speed);
int ptz_set_absolute_position(ptz_t *p, int h, int v, int speed);
int ptz_set_absolute_position_blocked(ptz_t *p, int h, int v, int speed);
int a_ptz_set_absolute_position(ptz_t *p, int h, int v, int speed);

/** 返回当前视角.
 */
int ptz_get_view_angle(ptz_t *p, int *h, int *v);

/** 同时得到 zoom value 和 position，这样为了减少网络传输 ... */
int ptz_get_current_position_and_zoom(ptz_t *p, int *h, int *v, int *z);

/** 将到中心的偏移为 h, v 的点，不变倍的情况下，移动到中心.

    @warning: 使用图像中心为原点，注意 hr, vr 的正负值，hr, vr 为到中心距离的比值.
			如：图像尺寸为 1920x1080

                若希望将坐标 （100, 100）移动到中心，需要.
					center = (960, 540)
					hr = (100 - 960) / 960;
					vr = (100 - 540) / 540;
 */
int ptz_ext_location_center(ptz_t *p, double hr, double vr, int speed, AnglesF *alf);

/** 类似 ptz_ext_location_center，但到达指定位置.
 */
int ptz_ext_location_center_offset(ptz_t *p, double hr_src, double vr_src, double hr_dst, double vr_dst, int speed, TwoAnglesF *TA);

/** 根据zoom value得到图像尺寸倍率.
*/
double ptz_zoom_ratio_of_value(double v);

/**获取当前镜头的焦距.
*/
double ptz_get_f( ptz_t *p);

double ptz_get_picture_scale( ptz_t *p);

double ptz_get_scales(ptz_t *p);

#ifdef __cplusplus
}
#endif // c++
