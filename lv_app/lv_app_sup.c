/**
 * @file lv_app_sup.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl/lv_app/lv_app_sup.h>
#include "../lv_objx/lv_btnm.h"
#include "../lv_objx/lv_ta.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_action_res_t lv_app_kb_action(lv_obj_t * btnm, uint16_t i);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * kb_btnm;
static lv_obj_t * kb_win;
static lv_obj_t * kb_ta;
static const char * kb_map_lc[] = {
"\0061#", "\004q", "\004w", "\004e", "\004r", "\004t", "\004y", "\004u", "\004i", "\004o", "\004p", "\010Del", "\n",
"\007ABC", "\004a", "\004s", "\004d", "\004f", "\004g", "\004h", "\004j", "\004k", "\004l", "\010Enter", "\n",
"*", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ";", "\n",
"\002Hide", "\002Left", "\006 ", "\002Right", "\002Ok", ""
};

static const char * kb_map_uc[] = {
"\0061#", "\004Q", "\004W", "\004E", "\004R", "\004T", "\004Y", "\004U", "\004I", "\004O", "\004P", "\010Del", "\n",
"\007abc", "\004A", "\004S", "\004D", "\004F", "\004G", "\004H", "\004J", "\004K", "\004L", "\010Enter", "\n",
"*", "/", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ";", "\n",
"\002Hide", "\002Left", "\006 ", "\002Right", "\002Ok", ""
};

static const char * kb_map_spec[] = {
"0", "1", "2", "3", "4", "5", "6", "4", "8", "9", "\002Del", "\n",
"\002abc", "+", "-", "=", "%", "!", "?", "#", "<", ">", "\002Enter", "\n",
"\\", "@", "$", "_", "(", ")", "{", "}", "[", "]", ":", "\"", "'", "\n",
"\002Hide", "\002Left", "\006 ", "\002Right", "\002Ok", ""
};

static const char * kb_map_num[] = {
"1", "2", "3", "\002Hide","\n",
"4", "5", "6", "\002Ok", "\n",
"7", "8", "9", "\002Del", "\n",
"+/-", "0", ".", "Left", "Right", ""
};

static cord_t kb_ta_ori_size;
static uint8_t kb_mode;
static void (*kb_close_action)(lv_obj_t *);
static void (*kb_ok_action)(lv_obj_t *);
static lv_btnms_t kb_btnms;
static bool kb_inited;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_app_kb_open(lv_obj_t * ta, lv_app_kb_mode_t mode, void (*close)(lv_obj_t *), void (*ok)(lv_obj_t *))
{
	if(kb_inited == false) {
		lv_btnms_get(LV_BTNMS_DEF, &kb_btnms);
		kb_btnms.rects.opad = 4 + LV_DOWNSCALE;
		kb_btnms.rects.vpad = 3 + LV_DOWNSCALE;
		kb_btnms.rects.hpad = 3 + LV_DOWNSCALE;
		kb_inited = true;
	}

    if(kb_btnm != NULL) {
        lv_app_kb_close(false);
    }

    kb_ta = ta;
    kb_mode = mode;
    kb_close_action = close;
    kb_ok_action = ok;

    kb_btnm = lv_btnm_create(lv_scr_act(), NULL);
    lv_obj_set_size(kb_btnm, LV_HOR_RES, LV_VER_RES / 2);
    lv_obj_align(kb_btnm, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_obj_set_style(kb_btnm, &kb_btnms);
    lv_btnm_set_cb(kb_btnm, lv_app_kb_action);
    if(mode & LV_APP_KB_MODE_TXT) lv_btnm_set_map(kb_btnm, kb_map_lc);
    else if(mode & LV_APP_KB_MODE_NUM) lv_btnm_set_map(kb_btnm, kb_map_num);

    kb_win = lv_app_get_win_from_obj(kb_ta);
    lv_obj_set_height(kb_win, LV_VER_RES / 2);
    lv_obj_set_y(kb_win, 0);

	lv_app_style_t * app_style = lv_app_get_style();
    cord_t win_cont_h = lv_obj_get_height(lv_win_get_content(kb_win)) -  2 * app_style->win_style.content.scrable_rects.vpad;
	kb_ta_ori_size = lv_obj_get_height(kb_ta);
    if(lv_obj_get_height(kb_ta)  > win_cont_h) {
    	lv_obj_set_height(kb_ta, win_cont_h);
    }

    lv_ta_set_cursor_pos(kb_ta, LV_TA_CUR_LAST);

#if LV_APP_ANIM_LEVEL != 0
    lv_page_focus(lv_win_get_content(kb_win), kb_ta, true);
#else
    lv_page_focus(lv_win_get_content(kb_win), kb_ta, false);
#endif
}

void lv_app_kb_close(bool ok)
{
	if(kb_btnm == NULL) return;

	if(ok == false) {
		if(kb_close_action != NULL) kb_close_action(kb_ta);
	} else {
		if(kb_ok_action != NULL) kb_ok_action(kb_ta);
	}

	lv_obj_set_height(kb_ta, kb_ta_ori_size);

	lv_obj_set_size(kb_win, LV_HOR_RES, LV_VER_RES);
	kb_win = NULL;

    lv_obj_del(kb_btnm);
    kb_btnm = NULL;

    kb_ta = NULL;
}

static lv_action_res_t lv_app_kb_action(lv_obj_t * btnm, uint16_t i)
{
	const char ** map = lv_btnm_get_map(btnm);
	const char * txt = map[i];

	if(txt[0] <= '\011') txt++;

	if(strcmp(txt, "abc") == 0) {
		lv_btnm_set_map(btnm, kb_map_lc);
	} else if(strcmp(txt, "ABC") == 0) {
		lv_btnm_set_map(btnm, kb_map_uc);
	} else if(strcmp(txt, "1#") == 0) {
		lv_btnm_set_map(btnm, kb_map_spec);
	}  else if(strcmp(txt, "Enter") == 0) {
		lv_ta_add_char(kb_ta, '\n');
	} else if(strcmp(txt, "Left") == 0) {
		lv_ta_cursor_left(kb_ta);
	} else if(strcmp(txt, "Right") == 0) {
		lv_ta_cursor_right(kb_ta);
	} else if(strcmp(txt, "Del") == 0) {
		lv_ta_del(kb_ta);
	} else if(strcmp(txt, "+/-") == 0) {
		uint16_t cur = lv_ta_get_cursor_pos(kb_ta);
		const char * ta_txt = lv_ta_get_txt(kb_ta);
		if(ta_txt[0] == '-') {
			lv_ta_set_cursor_pos(kb_ta, 1);
			lv_ta_del(kb_ta);
			lv_ta_add_char(kb_ta, '+');
			lv_ta_set_cursor_pos(kb_ta, cur);
		} else if(ta_txt[0] == '+') {
			lv_ta_set_cursor_pos(kb_ta, 1);
			lv_ta_del(kb_ta);
			lv_ta_add_char(kb_ta, '-');
			lv_ta_set_cursor_pos(kb_ta, cur);
		} else {
			lv_ta_set_cursor_pos(kb_ta, 0);
			lv_ta_add_char(kb_ta, '-');
			lv_ta_set_cursor_pos(kb_ta, cur + 1);
		}
	} else if(strcmp(txt, "Hide") == 0) {
		lv_app_kb_close(false);
		return LV_ACTION_RES_INV;
	} else if(strcmp(txt, "Ok") == 0) {
		lv_app_kb_close(true);
		return LV_ACTION_RES_INV;
	} else {
		lv_ta_add_text(kb_ta, txt);
	}

#if LV_APP_ANIM_LEVEL != 0
	lv_page_focus(lv_win_get_content(kb_win), kb_ta, true);
#else
	lv_page_focus(lv_win_get_content(kb_win), kb_ta, false);
#endif
	return LV_ACTION_RES_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
