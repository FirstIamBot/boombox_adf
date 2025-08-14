"""
* Copyright 2023 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
"""

is_increse = 1
input_value = 0

def pageActive_textprogress_img_1_timer_cb():
    global input_value
    global is_increse
    if input_value >= 100:
        is_increse = 0
    if input_value <= 0:
        is_increse = 1
    if is_increse == 1:
        input_value += 1
        pageActive_textprogress_img_1.set_value(input_value)
        pageActive_img_1.set_style_img_opa(input_value, lv.PART.MAIN|lv.STATE.DEFAULT)
    if input_value == 100:
        input_value = 0
