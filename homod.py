def n_events(width, depth):
    calc_in = lambda x, w: 1 + (x - 1) * (w - 1)
    exp_trans = 1
    tr_atomics = sum(range(1, width))
    for i in range(1, depth):
        num_inputs = calc_in(i, width)
        trans_first_row = (width - 1) * (num_inputs + width - 1)
        exp_trans += num_inputs * tr_atomics + trans_first_row
    return exp_trans


def n_atomics(width, depth):
    return (width - 1 + (width - 1) * width / 2) * (depth - 1) + 1
