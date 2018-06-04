import cv2
import numpy as np

BLUE_MIN = np.array([90, 80, 0], np.uint8)
BLUE_MAX = np.array([110, 255, 255], np.uint8)
YELLOW_MIN = np.array([20, 150, 100], np.uint8)
YELLOW_MAX = np.array([40, 255, 255], np.uint8)
RED_MIN = np.array([170, 190, 150], np.uint8)
RED_MAX = np.array([190, 255, 255], np.uint8)
HEIGHT = 200  # px
SAMPLE_SIZE = 25


class VideoFeed(object):
    def __init__(self, cam_select=0):
        self.cap = cv2.VideoCapture(cam_select)
        self.ret, self.full_frame = self.cap.read()
        self.frame = self.full_frame

    def __del__(self):
        self.cap.release()
        cv2.destroyAllWindows()

    def update(self, crop_check=False):
        self.ret, self.full_frame = self.cap.read()  # READ FRAME
        self.auto_crop()
        self.frame = self.full_frame
        if crop_check:
            self.crop()

    def auto_crop(self, threshold=0):
        if len(self.full_frame.shape) == 3:
            flat_image = np.max(self.full_frame, 2)
        else:
            flat_image = self.full_frame
        assert len(flat_image.shape) == 2

        rows = np.where(np.max(flat_image, 0) > threshold)[0]
        if rows.size:
            cols = np.where(np.max(flat_image, 1) > threshold)[0]
            crop = self.full_frame[cols[0]: cols[-1] + 1, rows[0]: rows[-1] + 1]
        else:
            crop = self.full_frame[:1, :1]

        # Now crop the image, and save to frame
        self.full_frame = crop.copy()

    def crop(self):
        x = self.frame.shape[1]
        y = self.frame.shape[0]
        self.frame = self.frame[y - HEIGHT:y, 0:x].copy()

    def show(self, name='feed', next_frame=True):
        if self.ret:
            cv2.imshow(name, self.full_frame)
        if next_frame:
            self.update()


