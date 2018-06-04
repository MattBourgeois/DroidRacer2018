from droidcv import *
import numpy as np

new_vid = VideoFeed(0)
pos_logic = PositionLogic(20, 4, 1)  # arguments: +/- ignore range, false pos range (normal) &, (larger/missing lane)

# infinite loop that finds up to 10 rois per lane per frame, breaks out if q is pressed
while True:
    # Update video feed frame and being hsvcv logic
    new_vid.update(True)
    new_logic = HSVcv(new_vid.frame, BLUE_MIN, BLUE_MAX, YELLOW_MIN, YELLOW_MAX)

    # scan for 2 lanes, 10 sample areas each
    for i in range(10):
        cv2.imshow('hsv', new_logic.hsv)
        new_logic.find_next()
        new_logic.black_fill_last_roi()

    # prepare position logic to feed into droid
    pos_logic.update(new_vid.full_frame, np.array(new_logic.roi_lists['LeftLane']),
                              np.array(new_logic.roi_lists['RightLane']))
    pos_logic.draw_middle(HEIGHT)
    pos_logic.draw_lanes(HEIGHT)
    direction = pos_logic.calc_direction()
    print(direction)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
