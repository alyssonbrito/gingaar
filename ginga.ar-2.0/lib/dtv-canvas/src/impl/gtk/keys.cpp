#include <glib.h>
#include <gdk/gdkkeysyms.h>
#include <util/log.h>
#include "system.h"


namespace canvas {
namespace gtk {

util::key::type getKeyCode( guint symbol ) {
	util::key::type key;

	switch (symbol) {
		case GDK_KEY_0				: key = (util::key::number_0);			break;
		case GDK_KEY_1				: key = (util::key::number_1);			break;
		case GDK_KEY_2				: key = (util::key::number_2);			break;
		case GDK_KEY_3				: key = (util::key::number_3);			break;
		case GDK_KEY_4				: key = (util::key::number_4);			break;
		case GDK_KEY_5				: key = (util::key::number_5);			break;
		case GDK_KEY_6				: key = (util::key::number_6);			break;
		case GDK_KEY_7				: key = (util::key::number_7);			break;
		case GDK_KEY_8				: key = (util::key::number_8);			break;
		case GDK_KEY_9				: key = (util::key::number_9);			break;

		case GDK_KEY_a				: key = (util::key::small_a);			break;
		case GDK_KEY_b				: key = (util::key::small_b);			break;
		case GDK_KEY_c				: key = (util::key::small_c);			break;
		case GDK_KEY_d				: key = (util::key::small_d);			break;
		case GDK_KEY_e				: key = (util::key::small_e);			break;
		case GDK_KEY_f				: key = (util::key::small_f);			break;
		case GDK_KEY_g				: key = (util::key::small_g);			break;
		case GDK_KEY_h				: key = (util::key::small_h);			break;
		case GDK_KEY_i				: key = (util::key::small_i);			break;
		case GDK_KEY_j				: key = (util::key::small_j);			break;
		case GDK_KEY_k				: key = (util::key::small_k);			break;
		case GDK_KEY_l				: key = (util::key::small_l);			break;
		case GDK_KEY_m				: key = (util::key::small_m);			break;
		case GDK_KEY_n				: key = (util::key::small_n);			break;
		case GDK_KEY_o				: key = (util::key::small_o);			break;
		case GDK_KEY_p				: key = (util::key::small_p);			break;
		case GDK_KEY_q				: key = (util::key::small_q);			break;
		case GDK_KEY_r				: key = (util::key::small_r);			break;
		case GDK_KEY_s				: key = (util::key::small_s);			break;
		case GDK_KEY_t				: key = (util::key::small_t);			break;
		case GDK_KEY_u				: key = (util::key::small_u);			break;
		case GDK_KEY_v				: key = (util::key::small_v);			break;
		case GDK_KEY_w				: key = (util::key::small_w);			break;
		case GDK_KEY_x				: key = (util::key::small_x);			break;
		case GDK_KEY_y				: key = (util::key::small_y);			break;
		case GDK_KEY_z				: key = (util::key::small_z);			break;

		case GDK_KEY_A				: key = (util::key::capital_a);			break;
		case GDK_KEY_B				: key = (util::key::capital_b);			break;
		case GDK_KEY_C				: key = (util::key::capital_c);			break;
		case GDK_KEY_D				: key = (util::key::capital_d);			break;
		case GDK_KEY_E				: key = (util::key::capital_e);			break;
		case GDK_KEY_F				: key = (util::key::capital_f);			break;
		case GDK_KEY_G				: key = (util::key::capital_g);			break;
		case GDK_KEY_H				: key = (util::key::capital_h);			break;
		case GDK_KEY_I				: key = (util::key::capital_i);			break;
		case GDK_KEY_J				: key = (util::key::capital_j);			break;
		case GDK_KEY_K				: key = (util::key::capital_k);			break;
		case GDK_KEY_L				: key = (util::key::capital_l);			break;
		case GDK_KEY_M				: key = (util::key::capital_m);			break;
		case GDK_KEY_N				: key = (util::key::capital_n);			break;
		case GDK_KEY_O				: key = (util::key::capital_o);			break;
		case GDK_KEY_P				: key = (util::key::capital_p);			break;
		case GDK_KEY_Q				: key = (util::key::capital_q);			break;
		case GDK_KEY_R				: key = (util::key::capital_r);			break;
		case GDK_KEY_S				: key = (util::key::capital_s);			break;
		case GDK_KEY_T				: key = (util::key::capital_t);			break;
		case GDK_KEY_U				: key = (util::key::capital_u);			break;
		case GDK_KEY_V				: key = (util::key::capital_v);			break;
		case GDK_KEY_W				: key = (util::key::capital_w);			break;
		case GDK_KEY_X				: key = (util::key::capital_x);			break;
		case GDK_KEY_Y				: key = (util::key::capital_y);			break;
		case GDK_KEY_Z				: key = (util::key::capital_z);			break;

		case GDK_KEY_Page_Down			: key = (util::key::page_down);			break;
		case GDK_KEY_Page_Up			: key = (util::key::page_up);			break;

		case GDK_KEY_F1				: key = (util::key::f1);			break;
		case GDK_KEY_F2				: key = (util::key::f2);			break;
		case GDK_KEY_F3				: key = (util::key::f3);			break;
		case GDK_KEY_F4				: key = (util::key::f4);			break;
		case GDK_KEY_F5				: key = (util::key::f5);			break;
		case GDK_KEY_F6				: key = (util::key::f6);			break;
		case GDK_KEY_F7				: key = (util::key::f7);			break;
		case GDK_KEY_F8				: key = (util::key::f8);			break;
		case GDK_KEY_F9				: key = (util::key::f9);			break;
		case GDK_KEY_F10			: key = (util::key::f10);			break;
		case GDK_KEY_F11			: key = (util::key::f11);			break;
		case GDK_KEY_F12			: key = (util::key::f12);			break;

		case GDK_KEY_plus			: key = (util::key::plus_sign);			break;
		case GDK_KEY_minus			: key = (util::key::minus_sign);		break;
		case GDK_KEY_asterisk			: key = (util::key::asterisk);			break;
		case GDK_KEY_period			: key = (util::key::period);			break;

		case GDK_KEY_Print			: key = (util::key::printscreen);		break;

		case GDK_KEY_Super_L:
		case GDK_KEY_Super_R:
							  key = (util::key::super);			break;

		case GDK_KEY_Menu			: key = (util::key::menu);			break;
		case GDK_KEY_Red			: key = (util::key::red);			break;
		case GDK_KEY_Green			: key = (util::key::green);			break;
		case GDK_KEY_Yellow			: key = (util::key::yellow);			break;
		case GDK_KEY_Blue			: key = (util::key::blue);			break;
		case GDK_KEY_Down			: key = (util::key::cursor_down);		break;
		case GDK_KEY_Left			: key = (util::key::cursor_left);		break;
		case GDK_KEY_Right			: key = (util::key::cursor_right);		break;
		case GDK_KEY_Up				: key = (util::key::cursor_up);			break;

		case GDK_KEY_Return			: key = (util::key::enter);			break;

		case GDK_KEY_BackSpace			: key = (util::key::backspace);			break;
		case GDK_KEY_Escape			: key = (util::key::escape);			break;

		case GDK_KEY_PowerOff			: key = (util::key::power);			break;
		case GDK_KEY_Pause			: key = (util::key::pause);			break;

		case GDK_KEY_greater			: key = (util::key::greater_than_sign);		break;
		case GDK_KEY_less			: key = (util::key::less_than_sign); 		break;

		case GDK_KEY_Back			: key = (util::key::back);			break;
		case GDK_KEY_Exit			: key = (util::key::exit);			break;

		default:
			LDEBUG("canvas::gtk", "[%s::%d] Key [%d(0x%x)] is not mapped.", __FUNCTION__, __LINE__, symbol, symbol);
			key = (util::key::null);
			break;
	};

	return key;
}

guint getGdkKeyCode(util::key::type symbol) {
	guint key;

	switch (symbol) {
		case (util::key::number_0)		: key = GDK_KEY_0;				break;
		case (util::key::number_1)		: key = GDK_KEY_1;				break;
		case (util::key::number_2)		: key = GDK_KEY_2;				break;
		case (util::key::number_3)		: key = GDK_KEY_3;				break;
		case (util::key::number_4)		: key = GDK_KEY_4;				break;
		case (util::key::number_5)		: key = GDK_KEY_5;				break;
		case (util::key::number_6)		: key = GDK_KEY_6;				break;
		case (util::key::number_7)		: key = GDK_KEY_7;				break;
		case (util::key::number_8)		: key = GDK_KEY_8;				break;
		case (util::key::number_9)		: key = GDK_KEY_9;				break;

		case (util::key::small_a)		: key = GDK_KEY_a;				break;
		case (util::key::small_b)		: key = GDK_KEY_b;				break;
		case (util::key::small_c)		: key = GDK_KEY_c;				break;
		case (util::key::small_d)		: key = GDK_KEY_d;				break;
		case (util::key::small_e)		: key = GDK_KEY_e;				break;
		case (util::key::small_f)		: key = GDK_KEY_f;				break;
		case (util::key::small_g)		: key = GDK_KEY_g;				break;
		case (util::key::small_h)		: key = GDK_KEY_h;				break;
		case (util::key::small_i)		: key = GDK_KEY_i;				break;
		case (util::key::small_j)		: key = GDK_KEY_j;				break;
		case (util::key::small_k)		: key = GDK_KEY_k;				break;
		case (util::key::small_l)		: key = GDK_KEY_l;				break;
		case (util::key::small_m)		: key = GDK_KEY_m;				break;
		case (util::key::small_n)		: key = GDK_KEY_n;				break;
		case (util::key::small_o)		: key = GDK_KEY_o;				break;
		case (util::key::small_p)		: key = GDK_KEY_p;				break;
		case (util::key::small_q)		: key = GDK_KEY_q;				break;
		case (util::key::small_r)		: key = GDK_KEY_r;				break;
		case (util::key::small_s)		: key = GDK_KEY_s;				break;
		case (util::key::small_t)		: key = GDK_KEY_t;				break;
		case (util::key::small_u)		: key = GDK_KEY_u;				break;
		case (util::key::small_v)		: key = GDK_KEY_v;				break;
		case (util::key::small_w)		: key = GDK_KEY_w;				break;
		case (util::key::small_x)		: key = GDK_KEY_x;				break;
		case (util::key::small_y)		: key = GDK_KEY_y;				break;
		case (util::key::small_z)		: key = GDK_KEY_z;				break;

		case (util::key::capital_a)		: key = GDK_KEY_A;				break;
		case (util::key::capital_b)		: key = GDK_KEY_B;				break;
		case (util::key::capital_c)		: key = GDK_KEY_C;				break;
		case (util::key::capital_d)		: key = GDK_KEY_D;				break;
		case (util::key::capital_e)		: key = GDK_KEY_E;				break;
		case (util::key::capital_f)		: key = GDK_KEY_F;				break;
		case (util::key::capital_g)		: key = GDK_KEY_G;				break;
		case (util::key::capital_h)		: key = GDK_KEY_H;				break;
		case (util::key::capital_i)		: key = GDK_KEY_I;				break;
		case (util::key::capital_j)		: key = GDK_KEY_J;				break;
		case (util::key::capital_k)		: key = GDK_KEY_K;				break;
		case (util::key::capital_l)		: key = GDK_KEY_L;				break;
		case (util::key::capital_m)		: key = GDK_KEY_M;				break;
		case (util::key::capital_n)		: key = GDK_KEY_N;				break;
		case (util::key::capital_o)		: key = GDK_KEY_O;				break;
		case (util::key::capital_p)		: key = GDK_KEY_P;				break;
		case (util::key::capital_q)		: key = GDK_KEY_Q;				break;
		case (util::key::capital_r)		: key = GDK_KEY_R;				break;
		case (util::key::capital_s)		: key = GDK_KEY_S;				break;
		case (util::key::capital_t)		: key = GDK_KEY_T;				break;
		case (util::key::capital_u)		: key = GDK_KEY_U;				break;
		case (util::key::capital_v)		: key = GDK_KEY_V;				break;
		case (util::key::capital_w)		: key = GDK_KEY_W;				break;
		case (util::key::capital_x)		: key = GDK_KEY_X;				break;
		case (util::key::capital_y)		: key = GDK_KEY_Y;				break;
		case (util::key::capital_z)		: key = GDK_KEY_Z;				break;

		case (util::key::page_down)		: key = GDK_KEY_Page_Down;			break;
		case (util::key::page_up)		: key = GDK_KEY_Page_Up;			break;

		case (util::key::f1)			: key = GDK_KEY_F1;				break;
		case (util::key::f2)			: key = GDK_KEY_F2;				break;
		case (util::key::f3)			: key = GDK_KEY_F3;				break;
		case (util::key::f4)			: key = GDK_KEY_F4;				break;
		case (util::key::f5)			: key = GDK_KEY_F5;				break;
		case (util::key::f6)			: key = GDK_KEY_F6;				break;
		case (util::key::f7)			: key = GDK_KEY_F7;				break;
		case (util::key::f8)			: key = GDK_KEY_F8;				break;
		case (util::key::f9)			: key = GDK_KEY_F9;				break;
		case (util::key::f10)			: key = GDK_KEY_F10;				break;
		case (util::key::f11)			: key = GDK_KEY_F11;				break;
		case (util::key::f12)			: key = GDK_KEY_F12;				break;

		case (util::key::plus_sign)		: key = GDK_KEY_plus;				break;
		case (util::key::minus_sign)		: key = GDK_KEY_minus;				break;

		case (util::key::asterisk)		: key = GDK_KEY_asterisk;			break;

		case (util::key::period)		: key = GDK_KEY_period;				break;

		case (util::key::printscreen)		: key = GDK_KEY_Print;				break;

		case (util::key::super)			: key = GDK_KEY_Super_R;			break;
		case (util::key::menu)			: key = GDK_KEY_Menu;				break;
		case (util::key::red)			: key = GDK_KEY_Red;				break;
		case (util::key::green)			: key = GDK_KEY_Green;				break;
		case (util::key::yellow)		: key = GDK_KEY_Yellow;				break;
		case (util::key::blue)			: key = GDK_KEY_Blue;				break;
		case (util::key::cursor_down)		: key = GDK_KEY_Down;				break;
		case (util::key::cursor_left)		: key = GDK_KEY_Left;				break;
		case (util::key::cursor_right)		: key = GDK_KEY_Right;				break;
		case (util::key::cursor_up)		: key = GDK_KEY_Up;				break;

		case (util::key::enter)			: key = GDK_KEY_Return;				break;

		case (util::key::backspace)		: key = GDK_KEY_BackSpace;			break;
		case (util::key::back)			: key = GDK_KEY_BackSpace;			break;
		case (util::key::escape)		: key = GDK_KEY_Escape;				break;
		case (util::key::exit)			: key = GDK_KEY_Escape;				break;

		case (util::key::power)			: key = GDK_KEY_PowerOff;			break;
		case (util::key::pause)			: key = GDK_KEY_Pause;				break;

		case (util::key::greater_than_sign)	: key = GDK_KEY_greater;			break;
		case (util::key::less_than_sign)	: key = GDK_KEY_less;				break;

		default:
			LDEBUG("canvas::gtk", "[%s::%d] Key [%d(0x%x)] is not mapped.", __FUNCTION__, __LINE__, symbol, symbol);
			key = 0;
			break;
	};

	return key;
}

}
}