class HSVcv(object):

    def __init__(self, frame, blue_min, blue_max, yellow_min, yellow_max):
        self.bgr = frame.copy()
        self.hsv = cv2.cvtColor(self.bgr, cv2.COLOR_BGR2HSV)
        self.y, self.x = self.hsv.shape[0], self.hsv.shape[1]
        self.median_hsv = {'LeftLane': [blue_min.copy(), blue_max.copy()],
                           'RightLane': [yellow_min.copy(), yellow_max.copy()]}
        first_rois = self.find_first().copy()
        self.roi_lists = {'LeftLane': [first_rois[0]], 'RightLane': [first_rois[1]]}
        self.black_fill_last_roi()

    # Find first two Regions of interest (fist point on both lanes) and add them to roi_list
    def find_first(self, s=SAMPLE_SIZE):
        pos_yx = [[0, 0, 0, -1], [0, 0, 0, -1]]  # default values for defining roi, [[max count, y, x, segment], ...]

        # Check left and right borders of frame against respective thresholds ( left = blue, right = yellow)
        y = 0
        while y < self.y - s:
            left_lane_selected_left_roi = frame2roi(y, 0, self.hsv, self.median_hsv['LeftLane'])
            right_lane_selected_right_roi = frame2roi(y, self.x - SAMPLE_SIZE, self.hsv, self.median_hsv['RightLane'])
            left_nonzero_count = cv2.countNonZero(left_lane_selected_left_roi)
            right_nonzero_count = cv2.countNonZero(right_lane_selected_right_roi)
            # Find max nonzero count regions of interest and set y and x position
            if left_nonzero_count > pos_yx[0][0]:
                pos_yx[0][0] = left_nonzero_count
                pos_yx[0][1] = y
                pos_yx[0][2] = 0

            if right_nonzero_count > pos_yx[1][0]:
                pos_yx[1][0] = right_nonzero_count
                pos_yx[1][1] = y
                pos_yx[1][2] = self.x - s

            y += s

        # # Check bottom border of frame against both thresholds (left = blue, rihgt = yellow)
        x = 0
        while x < self.x - s:
            bottom_lane_selected_left_roi = frame2roi(self.y - s, x, self.hsv, self.median_hsv['LeftLane'])
            bottom_lane_selected_right_roi = frame2roi(self.y - s, x, self.hsv, self.median_hsv['RightLane'])
            bottoml_leftr_nonzero_count = cv2.countNonZero(bottom_lane_selected_left_roi)
            bottoml_rightr_nonzero_count = cv2.countNonZero(bottom_lane_selected_right_roi)

            if bottoml_leftr_nonzero_count > pos_yx[0][0]:
                pos_yx[0][0] = bottoml_leftr_nonzero_count
                pos_yx[0][1] = y - s
                pos_yx[0][2] = x

            if bottoml_rightr_nonzero_count > pos_yx[1][0]:
                pos_yx[1][0] = bottoml_rightr_nonzero_count
                pos_yx[1][1] = self.y - s
                pos_yx[1][2] = x

            x += s
        return pos_yx.copy()

    def avoid_obstacle(self, avoid_threshold_min, avoid_threshold_max, sample_select_index=0, s=SAMPLE_SIZE):
        left_sample_select_x = self.roi_lists['LeftLane'][sample_select_index][2]
        right_sample_select_x = self.roi_lists['RightLane'][sample_select_index][2]
        sample_select_y = self.roi_lists['LeftLane'][sample_select_index][1]
        x = left_sample_select_x
        max_nonzero_count = 0
        max_nonzero_x = left_sample_select_x
        hsv_threshold = [avoid_threshold_min.copy(), avoid_threshold_max.copy()]

        while left_sample_select_x <= x <= right_sample_select_x:
            selected_sample = frame2roi(sample_select_y, x, self.hsv, hsv_threshold, s)
            sample_select_nonzero_count = cv2.countNonZero(selected_sample)
            if sample_select_nonzero_count > max_nonzero_count:
                max_nonzero_count = sample_select_nonzero_count
                max_nonzero_x = x
            x += s

        print('l', abs(max_nonzero_x - left_sample_select_x))
        print('r', abs(max_nonzero_x - right_sample_select_x))
        if abs(max_nonzero_x - left_sample_select_x) < abs(max_nonzero_x - right_sample_select_x):
            selected_lane = 'LeftLane'
            unselected_lane = 'RightLane'
        else:
            selected_lane = 'RightLane'
            unselected_lane = 'LeftLane'

        if max_nonzero_count > 3:
            for index in range(len(self.roi_lists[selected_lane])):
                del self.roi_lists[selected_lane][-1]
            y = self.roi_lists[unselected_lane][0][1]
            while y >= 0:
                self.roi_lists[selected_lane].append([0, y, max_nonzero_x, -1])
                y -= SAMPLE_SIZE

    # Finds the next two regions of interest (next point on both lanes) and add to roi_list, can disable second lane
    def find_next(self, double_lane=True, s=SAMPLE_SIZE):
        for lane in self.roi_lists:
            nth_roi = self.roi_lists[lane][-1]
            if nth_roi == [0, 0, 0, 0]:
                continue

            nth_y = nth_roi[1]
            nth_x = nth_roi[2]

            surrounding_y = self.bound2frame(
                [nth_y - s, nth_y - int(s * 0.75), nth_y - s, nth_y, nth_y + s, nth_y + int(s * 0.75), nth_y + s,
                 nth_y], 'y')

            surrounding_x = self.bound2frame(
                [nth_x - s, nth_x, nth_x + s, nth_x + s, nth_x + s, nth_x, nth_x - s, nth_x - s], 'x')

            max_nonzero_count, segment_pos = 0, 0
            next_roi, segment_weight = [0] * 4, [1] * 8
            nth_segment = nth_roi[3]
            if nth_segment >= 0:
                segment_weight = rshift([1, 0.9, 0.7, 0.2, 0, 0.2, 0.7, 0.9], nth_segment)

            # disable positions 7 6 5 ( negative y segments)
            neg_y_disable = 0

            # Find max nonzero roi from 5 segments around nth roi on both lanes
            for y, x in zip(surrounding_y, surrounding_x):
                left_nonzero_count = cv2.countNonZero(frame2roi(y, x, self.hsv, self.median_hsv[str(lane)]))
                if 4 <= neg_y_disable <= 6:
                    left_nonzero_count = 0

                left_nonzero_count = int(left_nonzero_count * segment_weight[segment_pos])
                if left_nonzero_count > max_nonzero_count:
                    max_nonzero_count = left_nonzero_count
                    next_roi = [max_nonzero_count, y, x, segment_pos]

                segment_pos += 1
                neg_y_disable += 1
            self.roi_lists[lane].append(next_roi)
            self.next_median_hsv(next_roi, str(lane))
            if not double_lane:
                return

    def next_median_hsv(self, next_roi, lane, s=SAMPLE_SIZE):
        y, x = next_roi[1], next_roi[2]
        roi_bgr = frame2roi(y, x, self.bgr)

        # Finds new hue thresholds based on median color of masked next region of interest
        roi_mask = frame2roi(y, x, self.hsv, self.median_hsv[lane])
        roi_bgr_masked = cv2.bitwise_and(roi_bgr, roi_bgr, mask=roi_mask)
        roi_bgr_masked[np.where((roi_bgr_masked == [0, 0, 0]).all(axis=2))] = [255, 255, 255]
        roi_blur = cv2.blur(roi_bgr_masked, (100, 100))
        roi_hsv = cv2.cvtColor(roi_blur, cv2.COLOR_BGR2HSV)
        min_hsv, max_hsv = self.median_hsv[lane][0], self.median_hsv[lane][1]
        median_hsv_color = roi_hsv[0][0][0]
        mod_check = np.clip(next_roi[0], 1, s * s)
        hsv_mod_hue = (1 - (mod_check / (s * s))) / 2
        min_hsv[0] = int(median_hsv_color * (0.9 - hsv_mod_hue))
        max_hsv[0] = int(median_hsv_color * (1.1 + hsv_mod_hue))

        # Saturation mod
        if self.median_hsv[lane][-1][0] < 10:
            self.median_hsv[lane][1] = int(min_hsv[1] * 0.1)

        self.median_hsv[lane] = [np.array(min_hsv), np.array(max_hsv)]

    def bound2frame(self, to_bound, axis=None, s=SAMPLE_SIZE):
        if axis is 'y':
            boundary = self.hsv.shape[0] - s
        elif axis is 'x':
            boundary = self.hsv.shape[1] - s
        else:
            print("This value is was not bounded")
            return -1

        return np.clip(to_bound, 0, boundary)

    def black_fill_last_roi(self, s=SAMPLE_SIZE):
        for lane in self.roi_lists:
            y = self.roi_lists[lane][-1][1]
            x = self.roi_lists[lane][-1][2]
            cv2.rectangle(self.hsv, (x, y), (x + s, y + s), (0, 0, 0), -1)


class PositionLogic(object):
    def __init__(self, ignore_range=0, fp_check1=0, fp_check2=0, ):
        self.frame = None
        self.left_roi_list = None
        self.right_roi_list = None
        self.frame_to_draw = None
        self.middle_yx_list = None
        self.roi_max_count = None
        self.ignore_range = ignore_range
        self.false_positive_range1= fp_check1
        self.false_positive_range2 = fp_check2
        self.false_positive_counter1 = 0
        self.false_positive_counter2 = 0

    def update(self, frame, left_roi_list, right_roi_list):
        self.left_roi_list = left_roi_list
        self.right_roi_list = right_roi_list
        self.frame = frame.copy()
        self.frame_to_draw = self.frame.copy()
        self.middle_yx_list = [[]]
        if len(left_roi_list) <= len(right_roi_list):
            self.roi_max_count = len(left_roi_list)
        else:
            self.roi_max_count = len(right_roi_list)

        for index in range(self.roi_max_count):
            # Find closest roi's on y axis
            nearest_y_index = find_nearest(self.left_roi_list[:, 1], self.right_roi_list[index][1])

            # Find middle x position
            middle_x = self.right_roi_list[index][2] - self.left_roi_list[nearest_y_index][2]
            middle_x = self.left_roi_list[index][2] + (middle_x / 2)

            # Find middle y position
            if self.left_roi_list[index][1] > self.right_roi_list[index][1]:
                middle_y = self.left_roi_list[index][1] - self.right_roi_list[index][1]
                middle_y = self.right_roi_list[index][1] + middle_y
            else:
                middle_y = self.right_roi_list[index][1] - self.left_roi_list[index][1]
                middle_y = self.left_roi_list[index][1] + middle_y

            if middle_y == 0:
                continue
            self.middle_yx_list.append([middle_y, middle_x])
        self.middle_yx_list.pop(0)

    def draw_middle(self, h=0):
        middle_max = len(self.middle_yx_list) - 2
        middle_index = 0
        while middle_index < middle_max:
            y1 = int(self.middle_yx_list[middle_index][0] + h)
            x1 = int(self.middle_yx_list[middle_index][1])
            y2 = int(self.middle_yx_list[middle_index + 1][0] + h)
            x2 = int(self.middle_yx_list[middle_index + 1][1])
            cv2.line(self.frame_to_draw, (x1, y1), (x2, y2), (255, 255, 0), 5)
            middle_index += 1

    def draw_lanes(self, h=0):
        for lane_index in range(len(self.left_roi_list) - 1):
            # Get positions for drawing left lane
            left_y1 = int(self.left_roi_list[lane_index][1] + h)
            left_x1 = self.left_roi_list[lane_index][2]
            left_y2 = int(self.left_roi_list[lane_index + 1][1] + h)
            left_x2 = self.left_roi_list[lane_index + 1][2]

            # draw left lane
            if self.left_roi_list[lane_index + 1][0] != 0:
                cv2.line(self.frame_to_draw, (left_x1, left_y1), (left_x2, left_y2), (0, 0, 0), 5)

        for lane_index in range(len(self.right_roi_list) - 1):
            # Get positions for drawing right lane
            right_y1 = int(self.right_roi_list[lane_index][1] + h)
            right_x1 = self.right_roi_list[lane_index][2]
            right_y2 = int(self.right_roi_list[lane_index + 1][1] + h)
            right_x2 = self.right_roi_list[lane_index + 1][2]

            # draw right lane
            if self.right_roi_list[lane_index + 1][0] != 0:
                cv2.line(self.frame_to_draw, (right_x1, right_y1), (right_x2, right_y2), (0, 0, 0), 5)

    def show(self):
        cv2.imshow('Drawn Lanes', self.frame_to_draw)

    def calc_direction(self):
        x_list = [x[1] for x in self.middle_yx_list]
        average_x = average_list(x_list)
        if average_x == 0:
            return 0
        else:
            true_middle_x = self.frame.shape[1] / 2
            dif_average_x = average_x - true_middle_x

            # ignore dif_average_x that fall under +/- ignore_range
            dif_average_x = self.ignore_check(int(dif_average_x))

            # Check to see if one of the lanes is much more visible than then other (used to check if getting to close
            # to lane)
            total_left_lane_nonzero = np.sum(self.left_roi_list[:, 0])
            total_right_lane_nonzero = np.sum(self.right_roi_list[:, 0])
            if total_left_lane_nonzero > (total_right_lane_nonzero * 3):
                dif_average_x += -total_left_lane_nonzero
                self.false_positive_counter2 += -1
            elif total_right_lane_nonzero > (total_left_lane_nonzero * 3):
                dif_average_x += total_right_lane_nonzero
                self.false_positive_counter2 += 1

            # check for false positives and ignore
            self.false_positive_counter1 += np.clip(dif_average_x, -1, 1)
            if -self.false_positive_range1 < self.false_positive_counter1 < self.false_positive_range1:
                if -self.false_positive_range2 < self.false_positive_counter2 < self.false_positive_range2:
                    dif_average_x = 0
            if abs(self.false_positive_counter1) > self.false_positive_range1:
                self.false_positive_counter1 = 0
            if abs(self.false_positive_counter2) > self.false_positive_range2:
                self.false_positive_counter2 = 0
            return int(dif_average_x)

    # ignore check to check against init dead zone
    def ignore_check(self, dif_avg_x):
        if -self.ignore_range <= dif_avg_x <= self.ignore_range:
            return 0
        else:
            return dif_avg_x


def rshift(seq, n=0):  # Rotational shift of array by n
    a = n % len(seq)
    return seq[-a:] + seq[:-a]


def frame2roi(y, x, frame, median_hsv=None, size=SAMPLE_SIZE):
    roi = frame[y: y + size, x: x + size].copy()
    if median_hsv is not None:
        return cv2.inRange(roi, median_hsv[0], median_hsv[1])
    else:
        return roi


def find_nearest(array, value):
    array = np.asarray(array)
    idx = (np.abs(array - value)).argmin()
    return idx


def average_list(array):
    if len(array) == 0:
        return 0
    return sum(array)/len(array)